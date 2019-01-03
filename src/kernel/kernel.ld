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

ENTRY(_start)
SECTIONS
{
 . = 0x80000;
 .start_text . : { *arch_start.s.o(.text) }
 .text : { *(.text) }
 . = ALIGN(4096);
 .data : { *(.data) }
 __bss_start = .;
 . = ALIGN(4096);
 .bss : {
   . = ALIGN(16);
   *(.bss COMMON)
 }
 __bss_end = .;
 .arch_kernel_data : { *(.arch_kernel_data) }
 __kernel_boot_heap = .;
}

__bss_size = (__bss_end - __bss_start)>>3;
