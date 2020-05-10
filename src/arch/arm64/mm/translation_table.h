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

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

#include "arch/arm64/mm/translation_descriptor.h"
#include "kernel/logger.h"
#include "kernel/utils/enum_iterator.h"
#include "kernel/utils/variant.h"

#include "kernel/config.h"
#include "kernel/mm/physical_allocator.h"

namespace arch {
namespace arm64 {
namespace mm {

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

enum class LookupLevel : uint8_t {
  _1 = 1,
  _2 = 2,
  _3 = 3,
  _4 = 4,
};

using LookupLevelInt = std::underlying_type<LookupLevel>::type;

template <kernel::mm::PageSize kPageSize>
struct DescriptorTable {
 public:
  using TableItem = TableDescriptor<kPageSize>;
  static constexpr size_t kEntryCount = TableEntryCount<kPageSize>::value;

  template <TableLvl kLevel>
  using EntryItem = EntryDescriptor<kPageSize, kLevel>;

  using Item = utils::Variant<TableItem, TableItem, EntryItem<TableLvl::_1>,
                              EntryItem<TableLvl::_2>, EntryItem<TableLvl::_3>>;

  Item& at(const size_t i) { return data[i]; }

  class Iterator {
   public:
    Iterator(Item* buffer, size_t index) : buffer_(buffer), index_(index) {}

    Item* operator->() const { return &buffer_[index_]; }
    Item& operator*() const { return buffer_[index_]; }
    bool operator!=(const Iterator& it) const {
      return (it.buffer_ != buffer_) || (it.index_ != index_);
    }

    Iterator& operator++(int) {
      for (auto i = (index_ + 1); i < kEntryCount; i++) {
        if (Get<typename TableItem::EntryType>(Get<TableItem>(buffer_[i])) !=
            EntryType::INVALID) {
          index_ = i;
          return *this;
        }
      }

      index_ = kEndIndex;
      return *this;
    }

    static constexpr size_t kEndIndex = static_cast<size_t>(-1);

   private:
    Item* buffer_;
    size_t index_;
  };

  Iterator Begin() {
    for (size_t i = 0; i < kEntryCount; i++) {
      auto type = Get<typename TableItem::EntryType>(Get<TableItem>(data[i]));
      if (type != EntryType::INVALID) {
        return Iterator(data, i);
      }
    }

    return End();
  }

  Iterator End() { return Iterator(data, Iterator::kEndIndex); }

 private:
  Item data[kEntryCount];
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
          template <class, size_t> class AllocatorBase>
class TranslationTable {
 public:
  using Config = TranslationTableConfig<kPageSize, kAddressLength>;
  using BlockSize = typename Config::BlockSize;
  using Table = typename Config::Table;
  using Page = kernel::mm::Page<kPageSize>;
  using TableAllocator = AllocatorBase<Table, sizeof(Table)>;
  using PageAllocator = AllocatorBase<Page, sizeof(Page)>;

  struct EntryParameters {
    BlockSize size;
    MemoryAttr mem_attr;
    S2AP s2ap;
    SH sh;
    AF af;
    Contiguous contiguous;
    XN xn;
  };

  TranslationTable() : root_table_(nullptr) {
    LOG(DEBUG) << "Constructor";
    root_table_ = MakeTable();
  }

  ~TranslationTable() {
    LOG(DEBUG) << "Destructor";
    DeallocTable(*root_table_, Config::kTableLevel);
    TableAllocator::Deallocate(root_table_);
  }

  void DeallocTable(Table& table, const LookupLevel level) {
    for (auto it = table.Begin(); it != table.End(); it++) {
      auto& item = Get<typename Table::TableItem>(*it);

      auto type = Get<typename Table::TableItem::EntryType>(item);
      void* next_item = item.GetAddress();

//      LOG(VERBOSE) << "Found table item: " << next_item
//                   << " type: " << static_cast<uint64_t>(type)
//                   << " level: " << static_cast<size_t>(level);
      if (type == EntryType::TABLE) {
        if (level != LookupLevel::_1) {
          auto next_level = utils::EnumIterator<LookupLevel, 0>(level);
          next_level--;
          auto next_level_table = reinterpret_cast<Table*>(next_item);
          DeallocTable(*next_level_table, next_level.Value());
          TableAllocator::Deallocate(next_level_table);
        } else {
//          Pages are now deallocated by kernel::memory::Region
//          PageAllocator::Deallocate(reinterpret_cast<Page*>(next_item));
        }
      }
    }
  }

  void* GetBase() { return reinterpret_cast<void*>(root_table_); }

  void Map(const void* v_ptr, const void* p_ptr, const EntryParameters& param) {
    auto chain_info = CreateTableChain(v_ptr, param.size);
    CreateEntry(v_ptr, p_ptr, param, std::get<LookupLevel>(chain_info),
                std::get<Table*>(chain_info));
  }

  std::pair<Table*, LookupLevel> CreateTableChain(const void* v_ptr,
                                                  const BlockSize size) {
    using LevelIterator = utils::EnumIterator<LookupLevel, 0>;
    auto it = LevelIterator(Config::kTableLevel);
    Table* table = root_table_;
    for (; (LevelIterator::End() != it) &&
           (Config::CalcBlockSizeFromTableLevel(it.Value()) != size);
         it--) {
      size_t index = Config::CalcIndex(v_ptr, it.Value());
      Table* next_level_table = reinterpret_cast<Table*>(
          table->at(index)
              .template Get<typename Table::TableItem>()
              .GetAddress());

      if (nullptr == next_level_table) {
        next_level_table = MakeTable();
        auto new_item = typename Table::TableItem();
        new_item.Set(typename Table::TableItem::EntryType(EntryType::TABLE),
                     typename Table::TableItem::Address(
                         Table::TableItem::ToTableAddress(next_level_table)),
                     typename Table::TableItem::PXN(PXN::EXECUTE),
                     typename Table::TableItem::XN(XN::EXECUTE),
                     typename Table::TableItem::AP(AP::NOEFFECT),
                     typename Table::TableItem::NsTable(NSTable::NON_SECURE));
        table->at(index) = new_item;
      }

      table = next_level_table;
    }

    return {table, it.Value()};
  }

  void CreateEntry(const void* v_ptr, const void* p_ptr,
                   const EntryParameters& param, const LookupLevel level,
                   Table* table) {
    uint64_t address = Config::CalcEntryAddress(p_ptr, level);
    auto entry_type = (Config::kMinBlockSize == param.size) ? EntryType::TABLE
                                                            : EntryType::BLOCK;
    const auto index = Config::CalcIndex(v_ptr, level);
    auto& entry = table->at(index);

    if (LookupLevel::_3 == level) {
      entry = MakeEntry<TableLvl::_1>(entry_type, address, param);
    } else if (LookupLevel::_2 == level) {
      entry = MakeEntry<TableLvl::_2>(entry_type, address, param);
    } else if (LookupLevel::_1 == level) {
      entry = MakeEntry<TableLvl::_3>(entry_type, address, param);
    }
  }

  template <TableLvl kLvl>
  auto MakeEntry(const EntryType entry_type, const uint64_t address,
                 const EntryParameters& param) {
    using Entry = typename Table::template EntryItem<kLvl>;
    auto entry = Entry();

    entry.Set(typename Entry::EntryType(entry_type),
              typename Entry::Address(address),
              typename Entry::MemoryAttr(param.mem_attr),
              typename Entry::S2AP(param.s2ap), typename Entry::SH(param.sh),
              typename Entry::AF(param.af),
              typename Entry::Contiguous(param.contiguous),
              typename Entry::XN(param.xn));

    return entry;
  }

  Table* MakeTable() {
    Table* table = TableAllocator::Make();
    LOG(VERBOSE) << "New table by address: " << table;
    return table;
  }

 private:
  Table* root_table_;
};

class AddressSpace {
 public:
  using TranslationTable =
      arch::arm64::mm::TranslationTable<kernel::mm::KERNEL_PAGE_SIZE, kernel::mm::KERNEL_ADDRESS_LENGTH,
                                        kernel::mm::SlabAllocator>;
  TranslationTable translation_table;
};

}  // namespace mm
}  // namespace arm64
}  // namespace arch

#endif  // ARCH_ARM64_MM_TRANSLATION_TABLE_H_
