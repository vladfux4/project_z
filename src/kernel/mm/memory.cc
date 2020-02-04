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

Memory::Memory() : mmu_(), p_space_() {}

Memory::~Memory() {}

void Memory::Init() {
  using namespace arch::arm64::mm;
  using TranslationTable = AddressSpace<BootAllocator>::TranslationTable;

  size_t base = 0;
  // 880Mb of ram
  for (; base < 440; base++) {
    void* address = reinterpret_cast<void*>(base << 21);
    p_space_.translation_table.Map(
        address, address,
        {TranslationTable::BlockSize::_2MB, MemoryAttr::NORMAL, S2AP::NORMAL,
         SH::INNER_SHAREABLE, AF::IGNORE, Contiguous::OFF, XN::EXECUTE});
  }

  // VC ram up to 0x3F000000
  for (; base < 512 - 8; base++) {
    void* address = reinterpret_cast<void*>(base << 21);
    p_space_.translation_table.Map(
        address, address,
        {TranslationTable::BlockSize::_2MB, MemoryAttr::NORMAL_NC, S2AP::NORMAL,
         SH::NON_SHAREABLE, AF::IGNORE, Contiguous::OFF, XN::EXECUTE});
  }

  // 16 MB peripherals at 0x3F000000 - 0x40000000
  for (; base < 512; base++) {
    void* address = reinterpret_cast<void*>(base << 21);
    p_space_.translation_table.Map(
        address, address,
        {TranslationTable::BlockSize::_2MB, MemoryAttr::DEVICE_NGNRNE,
         S2AP::NORMAL, SH::NON_SHAREABLE, AF::IGNORE, Contiguous::OFF,
         XN::EXECUTE});
  }

  // 2 MB for mailboxes at 0x40000000
  void* address = reinterpret_cast<void*>(base << 21);
  p_space_.translation_table.Map(
      address, address,
      {TranslationTable::BlockSize::_2MB, MemoryAttr::DEVICE_NGNRNE,
       S2AP::NORMAL, SH::NON_SHAREABLE, AF::IGNORE, Contiguous::OFF,
       XN::EXECUTE});

  mmu_.SetLowerTable(p_space_.translation_table.GetBase());

  mmu_.Enable();

  auto init_begin = BootStack::GetHead();
  LOG(DEBUG) << "pool init begin: " << init_begin;

  size_t length = (880 * (1ULL << 20));  // 880Mb of ram
  // minus kernel data and boot stack size
  length = length - reinterpret_cast<size_t>(init_begin);
  LOG(DEBUG) << "pool size: " << length;

  PhysicalPagePool::Construct(length);

  // allign the stack end to page size
  auto begin = BootStack::Push(1, PageSizeInfo<KERNEL_PAGE_SIZE>::in_bytes);

  LOG(DEBUG) << "pool new begin: " << begin;
  PhysicalPagePool::Get()->SetBeginAddress(begin);

  // minus new boot stack after pool allocation
  length = length - (reinterpret_cast<size_t>(begin) -
                     reinterpret_cast<size_t>(init_begin));
  LOG(DEBUG) << "pool new size: " << length;
  PhysicalPagePool::Get()->CutBytes(length);
}

void Memory::Select(Memory::VirtualAddressSpace& space) {
  mmu_.SetHigherTable(space.translation_table.GetBase());
}

mm::UniquePointer<Memory::VirtualAddressSpace, PhysicalAllocator>
Memory::CreateVirtualAddressSpace() {
  return mm::UniquePointer<mm::Memory::VirtualAddressSpace,
                           mm::PhysicalAllocator>::Make();
}

}  // namespace mm
}  // namespace kernel
