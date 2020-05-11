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
#include "kernel/mm/unique_ptr.h"

namespace arch {
namespace arm64 {
namespace mm {

class AddressSpace {
 public:
  using TranslationTable =
      arch::arm64::mm::TranslationTable<kernel::mm::KERNEL_PAGE_SIZE, kernel::mm::KERNEL_ADDRESS_LENGTH,
                                        kernel::mm::SlabAllocator>;

  static constexpr size_t kLowerStart = 0;
  static constexpr size_t kLowerEnd = (1ULL << kernel::mm::KERNEL_ADDRESS_LENGTH);
  static constexpr size_t kHigherStart = 0xFFFFFF8000000000;
  static constexpr size_t kHigherEnd = 0xFFFFFFFFFFFFF000;

  AddressSpace() : lower_table_(nullptr), higher_table_(nullptr) {}

  void MapRegion(void* begin, kernel::mm::PagedRegion::Sptr& region, const kernel::mm::Region::Attributes& attr);
  void MapRegion(void* begin, kernel::mm::DirectRegion::Sptr& region, const kernel::mm::Region::Attributes& attr);

  const TranslationTable* LowerTable() const {
    return lower_table_.Get();
  }

  const TranslationTable* HigherTable() const {
    return higher_table_.Get();
  }

 private:
  using TranslationTableUptr = kernel::mm::UniquePointer<TranslationTable, kernel::mm::SlabAllocator>;

  TranslationTable* ChooseTable(void* address, size_t length);

  TranslationTable* GetLowerTable() { return GetTable(lower_table_); }
  TranslationTable* GetHigherTable() { return GetTable(higher_table_); }

  TranslationTable* GetTable(TranslationTableUptr& table) {
    if (!table) {
      table = TranslationTableUptr::Make();
    }
    return table.Get();
  }

  TranslationTableUptr lower_table_;
  TranslationTableUptr higher_table_;
};

}  // namespace mm
}  // namespace arm64
}  // namespace arch

#endif  // ARCH_ARM64_MM_ADDRESS_SPACE_H_
