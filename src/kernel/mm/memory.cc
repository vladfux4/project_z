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
#include "kernel/mm/memory.h"

extern uint8_t __kernel_boot_heap;

namespace kernel {
namespace mm {

Memory::Memory()
    : mmu_(),
      boot_allocator_(&__kernel_boot_heap),
      pages_(nullptr),
      v_table_(boot_allocator_, 39) {
}

Memory::~Memory() {
}

void Memory::Init() {
  v_table_.Map(
      reinterpret_cast<void*>(0xFFFFFFFFFFE00000), reinterpret_cast<void*>(0x0000),
      arch::arm64::mm::TranslationTable<PageSize::_4KB>::BlockSize::_4KB,
      arch::arm64::mm::types::MEMORYATTR_NORMAL,
      arch::arm64::mm::types::S2AP_NORMAL, arch::arm64::mm::types::SH_INNER_SHAREABLE,
      arch::arm64::mm::types::AF_ON, arch::arm64::mm::types::CONTIGUOUS_OFF, arch::arm64::mm::types::XN_OFF);

  mmu_.SetKernelTable(v_table_.GetBase());

  mmu_.Enable();
}

}  // namespace mm
}  // namespace kernel
