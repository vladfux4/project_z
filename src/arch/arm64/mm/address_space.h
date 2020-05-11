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
#ifndef ARCH_ARM64_MM_ADDRESS_SPACE_H_
#define ARCH_ARM64_MM_ADDRESS_SPACE_H_

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

#include "arch/arm64/mm/translation_table.h"
#include "kernel/logger.h"
#include "kernel/config.h"
#include "kernel/mm/physical_allocator.h"
#include "kernel/mm/region.h"

namespace arch {
namespace arm64 {
namespace mm {

class AddressSpace {
 public:
  using TranslationTable =
      arch::arm64::mm::TranslationTable<kernel::mm::KERNEL_PAGE_SIZE, kernel::mm::KERNEL_ADDRESS_LENGTH,
                                        kernel::mm::SlabAllocator>;

  void MapRegion(void* begin, kernel::mm::PagedRegion::Sptr& region, const kernel::mm::Region::Attributes& attr) {
    auto& pages = region->Pages();
    auto address = reinterpret_cast<kernel::mm::PagedRegion::Page*>(begin);
    for (auto it = pages.Begin(); it != pages.End(); it++, address++) {
      LOG(DEBUG) << "map page v: " << address << " -> p: " << it.Value();

      TranslationTable::EntryParameters params = {
        AddressSpace::TranslationTable::BlockSize::_4KB,
        attr.mem_attr,
        attr.s2ap,
        attr.sh,
        attr.af,
        attr.contiguous,
        attr.xn
      };
      translation_table.Map(address, reinterpret_cast<void*>(it.Value()), params);
    }
  }

  void MapRegion(void* begin, kernel::mm::DirectRegion::Sptr& region, const kernel::mm::Region::Attributes& attr) {
    bool valid_region = false;
    TranslationTable::BlockSize block_size = TranslationTable::BlockSize::_4KB;
    size_t block_length = 0;
    size_t count = 0;

    if ((reinterpret_cast<size_t>(region->Begin()) % (1ULL << 12)) != 0) {
      LOG(ERROR) << "Not alligned region begin address: " << region->Begin();
      return;
    }

    if ((region->Length() % (1ULL << 12)) == 0) {
      block_size = TranslationTable::BlockSize::_4KB;
      block_length = (1ULL << 12);
      count = region->Length() / (1ULL << 12);
      valid_region = true;
    }

    if ((region->Length() % (1ULL << 21)) == 0) {
      block_size = TranslationTable::BlockSize::_2MB;
      block_length = (1ULL << 21);
      count = region->Length() / (1ULL << 21);
      valid_region = true;
    }

    if ((region->Length() % (1ULL << 30)) == 0) {
      block_size = TranslationTable::BlockSize::_1GB;
      block_length = (1ULL << 30);
      count = region->Length() / (1ULL << 30);
      valid_region = true;
    }

    if (!valid_region) {
      LOG(ERROR) << "Wrong region length: " << region->Length();
      return;
    }

    for (size_t i = 0; i < count; i++) {
      const size_t offset = (i * block_length);
      void* p_address =
          reinterpret_cast<void*>(
            offset + reinterpret_cast<size_t>(region->Begin()));
      void* v_address =
          reinterpret_cast<void*>(
            offset + reinterpret_cast<size_t>(begin));

      TranslationTable::EntryParameters params = {
        block_size,
        attr.mem_attr,
        attr.s2ap,
        attr.sh,
        attr.af,
        attr.contiguous,
        attr.xn
      };

      translation_table.Map(v_address, p_address, params);
    }

    LOG(DEBUG) << "map region v: " << begin << " -> p: " << region->Begin();
  }

  TranslationTable translation_table;
};

}  // namespace mm
}  // namespace arm64
}  // namespace arch

#endif  // ARCH_ARM64_MM_ADDRESS_SPACE_H_
