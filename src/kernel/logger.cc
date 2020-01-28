/*=============================================================================
Project Z - Operating system for ARM processors
Copyright (C) 2018 Vladyslav Samodelok <vladfux4@gmail.com>
All rights reserved.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

=============================================================================*/
#include "kernel/logger.h"

// print code
extern "C" {

#define MMIO_BASE 0x3F000000

#define GPFSEL0 ((volatile unsigned int*)(MMIO_BASE + 0x00200000))
#define GPFSEL1 ((volatile unsigned int*)(MMIO_BASE + 0x00200004))
#define GPFSEL2 ((volatile unsigned int*)(MMIO_BASE + 0x00200008))
#define GPFSEL3 ((volatile unsigned int*)(MMIO_BASE + 0x0020000C))
#define GPFSEL4 ((volatile unsigned int*)(MMIO_BASE + 0x00200010))
#define GPFSEL5 ((volatile unsigned int*)(MMIO_BASE + 0x00200014))
#define GPSET0 ((volatile unsigned int*)(MMIO_BASE + 0x0020001C))
#define GPSET1 ((volatile unsigned int*)(MMIO_BASE + 0x00200020))
#define GPCLR0 ((volatile unsigned int*)(MMIO_BASE + 0x00200028))
#define GPLEV0 ((volatile unsigned int*)(MMIO_BASE + 0x00200034))
#define GPLEV1 ((volatile unsigned int*)(MMIO_BASE + 0x00200038))
#define GPEDS0 ((volatile unsigned int*)(MMIO_BASE + 0x00200040))
#define GPEDS1 ((volatile unsigned int*)(MMIO_BASE + 0x00200044))
#define GPHEN0 ((volatile unsigned int*)(MMIO_BASE + 0x00200064))
#define GPHEN1 ((volatile unsigned int*)(MMIO_BASE + 0x00200068))
#define GPPUD ((volatile unsigned int*)(MMIO_BASE + 0x00200094))
#define GPPUDCLK0 ((volatile unsigned int*)(MMIO_BASE + 0x00200098))
#define GPPUDCLK1 ((volatile unsigned int*)(MMIO_BASE + 0x0020009C))

/* a properly aligned buffer */
// extern volatile unsigned int mbox[36];

#define MBOX_REQUEST 0

/* channels */
#define MBOX_CH_POWER 0
#define MBOX_CH_FB 1
#define MBOX_CH_VUART 2
#define MBOX_CH_VCHIQ 3
#define MBOX_CH_LEDS 4
#define MBOX_CH_BTNS 5
#define MBOX_CH_TOUCH 6
#define MBOX_CH_COUNT 7
#define MBOX_CH_PROP 8

/* tags */
#define MBOX_TAG_GETSERIAL 0x10004
#define MBOX_TAG_SETCLKRATE 0x38002
#define MBOX_TAG_LAST 0

int mbox_call(unsigned char ch);

/* mailbox message buffer */
volatile static unsigned int __attribute__((aligned(16))) mbox[36];

#define VIDEOCORE_MBOX (MMIO_BASE + 0x0000B880)
#define MBOX_READ ((volatile unsigned int*)(VIDEOCORE_MBOX + 0x0))
#define MBOX_POLL ((volatile unsigned int*)(VIDEOCORE_MBOX + 0x10))
#define MBOX_SENDER ((volatile unsigned int*)(VIDEOCORE_MBOX + 0x14))
#define MBOX_STATUS ((volatile unsigned int*)(VIDEOCORE_MBOX + 0x18))
#define MBOX_CONFIG ((volatile unsigned int*)(VIDEOCORE_MBOX + 0x1C))
#define MBOX_WRITE ((volatile unsigned int*)(VIDEOCORE_MBOX + 0x20))
#define MBOX_RESPONSE 0x80000000
#define MBOX_FULL 0x80000000
#define MBOX_EMPTY 0x40000000

/**
 * Make a mailbox call. Returns 0 on failure, non-zero on success
 */
int mbox_call(unsigned char ch) {
  unsigned int r = (((unsigned int)((unsigned long)&mbox) & ~0xF) | (ch & 0xF));
  /* wait until we can write to the mailbox */
  do {
    asm volatile("nop");
  } while (*MBOX_STATUS & MBOX_FULL);
  /* write the address of our message to the mailbox with channel identifier */
  *MBOX_WRITE = r;
  /* now wait for the response */
  while (1) {
    /* is there a response? */
    do {
      asm volatile("nop");
    } while (*MBOX_STATUS & MBOX_EMPTY);
    /* is it a response to our message? */
    if (r == *MBOX_READ) /* is it a valid successful response? */
      return mbox[1] == MBOX_RESPONSE;
  }
  return 0;
}

#define UART0_DR ((volatile unsigned int*)(MMIO_BASE + 0x00201000))
#define UART0_FR ((volatile unsigned int*)(MMIO_BASE + 0x00201018))
#define UART0_IBRD ((volatile unsigned int*)(MMIO_BASE + 0x00201024))
#define UART0_FBRD ((volatile unsigned int*)(MMIO_BASE + 0x00201028))
#define UART0_LCRH ((volatile unsigned int*)(MMIO_BASE + 0x0020102C))
#define UART0_CR ((volatile unsigned int*)(MMIO_BASE + 0x00201030))
#define UART0_IMSC ((volatile unsigned int*)(MMIO_BASE + 0x00201038))
#define UART0_ICR ((volatile unsigned int*)(MMIO_BASE + 0x00201044))

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init() {
  unsigned int r;

  /* initialize UART */
  *UART0_CR = 0;  // turn off UART0

  /* set up clock for consistent divisor values */
  mbox[0] = 9 * 4;
  mbox[1] = MBOX_REQUEST;
  mbox[2] = MBOX_TAG_SETCLKRATE;  // set clock rate
  mbox[3] = 12;
  mbox[4] = 8;
  mbox[5] = 2;        // UART clock
  mbox[6] = 4000000;  // 4Mhz
  mbox[7] = 0;        // clear turbo
  mbox[8] = MBOX_TAG_LAST;
  mbox_call(MBOX_CH_PROP);

  /* map UART0 to GPIO pins */
  r = *GPFSEL1;
  r &= ~((7 << 12) | (7 << 15));  // gpio14, gpio15
  r |= (4 << 12) | (4 << 15);     // alt0
  *GPFSEL1 = r;
  *GPPUD = 0;  // enable pins 14 and 15
  r = 150;
  while (r--) {
    asm volatile("nop");
  }
  *GPPUDCLK0 = (1 << 14) | (1 << 15);
  r = 150;
  while (r--) {
    asm volatile("nop");
  }
  *GPPUDCLK0 = 0;  // flush GPIO setup

  *UART0_ICR = 0x7FF;  // clear interrupts
  *UART0_IBRD = 2;     // 115200 baud
  *UART0_FBRD = 0xB;
  *UART0_LCRH = 0b11 << 5;  // 8n1
  *UART0_CR = 0x301;        // enable Tx, Rx, FIFO
}

/**
 * Send a character
 */
void uart_send(unsigned int c) {
  /* wait until we can send */
  do {
    asm volatile("nop");
  } while (*UART0_FR & 0x20);
  /* write the character to the buffer */
  *UART0_DR = c;
}

/**
 * Display a string
 */
void uart_puts(const char* s) {
  while (*s) {
    /* convert newline to carrige return + newline */
    if (*s == '\n') uart_send('\r');
    uart_send(*s++);
  }
}

/**
 * Display a binary value in hexadecimal
 */
void uart_hex(unsigned long long d) {
  unsigned int n;
  int c;
  for (c = 60; c >= 0; c -= 4) {
    // get highest tetrad
    n = (d >> c) & 0xF;
    // 0-9 => '0'-'9', 10-15 => 'A'-'F'
    n += n > 9 ? 0x37 : 0x30;
    uart_send(n);
  }
}
}

namespace kernel {
namespace log {

void InitPrint() { uart_init(); }

void Print(const char* s) { uart_puts(s); }

void Print(const char* s, const uint64_t d) {
  uart_puts(s);
  uart_hex(d);
  uart_puts("\n");
}

void PrintHex(const uint64_t d) {
  uart_puts("0x");
  uart_hex(d);
}

}  // namespace log
}  // namespace kernel
