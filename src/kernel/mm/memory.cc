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

namespace kernel {
namespace mm {

PhysicalPagePool* PhysicalPagePool::ref_ = nullptr;

Memory::Memory() : mmu_(), p_space_() {}

Memory::~Memory() {}

void Memory::Init() {
  using namespace arch::arm64::mm;

  size_t base = 0;
  // 880Mb of ram
  for (; base < 440; base++) {  // 4440
    void* address = reinterpret_cast<void*>(base << 21);
    p_space_.translation_table.Map(
        address, address, TranslationTable::BlockSize::_2MB,
        {MemoryAttr::NORMAL, S2AP::NORMAL, SH::INNER_SHAREABLE, AF::IGNORE,
         Contiguous::OFF, XN::EXECUTE});
  }

  // VC ram up to 0x3F000000
  for (; base < 512 - 8; base++) {
    void* address = reinterpret_cast<void*>(base << 21);
    p_space_.translation_table.Map(
        address, address, TranslationTable::BlockSize::_2MB,
        {MemoryAttr::NORMAL_NC, S2AP::NORMAL, SH::NON_SHAREABLE, AF::IGNORE,
         Contiguous::OFF, XN::EXECUTE});
  }

  // 16 MB peripherals at 0x3F000000 - 0x40000000
  for (; base < 512; base++) {
    void* address = reinterpret_cast<void*>(base << 21);
    p_space_.translation_table.Map(
        address, address, TranslationTable::BlockSize::_2MB,
        {MemoryAttr::DEVICE_NGNRNE, S2AP::NORMAL, SH::NON_SHAREABLE, AF::IGNORE,
         Contiguous::OFF, XN::EXECUTE});
  }

  // 2 MB for mailboxes at 0x40000000
  void* address = reinterpret_cast<void*>(base << 21);
  p_space_.translation_table.Map(
      address, address, TranslationTable::BlockSize::_2MB,
      {MemoryAttr::DEVICE_NGNRNE, S2AP::NORMAL, SH::NON_SHAREABLE, AF::IGNORE,
       Contiguous::OFF, XN::EXECUTE});

  mmu_.SetLowerTable(p_space_.translation_table.GetBase());

  mmu_.Enable();

  PhysicalPagePool::Construct();

  auto v_space = PhysicalPagePoolAllocator<
      AddressSpace<PhysicalPagePoolAllocator>>::Allocate();
  new (v_space) AddressSpace<PhysicalPagePoolAllocator>();

  v_space->translation_table.Map(
      reinterpret_cast<void*>(0xFFFFFFFFFFE00000),
      reinterpret_cast<void*>(0x0000), TranslationTable::BlockSize::_4KB,
      {MemoryAttr::NORMAL, S2AP::NORMAL, SH::INNER_SHAREABLE, AF::IGNORE,
       Contiguous::OFF, XN::EXECUTE});

  mmu_.SetHigherTable(v_space->translation_table.GetBase());
}

}  // namespace mm
}  // namespace kernel
