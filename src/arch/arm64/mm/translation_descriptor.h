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
#ifndef ARCH_ARM64_MM_TRANSLATION_DESCRIPTOR_H_
#define ARCH_ARM64_MM_TRANSLATION_DESCRIPTOR_H_

#include <cstdint>

#include "kernel/types.h"
#include "kernel/utils/register.h"

namespace arch {
namespace arm64 {
namespace mm {

/**
 * @brief The Translation table lvl
 */
enum class TableLvl { _1, _2, _3 };

/**
 * @brief The Descriptor entry type
 */
enum class EntryType {
  INVALID = 0b00U,
  BLOCK = 0b01U,
  TABLE = 0b11U,
};

/**
 * @brief The access permission mode enum
 *
 * The access read/write permission bits control
 * access to the corresponding memory region
 */
enum class AP {
  NOEFFECT = 0b00U,
  NO_EL0 = 0b01U,
  NO_WRITE = 0b10U,
  NO_WRITE_EL0_READ = 0b11U,
};

/**
 * @brief The execute never bit enum
 */
enum class XN {
  EXECUTE = 0,
  NO_EXECUTE = 1,
};

/**
 * @brief The privileged execute never bit enum
 */
enum class PXN {
  EXECUTE = 0,
  NO_EXECUTE = 1,
};

/**
 * @brief The NSTable bit enum
 *
 * NSTable bit indicates whether the table identified in the descriptor
 * is in Secure or Non-secure memory
 */
enum class NSTable {
  SECURE = 0,
  NON_SECURE = 1,
};

/**
 * @brief The NS bit enum
 *
 * NS bit indicates whether the descriptor refers to the Secure or
 * the Non-secure address map
 */
enum class NS {
  SECURE = 0,
  NON_SECURE = 1,
};

/**
 * @brief The S2AP data access permissions enum
 */
enum class S2AP {
  NORMAL = 0,
  NOREAD_EL0 = 1,
  NO_WRITE = 2,
};

enum class SH {
  NON_SHAREABLE = 0,
  OUTER_SHAREABLE = 2,
  INNER_SHAREABLE = 3,
};

/**
 * @brief The AF bit enum
 *
 * The Access flag indicates when a page or section of memory is accessed
 * for the first time since the Access flag in the corresponding
 * translation table descriptor was set to 0
 */
enum class AF {
  HANDLE = 0,
  IGNORE = 1,
};

/**
 * @brief The Contiguous bit enum
 *
 * A hint bit indicating that the translation table entry is one of
 * a contiguous set or entries, that might be cached in a single TLB entry
 */
enum class Contiguous {
  OFF = 0,
  ON = 1,
};

enum class MemoryAttr {
  DEVICE_NGNRNE = 0,
  DEVICE_NGNRE = 1,
  DEVICE_GRE = 2,
  NORMAL_NC = 3,
  NORMAL = 4,
};

template <kernel::mm::PageSize kPageSize>
struct TableAddressStartBit {};

template <>
struct TableAddressStartBit<kernel::mm::PageSize::_4KB> {
  static constexpr uint8_t value = 12;
};

template <>
struct TableAddressStartBit<kernel::mm::PageSize::_16KB> {
  static constexpr uint8_t value = 14;
};

template <>
struct TableAddressStartBit<kernel::mm::PageSize::_64KB> {
  static constexpr uint8_t value = 16;
};

template <kernel::mm::PageSize kPageSize>
struct __attribute__((__packed__)) TableDescriptor
    : public utils::rtr::Register<
          TableDescriptor<kPageSize>, std::size_t,
          utils::rtr::Field<EntryType, 2>,  // @0-1 entry type
          utils::rtr::Field<uint64_t, (TableAddressStartBit<kPageSize>::value -
                                       2)>,  // @2-11 Set to 0
          utils::rtr::Field<uint64_t, (48 - TableAddressStartBit<kPageSize>::
                                                value)>,  // @12-47 36 Bits of
                                                          // address
          utils::rtr::Field<uint64_t, 11>,                // @48-58 Set to 0
          utils::rtr::Field<PXN, 1>,     // @59 Never allow execution from a
                                         // lower EL level
          utils::rtr::Field<XN, 1>,      // @60 Never allow translation from
                                         // a lower EL level
          utils::rtr::Field<AP, 2>,      // @61-62 AP Table control .. see
                                         // enumerate options
          utils::rtr::Field<NSTable, 1>  // @63 Secure state, for
                                         // accesses from Non-secure
          > {
  using EntryType = typename TableDescriptor<kPageSize>::template FieldAlias<0>;
  using Address = typename TableDescriptor<kPageSize>::template FieldAlias<2>;
  using PXN = typename TableDescriptor<kPageSize>::template FieldAlias<4>;
  using XN = typename TableDescriptor<kPageSize>::template FieldAlias<5>;
  using AP = typename TableDescriptor<kPageSize>::template FieldAlias<6>;
  using NsTable = typename TableDescriptor<kPageSize>::template FieldAlias<7>;

  static inline uint64_t ToTableAddress(const void* address) {
    return (reinterpret_cast<uint64_t>(address) >>
            TableAddressStartBit<kPageSize>::value);
  }

  inline void* GetAddress() {
    const uint64_t address = this->template Get<Address>();
    return reinterpret_cast<void*>(address
                                   << TableAddressStartBit<kPageSize>::value);
  }
};

static_assert(sizeof(TableDescriptor<kernel::mm::PageSize::_4KB>) == 8,
              "TableDescriptor should be 0x08 bytes in size");
static_assert(sizeof(TableDescriptor<kernel::mm::PageSize::_16KB>) == 8,
              "TableDescriptor should be 0x08 bytes in size");
static_assert(sizeof(TableDescriptor<kernel::mm::PageSize::_64KB>) == 8,
              "TableDescriptor should be 0x08 bytes in size");

template <kernel::mm::PageSize kPageSize, TableLvl kLvl>
struct EntryAddressStartBit {};

template <>
struct EntryAddressStartBit<kernel::mm::PageSize::_4KB, TableLvl::_3> {
  static constexpr uint8_t value = 12;
};

template <>
struct EntryAddressStartBit<kernel::mm::PageSize::_16KB, TableLvl::_3> {
  static constexpr uint8_t value = 14;
};

template <>
struct EntryAddressStartBit<kernel::mm::PageSize::_64KB, TableLvl::_3> {
  static constexpr uint8_t value = 16;
};

template <>
struct EntryAddressStartBit<kernel::mm::PageSize::_4KB, TableLvl::_2> {
  static constexpr uint8_t value = 21;
};

template <TableLvl kLvl>
struct EntryAddressStartBit<kernel::mm::PageSize::_16KB, kLvl> {
  static constexpr uint8_t value = 25;
};

template <TableLvl kLvl>
struct EntryAddressStartBit<kernel::mm::PageSize::_64KB, kLvl> {
  static constexpr uint8_t value = 29;
};

template <>
struct EntryAddressStartBit<kernel::mm::PageSize::_4KB, TableLvl::_1> {
  static constexpr uint8_t value = 30;
};

template <kernel::mm::PageSize kPageSize, TableLvl kLvl>
struct __attribute__((__packed__)) EntryDescriptor
    : public utils::rtr::Register<
          EntryDescriptor<kPageSize, kLvl>, std::size_t,
          utils::rtr::Field<EntryType, 2>,   // @0-1 Always 1 for a block
                                             // table
          utils::rtr::Field<MemoryAttr, 4>,  // @2-5
          utils::rtr::Field<S2AP, 2>,        // @6-7
          utils::rtr::Field<SH, 2>,          // @8-9
          utils::rtr::Field<AF, 1>,          // @10 Accessable flag
          utils::rtr::Field<uint64_t,
                            (EntryAddressStartBit<kPageSize, kLvl>::value -
                             11)>,  // @11-(X-1) Set to 0
          utils::rtr::Field<uint64_t,
                            (48 - EntryAddressStartBit<kPageSize, kLvl>::
                                      value)>,  // @X-47 N Bits of address
          utils::rtr::Field<uint8_t, 4>,        // @48-51 Set to 0
          utils::rtr::Field<Contiguous, 1>,     // @52 Contiguous
          utils::rtr::Field<bool, 1>,           // @53 Set to 0
          utils::rtr::Field<XN, 1>,             // @54 No execute if bit set
          utils::rtr::Field<uint16_t, 9>        // @55-63 Set to 0
          > {
  using EntryType =
      typename EntryDescriptor<kPageSize, kLvl>::template FieldAlias<0>;
  using MemoryAttr =
      typename EntryDescriptor<kPageSize, kLvl>::template FieldAlias<1>;
  using S2AP =
      typename EntryDescriptor<kPageSize, kLvl>::template FieldAlias<2>;
  using SH = typename EntryDescriptor<kPageSize, kLvl>::template FieldAlias<3>;
  using AF = typename EntryDescriptor<kPageSize, kLvl>::template FieldAlias<4>;
  using Address =
      typename EntryDescriptor<kPageSize, kLvl>::template FieldAlias<6>;
  using Contiguous =
      typename EntryDescriptor<kPageSize, kLvl>::template FieldAlias<8>;
  using XN = typename EntryDescriptor<kPageSize, kLvl>::template FieldAlias<10>;

  static_assert(!((kernel::mm::PageSize::_16KB == kPageSize) &&
                  (TableLvl::_1 == kLvl)),
                "Wrong combination");
  static_assert(!((kernel::mm::PageSize::_64KB == kPageSize) &&
                  (TableLvl::_1 == kLvl)),
                "Wrong combination");
};

static_assert(
    sizeof(EntryDescriptor<kernel::mm::PageSize::_4KB, TableLvl::_1>) == 8,
    "EntryDescriptor should be 0x08 bytes in size");
static_assert(
    sizeof(EntryDescriptor<kernel::mm::PageSize::_4KB, TableLvl::_2>) == 8,
    "EntryDescriptor should be 0x08 bytes in size");
static_assert(
    sizeof(EntryDescriptor<kernel::mm::PageSize::_4KB, TableLvl::_3>) == 8,
    "EntryDescriptor should be 0x08 bytes in size");
static_assert(
    sizeof(EntryDescriptor<kernel::mm::PageSize::_16KB, TableLvl::_2>) == 8,
    "EntryDescriptor should be 0x08 bytes in size");
static_assert(
    sizeof(EntryDescriptor<kernel::mm::PageSize::_16KB, TableLvl::_3>) == 8,
    "EntryDescriptor should be 0x08 bytes in size");
static_assert(
    sizeof(EntryDescriptor<kernel::mm::PageSize::_64KB, TableLvl::_2>) == 8,
    "EntryDescriptor should be 0x08 bytes in size");
static_assert(
    sizeof(EntryDescriptor<kernel::mm::PageSize::_64KB, TableLvl::_3>) == 8,
    "EntryDescriptor should be 0x08 bytes in size");

}  // namespace mm
}  // namespace arm64
}  // namespace arch

#endif  // ARCH_ARM64_MM_TRANSLATION_DESCRIPTOR_H_
