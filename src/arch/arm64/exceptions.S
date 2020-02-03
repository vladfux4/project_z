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

hang:
  wfi
  b     hang

.globl enable_irq
enable_irq:
  msr   daifclr, #2
  ret

.globl disable_irq
disable_irq:
  msr   daifset, #2
  ret

_exception_entry:
  stp  x29, x30, [sp, #-16]!
  stp  x27, x28, [sp, #-16]!
  stp  x25, x26, [sp, #-16]!
  stp  x23, x24, [sp, #-16]!
  stp  x21, x22, [sp, #-16]!
  stp  x19, x20, [sp, #-16]!
  stp  x17, x18, [sp, #-16]!
  stp  x15, x16, [sp, #-16]!
  stp  x13, x14, [sp, #-16]!
  stp  x11, x12, [sp, #-16]!
  stp  x9, x10, [sp, #-16]!
  stp  x7, x8, [sp, #-16]!
  stp  x5, x6, [sp, #-16]!
  stp  x3, x4, [sp, #-16]!
  stp  x1, x2, [sp, #-16]!
  str x0, [sp, #-8]!
  mrs  x1, elr_el1
  mrs  x2, spsr_el1
  stp  x1, x2, [sp, #-16]!
  ret

_exception_exit:
  ldp  x1, x2, [sp],#16
  msr  elr_el1, x1
  msr  spsr_el1, x2
  ldr  x0, [sp],#8
  ldp  x1, x2, [sp],#16
  ldp  x3, x4, [sp],#16
  ldp  x5, x6, [sp],#16
  ldp  x7, x8, [sp],#16
  ldp  x9, x10, [sp],#16
  ldp  x11, x12, [sp],#16
  ldp  x13, x14, [sp],#16
  ldp  x15, x16, [sp],#16
  ldp  x17, x18, [sp],#16
  ldp  x19, x20, [sp],#16
  ldp  x21, x22, [sp],#16
  ldp  x23, x24, [sp],#16
  ldp  x25, x26, [sp],#16
  ldp  x27, x28, [sp],#16
  ldp  x29, x30, [sp],#16
  eret

_switch_context:
  cbnz x0, 1f
  b  _exception_exit
1:
  b hang
  cbz x1, 2f
/*  ldp  x9, x10, [sp],#16 //elr, spsr
  stp  x9, x10, [x1],#16*/

/*  msr  elr_el1, x1
  msr  spsr_el1, x2*/
2:
  b hang

/*  ldr  x0, [sp],#8
  ldp  x1, x2, [sp],#16
  ldp  x3, x4, [sp],#16
  ldp  x5, x6, [sp],#16
  ldp  x7, x8, [sp],#16
  ldp  x9, x10, [sp],#16
  ldp  x11, x12, [sp],#16
  ldp  x13, x14, [sp],#16
  ldp  x15, x16, [sp],#16
  ldp  x17, x18, [sp],#16
  ldp  x19, x20, [sp],#16
  ldp  x21, x22, [sp],#16
  ldp  x23, x24, [sp],#16
  ldp  x25, x26, [sp],#16
  ldp  x27, x28, [sp],#16
  ldp  x29, x30, [sp],#16

  b  _exception_exit*/

.align  11
.globl  vectors
vectors:
.balign 128
  b hang
.balign 128
  bl _exception_entry
  bl  c_irq_handler
  b  _exception_exit
/*  b  _switch_context*/
.balign 128
  b hang
.balign 128
  b hang
.balign 128
  bl _exception_entry
  bl  c_irq_handler
  b  _exception_exit
/*  b  _switch_context*/
.balign 128
  bl _exception_entry
  bl  c_irq_handler
  b  _exception_exit
.balign 128
  b hang
.balign 128
  b hang
.balign 128
  b hang
.balign 128
  b hang
.balign 128
  b hang
.balign 128
  b hang
.balign 128
  b hang
.balign 128
  b hang
.balign 128
  b hang
.balign 128
  b hang
