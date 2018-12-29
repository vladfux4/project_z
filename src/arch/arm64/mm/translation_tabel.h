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
#ifndef ARCH_ARM64_MM_TRANSLATION_TABLE_H_
#define ARCH_ARM64_MM_TRANSLATION_TABLE_H_

#include <stdint.h>
#include <assert.h>

#include "arch/arm64/mm/translation_descriptor.h"

namespace arch {
namespace arm64 {
namespace mm {

class TranslationTable4K2MBlock {
 public:

  TranslationTable4K2MBlock();
  inline uint8_t* GetBasePtr();

 private:
  static constexpr uint64_t kEntryCount = 512;
  static constexpr uint64_t kBlockTableCount = 2;

  struct Table4KLvl1 {
    typedef TableDescriptor<PageSize::_4KB> Entry;
    __attribute__((aligned(4096)))
    Entry data[kEntryCount];
  };

  struct Table2MBlockLvl2 {
    typedef EntryDescriptor<PageSize::_4KB, TableLvl::_2> Entry;
    __attribute__((aligned(4096)))
    Entry data[kEntryCount];
  };

  Table4KLvl1 table_lvl1_;
  Table2MBlockLvl2 block_table_lvl2_[kBlockTableCount];
};

static_assert(sizeof(TranslationTable4K2MBlock) == (512 * 8 * 3), "Wrong table size");

inline uint8_t* TranslationTable4K2MBlock::GetBasePtr() {
  return reinterpret_cast<uint8_t*>(table_lvl1_.data);
}

class TranslationTableVirtual4K {
 public:
  TranslationTableVirtual4K();
  inline uint8_t* GetBasePtr();

  struct Table4KDsc {
    typedef TableDescriptor<PageSize::_4KB> Entry;
    __attribute__((aligned(4096)))
    Entry data[512];
  };

  struct TableLvl3 {
    typedef EntryDescriptor<PageSize::_4KB, TableLvl::_3> Entry;
    __attribute__((aligned(4096)))
    Entry data[512];
  };

  Table4KDsc table_1_;
  Table4KDsc table_2_;
  TableLvl3 table_3_;
};

static_assert(sizeof(TranslationTableVirtual4K) == (512 * 8 * 3), "Wrong table size");

inline uint8_t* TranslationTableVirtual4K::GetBasePtr() {
  return reinterpret_cast<uint8_t*>(table_1_.data);
}

}  // namespace mm
}  // namespace arm64
}  // namespace arch

#endif  // ARCH_ARM64_MM_TRANSLATION_TABLE_H_
