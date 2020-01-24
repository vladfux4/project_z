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

#include <assert.h>
#include <stdint.h>
#include <cstddef>
#include <type_traits>
#include <utility>

#include "arch/arm64/mm/translation_descriptor.h"
#include "kernel/logger.h"
#include "kernel/mm/allocator.h"

// Default placement versions of operator new.
inline void* operator new(size_t, void* __p) { return __p; }
inline void* operator new[](size_t, void* __p) { return __p; }

namespace arch {
namespace arm64 {
namespace mm {

enum class LookupLevel : uint8_t {
  _1 = 1,
  _2 = 2,
  _3 = 3,
  _4 = 4,
};

using LookupLevelInt = std::underlying_type<LookupLevel>::type;

template <typename T,
          typename std::underlying_type<T>::type kEnd =
              static_cast<typename std::underlying_type<T>::type>(-1)>
class EnumIterator {
 public:
  using UnderlyingType = typename std::underlying_type<T>::type;

  constexpr EnumIterator(T value) : value_(value) {}

  const auto& Value() { return value_; }
  static const auto& End() { return end_; }

  const UnderlyingType& Int() {
    return *reinterpret_cast<UnderlyingType*>(&value_);
  }

  EnumIterator& operator++(int) {
    value_ = static_cast<T>(Int() + 1);
    return *this;
  }

  EnumIterator& operator--(int) {
    value_ = static_cast<T>(Int() - 1);
    return *this;
  }

  bool operator!=(const EnumIterator& obj) const {
    return (obj.value_ != value_);
  }

 private:
  static const EnumIterator end_;
  T value_;
};

template <typename T, typename std::underlying_type<T>::type kEnd>
const EnumIterator<T, kEnd> EnumIterator<T, kEnd>::end_ =
    EnumIterator(static_cast<T>(kEnd));

template <kernel::mm::PageSize size>
struct TableEntryCount {};

template <>
struct TableEntryCount<kernel::mm::PageSize::_4KB> {
  static constexpr size_t value = (1ULL << 9);
};

template <>
struct TableEntryCount<kernel::mm::PageSize::_16KB> {
  static constexpr size_t value = (1ULL << 11);
};

template <>
struct TableEntryCount<kernel::mm::PageSize::_64KB> {
  static constexpr size_t value = (1ULL << 13);
};

template <kernel::mm::PageSize kPageSize>
struct DescriptorTable {
  using TableItem = TableDescriptor<kPageSize>;

  template <TableLvl kLevel>
  using EntryItem = EntryDescriptor<kPageSize, kLevel>;

  struct Item {
    union Value {
      TableItem table;
      EntryItem<TableLvl::_1> entry_1;
      EntryItem<TableLvl::_2> entry_2;
      EntryItem<TableLvl::_3> entry_3;

      Value() { new (&table) TableItem(); }
    };

    auto& Table() { return value.table; }

    Item& operator=(const TableItem& item) {
      value.table = item;
      return *this;
    }

    Item& operator=(const EntryItem<TableLvl::_1>& item) {
      value.entry_1 = item;
      return *this;
    }

    Item& operator=(const EntryItem<TableLvl::_2>& item) {
      value.entry_2 = item;
      return *this;
    }

    Item& operator=(const EntryItem<TableLvl::_3>& item) {
      value.entry_3 = item;
      return *this;
    }

    Value value;
  };

  Item data[TableEntryCount<kPageSize>::value];
};

static_assert(sizeof(DescriptorTable<kernel::mm::PageSize::_4KB>) == (512 * 8),
              "Wrong table size");

template <kernel::mm::PageSize kSize, std::size_t kAddressLength>
class TranslationTableConfig {};

template <std::size_t kAddressLength>
class TranslationTableConfig<kernel::mm::PageSize::_4KB, kAddressLength> {
 public:
  enum class BlockSize {
    _4KB,
    _2MB,
    _1GB,
    _512GB,
  };

  static constexpr BlockSize kMinBlockSize = BlockSize::_4KB;
  static constexpr kernel::mm::PageSize kPageSize = kernel::mm::PageSize::_4KB;
  using Table = DescriptorTable<kPageSize>;

  template <std::size_t kLength, typename Spec = void>
  struct MakeTableLevel {};

  template <std::size_t kLength>
  struct MakeTableLevel<kLength,
                        std::enable_if_t<(kLength > 12 && kLength <= 21)>> {
    static constexpr auto value = LookupLevel::_1;
  };

  template <std::size_t kLength>
  struct MakeTableLevel<kLength,
                        std::enable_if_t<(kLength > 20 && kLength <= 30)>> {
    static constexpr auto value = LookupLevel::_2;
  };

  template <std::size_t kLength>
  struct MakeTableLevel<kLength,
                        std::enable_if_t<(kLength > 30 && kLength <= 39)>> {
    static constexpr auto value = LookupLevel::_3;
  };

  template <std::size_t kLength>
  struct MakeTableLevel<kLength,
                        std::enable_if_t<(kLength > 39 && kLength <= 64)>> {
    static constexpr auto value = LookupLevel::_4;
  };

  static constexpr auto kTableLevel = MakeTableLevel<kAddressLength>::value;

  static inline size_t CalcIndex(const void* ptr, const LookupLevel level) {
    return ((reinterpret_cast<size_t>(ptr) >>
             (12 + (9 * (static_cast<LookupLevelInt>(level) - 1)))) &
            (512 - 1));
  }

  static inline uint64_t CalcEntryAddress(const void* ptr,
                                          const LookupLevel level) {
    return reinterpret_cast<uint64_t>(ptr) >>
           (12 + (9 * (static_cast<LookupLevelInt>(level) - 1)));
  }

  static inline BlockSize CalcBlockSizeFromTableLevel(const LookupLevel level) {
    switch (level) {
      case LookupLevel::_4:
        return BlockSize::_512GB;
      case LookupLevel::_3:
        return BlockSize::_1GB;
      case LookupLevel::_2:
        return BlockSize::_2MB;
      case LookupLevel::_1:
        return BlockSize::_4KB;
    }
    assert(false);
  }
};

template <kernel::mm::PageSize kPageSize, std::size_t kAddressLength,
          template <class> class AllocatorBase>
class TranslationTable {
 public:
  using Config = TranslationTableConfig<kPageSize, kAddressLength>;
  using BlockSize = typename Config::BlockSize;
  using Table = typename Config::Table;
  using Allocator = AllocatorBase<Table>;

  struct EntryParameters {
    types::MemoryAttr mem_attr;
    types::S2AP s2ap;
    types::SH sh;
    types::AF af;
    types::Contiguous contiguous;
    types::XN xn;
  };

  TranslationTable(Allocator& alloc) : alloc_(alloc), root_table_(nullptr) {
    DDBG_LOG("Constructor");
    root_table_ = MakeTable();
  }

  void* GetBase() { return reinterpret_cast<void*>(root_table_); }

  void Map(const void* v_ptr, const void* p_ptr, const BlockSize size,
           const EntryParameters& param) {
    auto chain_info = CreateTableChain(v_ptr, size);
    CreateEntry(v_ptr, p_ptr, size, param, std::get<LookupLevel>(chain_info),
                std::get<Table*>(chain_info));
  }

  std::pair<Table*, LookupLevel> CreateTableChain(const void* v_ptr,
                                                  const BlockSize size) {
    using LevelIterator = EnumIterator<LookupLevel, 0>;
    auto it = LevelIterator(Config::kTableLevel);
    Table* table = root_table_;
    for (; (LevelIterator::End() != it) &&
           (Config::CalcBlockSizeFromTableLevel(it.Value()) != size);
         it--) {
      size_t index = Config::CalcIndex(v_ptr, it.Value());
      Table* next_level_table = reinterpret_cast<Table*>(
          Table::TableItem::ToAddress(table->data[index].Table().data.address));
      if (nullptr == next_level_table) {
        DDBG_LOG("current level: ", it.Int());
        DDBG_LOG("table index: ", index);

        next_level_table = MakeTable();
        table->data[index] = typename Table::TableItem(
            types::Entry::ENTRY_TABLE,
            Table::TableItem::ToTableAddress(next_level_table),
            types::PXN_EXECUTE, types::XN_EXECUTE, types::AP_NOEFFECT,
            types::NSTABLE_NON_SECURE);
      }

      table = next_level_table;
    }

    return {table, it.Value()};
  }

  void CreateEntry(const void* v_ptr, const void* p_ptr, const BlockSize size,
                   const EntryParameters& param, const LookupLevel level,
                   Table* table) {
    uint64_t address = Config::CalcEntryAddress(p_ptr, level);
    auto entry_type = (Config::kMinBlockSize == size) ? types::ENTRY_TABLE
                                                      : types::ENTRY_BLOCK;
    const auto index = Config::CalcIndex(v_ptr, level);
    auto& entry = table->data[index];

    DDBG_LOG("New etry. table level: ", static_cast<LookupLevelInt>(level));
    DDBG_LOG("entry index: ", index);
    DDBG_LOG("address: ", address);

    if (LookupLevel::_3 == level) {
      entry = MakeEntry<TableLvl::_1>(entry_type, address, param);
    } else if (LookupLevel::_2 == level) {
      entry = MakeEntry<TableLvl::_2>(entry_type, address, param);
    } else if (LookupLevel::_1 == level) {
      entry = MakeEntry<TableLvl::_3>(entry_type, address, param);
    }
  }

  template <TableLvl kLvl>
  auto MakeEntry(const types::Entry entry_type, const uint64_t address,
                 const EntryParameters& param) {
    return typename Table::template EntryItem<kLvl>(
        entry_type, address, param.mem_attr, param.s2ap, param.sh, param.af,
        param.contiguous, param.xn);
  }

  Table* MakeTable() {
    Table* table = new (alloc_.AllocateAligned(sizeof(Table))) Table();
    DDBG_LOG("New table ptr: ", reinterpret_cast<uint64_t>(table));
    return table;
  }

 private:
  Allocator& alloc_;
  Table* root_table_;
};

}  // namespace mm
}  // namespace arm64
}  // namespace arch

#endif  // ARCH_ARM64_MM_TRANSLATION_TABLE_H_
