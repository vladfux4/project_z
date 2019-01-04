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
    : mmu_(KERNEL_ADDRESS_LENGTH),
      boot_allocator_(&__kernel_boot_heap),
      pages_(nullptr),
      p_table_(boot_allocator_, KERNEL_ADDRESS_LENGTH),
      v_table_(boot_allocator_, KERNEL_ADDRESS_LENGTH) {
}

Memory::~Memory() {
}

void Memory::Init() {
  using namespace arch::arm64::mm;

  size_t base = 0;
  // 880Mb of ram
  for (; base < 440; base++) { //4440
    void* address = reinterpret_cast<void*>(base << 21);
    p_table_.Map(
        address, address,
        TranslationTable::BlockSize::_2MB,
        types::MEMORYATTR_NORMAL,
        types::S2AP_NORMAL, types::SH_INNER_SHAREABLE,
        types::AF_ON, types::CONTIGUOUS_OFF, types::XN_OFF);
  }

  // VC ram up to 0x3F000000
  for (; base < 512 - 8; base++) {
    void* address = reinterpret_cast<void*>(base << 21);
    p_table_.Map(
        address, address,
        TranslationTable::BlockSize::_2MB,
        types::MEMORYATTR_NORMAL_NC,
        types::S2AP_NORMAL, types::SH_NON_SHAREABLE,
        types::AF_ON, types::CONTIGUOUS_OFF, types::XN_OFF);
  }

  // 16 MB peripherals at 0x3F000000 - 0x40000000
  for (; base < 512; base++) {
    void* address = reinterpret_cast<void*>(base << 21);
    p_table_.Map(
        address, address,
        TranslationTable::BlockSize::_2MB,
        types::MEMORYATTR_DEVICE_NGNRNE,
        types::S2AP_NORMAL, types::SH_NON_SHAREABLE,
        types::AF_ON, types::CONTIGUOUS_OFF, types::XN_OFF);
  }

  // 2 MB for mailboxes at 0x40000000
  void* address = reinterpret_cast<void*>(base << 21);
  p_table_.Map(
        address, address,
        TranslationTable::BlockSize::_2MB,
        types::MEMORYATTR_DEVICE_NGNRNE,
        types::S2AP_NORMAL, types::SH_NON_SHAREABLE,
        types::AF_ON, types::CONTIGUOUS_OFF, types::XN_OFF);

  mmu_.SetUserTable(p_table_.GetBase());

  v_table_.Map(
      reinterpret_cast<void*>(0xFFFFFFFFFFE00000), reinterpret_cast<void*>(0x0000),
      TranslationTable::BlockSize::_4KB,
      types::MEMORYATTR_NORMAL,
      types::S2AP_NORMAL, types::SH_INNER_SHAREABLE,
      types::AF_ON, types::CONTIGUOUS_OFF, types::XN_OFF);

  mmu_.SetKernelTable(v_table_.GetBase());

  mmu_.Enable();
}

}  // namespace mm
}  // namespace kernel