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
#include "kernel/mm/region.h"

namespace kernel {
namespace mm {

Memory::Memory() : mmu_(), p_space_(nullptr) {
  InitPagePool();
  InitPhSpace();

  Select(*p_space_);
  mmu_.Enable();
}

void Memory::Select(AddressSpace& space) {
  mmu_.SelectAddressSpace(space);
}

PagedRegion::Sptr Memory::CreatePagedRegion(const size_t count) {
  return PagedRegion::Sptr::Make(count);
}

DirectRegion::Sptr Memory::CreateDirectRegion(void* begin, const size_t length)
{
  return DirectRegion::Sptr::Make(begin, length);
}

AddressSpace::Uptr Memory::CreateAddressSpace()
{
  return AddressSpace::Uptr::Make();
}

void Memory::InitPagePool() {
  auto init_begin = BootStack::GetHead();
  LOG(DEBUG) << "pool init begin: " << init_begin;

  size_t length = (880 * (1ULL << 20));  // 880Mb of ram
  // minus kernel data and boot stack size
  length = length - reinterpret_cast<size_t>(init_begin);
  LOG(DEBUG) << "pool size: " << length;

  auto page_pool = new (BootAllocator<PagePool>::Allocate()) PagePool(length);
  StaticPagePool::Make(*page_pool);

  // allign the stack end to page size
  auto begin = BootStack::Push(1, PageSizeInfo<KERNEL_PAGE_SIZE>::in_bytes);

  LOG(DEBUG) << "pool new begin: " << begin;
  StaticPagePool::Value().SetBeginAddress(begin);

  // minus new boot stack after pool allocation
  length = length - (reinterpret_cast<size_t>(begin) -
                     reinterpret_cast<size_t>(init_begin));
  LOG(DEBUG) << "pool new size: " << length;
  StaticPagePool::Value().CutBytes(length);
}

void Memory::InitPhSpace()
{
  p_space_ = AddressSpace::Uptr::Make();
  void* base = nullptr;
  using namespace arch::arm64::mm;

  // 880Mb of ram
  auto ram = CreateDirectRegion(base, (880ULL << 20));
  p_space_->MapRegion(base, ram, {MemoryAttr::NORMAL, S2AP::NORMAL,
       SH::INNER_SHAREABLE, AF::IGNORE, Contiguous::OFF, XN::EXECUTE});

  // VC ram up to 0x3F000000
  base = reinterpret_cast<void*>((880ULL << 20));
  auto vc_ram = CreateDirectRegion(base, (128ULL << 20));
  p_space_->MapRegion(base, vc_ram, {MemoryAttr::NORMAL_NC, S2AP::NORMAL,
         SH::NON_SHAREABLE, AF::IGNORE, Contiguous::OFF, XN::EXECUTE});

  // 16 MB peripherals at 0x3F000000 - 0x40000000
  base = reinterpret_cast<void*>((1008ULL << 20));
  auto peripherals = CreateDirectRegion(base, (16ULL << 20));
  p_space_->MapRegion(base, peripherals, {MemoryAttr::DEVICE_NGNRNE,
         S2AP::NORMAL, SH::NON_SHAREABLE, AF::IGNORE, Contiguous::OFF,
         XN::EXECUTE});

  // 2 MB for mailboxes at 0x40000000
  base = reinterpret_cast<void*>((1024ULL << 20));
  auto mailboxes = CreateDirectRegion(base, (2ULL << 20));
  p_space_->MapRegion(base, mailboxes, {MemoryAttr::DEVICE_NGNRNE,
       S2AP::NORMAL, SH::NON_SHAREABLE, AF::IGNORE, Contiguous::OFF,
       XN::EXECUTE});
}

}  // namespace mm
}  // namespace kernel
