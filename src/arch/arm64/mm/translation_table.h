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
#include <cstddef>
#include <assert.h>

#include "arch/arm64/mm/translation_descriptor.h"
#include "kernel/mm/allocator.h"
#include "kernel/logger.h"

namespace arch {
namespace arm64 {
namespace mm {

/**
 * @brief Get Table entry count by page size
 *
 * @param size Page size
 *
 * @return entry count
 */
constexpr size_t TableEntryCount(const kernel::mm::PageSize size) {
  switch (size) {
    case kernel::mm::PageSize::_4KB: { return (1ULL << 9); }
    case kernel::mm::PageSize::_16KB: { return (1ULL << 11); }
    case kernel::mm::PageSize::_64KB: { return (1ULL << 13); }
  }

  return 0;
}

/**
 * @brief Translation descriptor table struct
 */
template<kernel::mm::PageSize kPageSize>
struct DescriptorTable {
  typedef TableDescriptor<kPageSize> Entry;
  Entry data[TableEntryCount(kPageSize)];
};
static_assert(sizeof(DescriptorTable<kernel::mm::PageSize::_4KB>) == (512 * 8), "Wrong table size");

/**
 * @brief Translation entry table struct
 */
template<kernel::mm::PageSize kPageSize, TableLvl kLvl>
struct EntryTable {
  typedef EntryDescriptor<kPageSize, kLvl> Entry;
  Entry data[TableEntryCount(kPageSize)];
};
static_assert(sizeof(EntryTable<kernel::mm::PageSize::_4KB, TableLvl::_1>) == (512 * 8), "Wrong table size");

template<kernel::mm::PageSize kSize>
class TranslationTableConfig {
};

template<>
class TranslationTableConfig<kernel::mm::PageSize::_4KB> {
 public:
  /**
   * @brief The BlockSize enum
   */
  enum class BlockSize {
    _4KB,
    _2MB,
    _1GB,
    _512GB,
  };

  typedef DescriptorTable<kernel::mm::PageSize::_4KB> Table;
  static const BlockSize kMinBlockSize = BlockSize::_4KB;
  static const kernel::mm::PageSize kPageSize = kernel::mm::PageSize::_4KB;

  /**
   * @brief Calculate table index by pointer
   *
   * @param ptr Pointer
   * @param level Table level
   *
   * @return index
   */
  static inline size_t CalcIndex(const void* ptr, const uint8_t level) {
    return ((reinterpret_cast<size_t>(ptr) >> (12 + (9 * (level - 1)))) & (512 - 1));
  }

  /**
   * @brief Calculate entry address for table
   *
   * @param ptr Pointer
   * @param level Table level
   *
   * @return pointer
   */
  static inline uint64_t CalcEntryAddress(const void* ptr, const uint8_t level) {
    return reinterpret_cast<uint64_t>(ptr) >> (12 + (9 * (level - 1)));
  }

  /**
   * @brief Calculate BlockSize from level
   *
   * @param level Table level
   *
   * @return block size
   */
  static inline BlockSize CalcBlockSizeFromTableLevel(const uint8_t level) {
    switch (level) {
      case 4: {
        return BlockSize::_512GB;
      }
      case 3: {
        return BlockSize::_1GB;
      }
      case 2: {
        return BlockSize::_2MB;
      }
      case 1: {
        return BlockSize::_4KB;
      }
      default: {
        //assert();
      }
    }

    return BlockSize::_4KB;
  }

  /**
   * @brief Calculate table level by address length
   *
   * @param address_length Length
   *
   * @return table max level
   */
  static inline uint8_t CalcTableLevel(const uint8_t address_length) {
    uint8_t level = 0;

    if (address_length > 12) {
      level = 1;
    }

    if (address_length > 21) {
      level = 2;
    }

    if (address_length > 30) {
      level = 3;
    }

    if (address_length > 39) {
      level = 4;
    }

    if (0 == level) {
      //assert();
    }

    return level;
  }
};

template <class Config>
class TranslationTableBase {
 public:
  typedef typename Config::BlockSize BlockSize;
  typedef typename Config::Table Table;

  /**
   * @brief Constructor
   *
   * @param alloc Reference on allocator
   * @param address_length Address length
   */
  TranslationTableBase(kernel::mm::Allocator& alloc,
                       const uint8_t address_length);

  /**
   * @brief Get table base pointer
   *
   * @return pointer
   */
  inline void* GetBase();

  /**
   * @brief Map virtual address on physical
   *
   * @param v_ptr Virtual address
   * @param p_ptr Physical address
   * @param size Block size
   *
   * @param mem_attr
   * @param s2ap
   * @param sh
   * @param af
   * @param contiguous
   * @param xn
   */
  void Map(const void* v_ptr, const void* p_ptr,
           const BlockSize size,
           const types::MemoryAttr mem_attr,
           const types::S2AP s2ap, const types::SH sh,
           const types::AF af, const types::Contiguous contiguous,
           const types::XN xn);

  /**
   * @brief CreateTable
   *
   * @return Pointer on new table
   */
  Table* CreateTable();

 private:
  kernel::mm::Allocator& alloc_;
  uint8_t address_length_;
  Table* root_table_;
};

template<kernel::mm::PageSize kSize>
class TranslationTable : public TranslationTableBase<TranslationTableConfig<kSize>> {
 public:
  TranslationTable(kernel::mm::Allocator& alloc,
                   const uint8_t address_length)
      : TranslationTableBase<TranslationTableConfig<kSize>>(alloc, address_length) {}
};

template<class Config>
TranslationTableBase<Config>::TranslationTableBase(
    kernel::mm::Allocator& alloc,
    const uint8_t address_length)
    : alloc_(alloc),
      address_length_(address_length),
      root_table_(nullptr) {
  DDBG_LOG("Constructor");

  root_table_ = CreateTable();
}

template<class Config>
inline void* TranslationTableBase<Config>::GetBase() {
  return reinterpret_cast<void*>(root_table_);
}

template<class Config>
void TranslationTableBase<Config>::Map(
         const void* v_ptr, const void* p_ptr,
         const TranslationTableBase<Config>::BlockSize size,
         const types::MemoryAttr mem_attr,
         const types::S2AP s2ap, const types::SH sh,
         const types::AF af, const types::Contiguous contiguous,
         const types::XN xn) {
  auto level = Config::CalcTableLevel(address_length_);

  Table* table = root_table_;
  while (0 != level) { //find table
    if (size == Config::CalcBlockSizeFromTableLevel(level)) {
      break;
    }

    size_t index = Config::CalcIndex(v_ptr, level);
    Table* next_level_table = reinterpret_cast<Table*>(Table::Entry::ToAddress(table->data[index].data.address));
    if (nullptr == next_level_table) {
      DDBG_LOG("current level: ", level);
      DDBG_LOG("table index: ", index);

      next_level_table = CreateTable();
      table->data[index] = typename Table::Entry(
        types::Entry::ENTRY_TABLE,
        Table::Entry::ToTableAddress(next_level_table),
        types::PXN_OFF, types::XN_OFF, types::AP_NOEFFECT, types::NS_ON);
    }

    level--;
    table = next_level_table;
  }

  // create entry
  const auto index = Config::CalcIndex(v_ptr, level);
  auto& entry = table->data[index];
  uint64_t address = Config::CalcEntryAddress(p_ptr, level);
  auto entry_type = (Config::kMinBlockSize == size) ?
                     types::ENTRY_TABLE : types::ENTRY_BLOCK;

  DDBG_LOG("New etry. table level: ", level);
  DDBG_LOG("entry index: ", index);
  DDBG_LOG("address: ", address);

  if (3 == level) {
    typedef EntryDescriptor<Config::kPageSize, TableLvl::_1> Entry;
    *reinterpret_cast<Entry*>(&entry) =
        Entry(entry_type, address, mem_attr, s2ap, sh, af, contiguous, xn);
  } else if (2 == level) {
    typedef EntryDescriptor<Config::kPageSize, TableLvl::_2> Entry;
    *reinterpret_cast<Entry*>(&entry) =
        Entry(entry_type, address, mem_attr, s2ap, sh, af, contiguous, xn);
  } else if (1 == level) {
    typedef EntryDescriptor<Config::kPageSize, TableLvl::_3> Entry;
    *reinterpret_cast<Entry*>(&entry) =
        Entry(entry_type, address, mem_attr, s2ap, sh, af, contiguous, xn);
  }
}

template<class Config>
typename TranslationTableBase<Config>::Table*
TranslationTableBase<Config>::CreateTable() {
  auto table = reinterpret_cast<Table*>(alloc_.Allocate(sizeof(Table), sizeof(Table)));
  DDBG_LOG("New table ptr: ", reinterpret_cast<uint64_t>(table));

  for (size_t i = 0; i < TableEntryCount(kernel::mm::PageSize::_4KB); i++) {
    table->data[i] = typename Table::Entry();
  }

  return table;
}

}  // namespace mm
}  // namespace arm64
}  // namespace arch

#endif  // ARCH_ARM64_MM_TRANSLATION_TABLE_H_
