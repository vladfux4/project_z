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
#include "kernel/kernel.h"

#include <stdint.h>

#include <cstddef>

#include "kernel/logger.h"
#include "kernel/mm/unique_ptr.h"

/// dummy operators for c++ support
extern "C" {
void __cxa_pure_virtual(void) {
  // We might want to write some diagnostics to uart in this case
  while (true) {
  }
}

void* memset(void* dst, int val, size_t size) {
  for (size_t i = 0; i < size; i++) {
    ((uint8_t*)dst)[i] = (uint8_t)val;
  }

  return dst;
}

__attribute__((__noreturn__)) void __assert_func(const char*, int, const char*,
                                                 const char*) {
  while (true) {
  }
}
}

void Function_1() {
  static uint64_t a = 0;
  
    while (true) {
  a++;

  uint64_t* v_ptr =
      reinterpret_cast<uint64_t*>(0xFFFFFFFFFFE00050);  // mapped on 0x50
  *v_ptr = a;
}
}

void Function_2() {
  static uint64_t a = 0;
  a++;

  uint64_t* v_ptr =
      reinterpret_cast<uint64_t*>(0xFFFFFFFFFFF00060);  // mapped on 0x50
  *v_ptr = a;
}

namespace kernel {

static uint8_t __attribute__((aligned(4096))) kernel_storage[sizeof(Kernel)];

Kernel::Kernel() : memory_(), scheduler_(memory_) {}

void Kernel::Routine() {
  Init();

  LOG(INFO) << "Hello";
  LOG(INFO) << "Free pages: "
            << static_cast<size_t>(
                   kernel::mm::PhysicalPagePool::Get()->FreeItems());
  {
    auto process_1 = scheduler_.CreateProcess();
    process_1->AddressSpace().MapNewPage(
        reinterpret_cast<void*>(0xFFFFFFFFFFE00000));
    process_1->SetExec(Function_1);

    auto process_2 = scheduler_.CreateProcess();
    process_2->AddressSpace().MapNewPage(
        reinterpret_cast<void*>(0xFFFFFFFFFFF00000));
    process_2->SetExec(Function_2);

    // address translation test code
//    while (true) {
//      static uint64_t a = 0;
//      a++;
//      uint64_t* ptr = reinterpret_cast<uint64_t*>(0x40);
//      *ptr = a;

      scheduler_.Select(*process_1);
//      process_1->Exec();
      scheduler_.Select(*process_2);
//      process_2->Exec();

      //      if (a == 0xFF) {
      //        break;
      //      }
//    }
  }

  LOG(INFO) << "Free pages: "
            << static_cast<size_t>(
                   kernel::mm::PhysicalPagePool::Get()->FreeItems());
}

Kernel::~Kernel() {}

extern "C" {

extern uint8_t vectors;

static uint32_t cntfrq = 0;

uint32_t read_cntfrq(void) {
  uint32_t val;
  asm volatile("mrs %0, cntfrq_el0" : "=r"(val));
  return val;
}

void write_cntv_tval(uint32_t val) {
  asm volatile("msr cntv_tval_el0, %0" ::"r"(val));
}

uint32_t read_cntv_tval(void) {
  uint32_t val;
  asm volatile("mrs %0, cntv_tval_el0" : "=r"(val));
  return val;
}

// Memory-Mapped I/O output
static inline void mmio_write(intptr_t reg, uint32_t data) {
  *(volatile uint32_t*)reg = data;
}

// Memory-Mapped I/O input
static inline uint32_t mmio_read(intptr_t reg) {
  return *(volatile uint32_t*)reg;
}

#define CORE0_TIMER_IRQCNTL 0x40000040
#define CORE0_IRQ_SOURCE 0x40000060

void routing_core0cntv_to_core0irq(void) {
  mmio_write(CORE0_TIMER_IRQCNTL, 0x08);
}

void enable_cntv(void) {
  uint32_t cntv_ctl;
  cntv_ctl = 1;
  asm volatile("msr cntv_ctl_el0, %0" ::"r"(cntv_ctl));
}

extern void enable_irq(void);

extern void disable_irq(void);

uint32_t read_core0timer_pending(void) {
  uint32_t tmp;
  tmp = mmio_read(CORE0_IRQ_SOURCE);
  return tmp;
}

uint64_t read_cntvct(void) {
  uint64_t val;
  asm volatile("mrs %0, cntvct_el0" : "=r"(val));
  return (val);
}

void timer_tick() {
  uint32_t cntvct;
  uint32_t val;

  disable_irq();
  if (read_core0timer_pending() & 0x08) {
    val = read_cntv_tval();
    LOG(INFO) << "handler CNTV_TVAL: " << val;

    write_cntv_tval(cntfrq);  // clear cntv interrupt and set next 1sec timer.

    cntvct = read_cntvct();
    LOG(INFO) << "handler CNTVCT: " << cntvct;
  }

  enable_irq();
}

void c_irq_handler(void) {
  timer_tick();
}

void c_process_irq_handler(void) {
  timer_tick();
  LOG(INFO) << "Exception from process context";
  
  LOG(INFO) << "Start delay";
  for (size_t i = 0; i < 0x1FFFFFFF; i++) {
  }
  LOG(INFO) << "End delay";
}

void Kernel::Init() { memory_.Init(); }

void KernelEntry() {
  asm volatile("msr	vbar_el1, %0" : : "r"(&vectors));

  log::InitPrint();

  cntfrq = read_cntfrq();
  LOG(INFO) << "CNTFRQ  : " << cntfrq;

  write_cntv_tval(cntfrq);  // clear cntv interrupt and set next 1 sec timer.
  uint32_t val = read_cntv_tval();
  LOG(INFO) << "CNTV_TVAL  : " << val;

  routing_core0cntv_to_core0irq();
  enable_cntv();
  enable_irq();

  auto kernel = new (reinterpret_cast<Kernel*>(kernel_storage)) Kernel();
  kernel->Routine();
}
}

}  // namespace kernel
