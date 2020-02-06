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

.macro exception_entry
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
.endm

.macro _switch_ttb
  msr ttbr1_el1, x9
  tlbi vmalle1is
  dsb ish
  isb
.endm

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
  bl  kernel_get_current_context
  cbz x0, _load_context
_save_context:
  mov x28, x0
  add x28, x28, #8 // skip tt

  ldp  x9, x10, [sp],#16 //elr, spsr
  stp  x9, x10, [x28],#16

  mrs x9, sp_el0 //sp
  str  x9, [x28],#8

  ldr  x9, [sp],#8 //x0
  str  x9, [x28],#8
  ldp  x9, x10, [sp],#16 //x1, x2
  stp  x9, x10, [x28],#16
  ldp  x9, x10, [sp],#16 //x3, x4
  stp  x9, x10, [x28],#16
  ldp  x9, x10, [sp],#16 //x5, x6
  stp  x9, x10, [x28],#16
  ldp  x9, x10, [sp],#16 //x7, x8
  stp  x9, x10, [x28],#16
  ldp  x9, x10, [sp],#16 //x9, x10
  stp  x9, x10, [x28],#16
  ldp  x9, x10, [sp],#16 //x11, x12
  stp  x9, x10, [x28],#16
  ldp  x9, x10, [sp],#16 //x13, x14
  stp  x9, x10, [x28],#16
  ldp  x9, x10, [sp],#16 //x15, x16
  stp  x9, x10, [x28],#16
  ldp  x9, x10, [sp],#16 //x17, x18
  stp  x9, x10, [x28],#16
  ldp  x9, x10, [sp],#16 //x19, x20
  stp  x9, x10, [x28],#16
  ldp  x9, x10, [sp],#16 //x21, x22
  stp  x9, x10, [x28],#16
  ldp  x9, x10, [sp],#16 //x23, x24
  stp  x9, x10, [x28],#16
  ldp  x9, x10, [sp],#16 //x25, x26
  stp  x9, x10, [x28],#16
  ldp  x9, x10, [sp],#16 //x27, x28
  stp  x9, x10, [x28],#16
  ldp  x9, x10, [sp],#16 //x29, x30
  stp  x9, x10, [x28],#16

_load_context:
  bl  kernel_get_context_to_switch
  mov x30, x0

  ldr  x9, [x30],#8
  _switch_ttb

  ldp  x9, x10, [x30],#16 //elr, spsr*/
  msr  elr_el1, x9
  msr  spsr_el1, x10

  ldr  x9, [x30],#8 //sp
  msr sp_el0, x9

  ldr  x0, [x30],#8
  ldp  x1, x2, [x30],#16
  ldp  x3, x4, [x30],#16
  ldp  x5, x6, [x30],#16
  ldp  x7, x8, [x30],#16
  ldp  x9, x10, [x30],#16
  ldp  x11, x12, [x30],#16
  ldp  x13, x14, [x30],#16
  ldp  x15, x16, [x30],#16
  ldp  x17, x18, [x30],#16
  ldp  x19, x20, [x30],#16
  ldp  x21, x22, [x30],#16
  ldp  x23, x24, [x30],#16
  ldp  x25, x26, [x30],#16
  ldp  x27, x28, [x30],#16
  ldp  x29, x30, [x30]
  eret
	
exception_exit:
  cbnz x0, _switch_context
  b  _exception_exit

.align  11
.globl  exception_vectors
exception_vectors:
.balign 128
  b hang
.balign 128
  exception_entry
  bl c_irq_handler
  b exception_exit
.balign 128
  b hang
.balign 128
  b hang
.balign 128
  exception_entry
  bl c_sync_handler
  b exception_exit
.balign 128
  exception_entry
  bl c_irq_handler
  b exception_exit
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
