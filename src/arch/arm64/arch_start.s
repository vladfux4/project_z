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

.globl _start
_start:
  ldr x0, =kernel_stack_top
  mov sp, x0
  b KernelEntry

.section .arch_kernel_data
/* stacks definition */
.align 8
kernel_stack:
.skip 0x1000; /* 4kB of stack memory */
kernel_stack_top = .;
