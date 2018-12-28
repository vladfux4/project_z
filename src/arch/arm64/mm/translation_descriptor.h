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
#ifndef ARCH_ARM64_TRANSLATION_DESCRIPTOR_H_
#define ARCH_ARM64_TRANSLATION_DESCRIPTOR_H_

#include <stdint.h>
#include <assert.h>

namespace arch {
namespace arm64 {
namespace mm {
namespace types {

/**
 * @brief The Translation table lvl
 */
enum class TableLvl {
 _1,
 _2,
 _3,
};

/**
 * @brief The Page size enum
 */
enum class GranuleSize {
  _4KB,
  _16KB,
  _64KB,
};

/**
 * @brief The Descriptor entry type
 */
enum EntryType {
  ENTRY_INVALID = 0b00U,
  ENTRY_BLOCK = 0b01U,
  ENTRY_TABLE = 0b11U,
};

enum ApType {
  AP_NOEFFECT = 0b00U,
  AP_NO_EL0 = 0b01U,
  AP_NO_WRITE = 0b10U,
  AP_NO_WRITE_EL0_READ = 0b11U,
};

enum PxnType {
  PXN_OFF = 0,
  PXN_ON = 1,
};

enum XnType {
  XN_OFF = 0,
  XN_ON = 1,
};

enum NsType {
  NS_OFF = 0,
  NS_ON = 1,
};

template<GranuleSize kGranule>
struct __attribute__((__packed__)) TableDescriptor {
 public:
  static constexpr uint8_t AddressStartBit(const GranuleSize granule) {
    switch (granule) {
      case GranuleSize::_4KB: { return 12; }
      case GranuleSize::_16KB: { return 14; }
      case GranuleSize::_64KB: { return 16; }
    }

    return 0xff;
  }

  EntryType entry_type : 2; // @0-1 Always 3 for a page table
  uint64_t _reserved2 : (AddressStartBit(kGranule) - 2); // @2-11 Set to 0
  uint64_t address : (48 - AddressStartBit(kGranule)); // @12-47 36 Bits of address
  uint64_t _reserved48_58 : 11; // @48-58 Set to 0
  PxnType pxn : 1; // @59 Never allow execution from a lower EL level
  XnType xn : 1; // @60 Never allow translation from a lower EL level
  ApType ap : 2; // @61-62 AP Table control .. see enumerate options
  NsType ns : 1; // @63 Secure state, for accesses from Non-secure state this bit is RES0 and is ignored
};

static_assert(sizeof(TableDescriptor<GranuleSize::_4KB>) == 8, "TableDescriptor should be 0x08 bytes in size");
static_assert(sizeof(TableDescriptor<GranuleSize::_16KB>) == 8, "TableDescriptor should be 0x08 bytes in size");
static_assert(sizeof(TableDescriptor<GranuleSize::_64KB>) == 8, "TableDescriptor should be 0x08 bytes in size");

enum S2APType {
  S2AP_NOREAD_EL0 = 1,  // No read access for EL0
  S2AP_NO_WRITE = 2,  // No write access
};

enum SHType {
  SH_OUTER_SHAREABLE = 2, //   Outter shareable
  SH_INNER_SHAREABLE = 3, //   Inner shareable
};

template<GranuleSize kGranule, TableLvl kLvl>
struct __attribute__((__packed__)) EntryDescriptor {
 static_assert(!((GranuleSize::_16KB == kGranule) && (TableLvl::_1 == kLvl)), "Not allowed combination");
 static_assert(!((GranuleSize::_64KB == kGranule) && (TableLvl::_1 == kLvl)), "Not allowed combination");

 public:
  static constexpr uint8_t AddressStartBit(const GranuleSize granule, const TableLvl lvl) {
    if (TableLvl::_3 == lvl) {
      switch (granule) {
        case GranuleSize::_4KB: { return 12; }
        case GranuleSize::_16KB: { return 14; }
        case GranuleSize::_64KB: { return 16; }
      }
    }

    switch (granule) {
      case GranuleSize::_4KB: { return (TableLvl::_2 == lvl) ? 21 : 30; }
      case GranuleSize::_16KB: { return 25; }
      case GranuleSize::_64KB: { return 29; }
    }

    return 0xff;
  }

  EntryType entry_type : 2; // @0-1  Always 1 for a block table
  uint64_t mem_attr : 4; // @2-5
  S2APType s2ap : 2; // @6-7
  SHType sh : 2; // @8-9
  uint64_t af : 1; // @10  Accessable flag
  uint64_t _reserved1 : (AddressStartBit(kGranule, kLvl) - 11); // @11-(X-1) Set to 0
  uint64_t address : (48 - AddressStartBit(kGranule, kLvl)); // @X-47 N Bits of address
  uint64_t _reserved48_51 : 4; // @48-51 Set to 0
  uint64_t contiguous : 1; // @52 Contiguous
  uint64_t _reserved53 : 1; // @53 Set to 0
  uint64_t xn : 1; // @54 No execute if bit set
  uint64_t _reserved55_63 : 9; // @55-63 Set to 0
};

static_assert(sizeof(EntryDescriptor<GranuleSize::_4KB, TableLvl::_1>) == 8, "EntryDescriptor should be 0x08 bytes in size");
static_assert(sizeof(EntryDescriptor<GranuleSize::_4KB, TableLvl::_2>) == 8, "EntryDescriptor should be 0x08 bytes in size");
static_assert(sizeof(EntryDescriptor<GranuleSize::_4KB, TableLvl::_3>) == 8, "EntryDescriptor should be 0x08 bytes in size");
static_assert(sizeof(EntryDescriptor<GranuleSize::_16KB, TableLvl::_2>) == 8, "EntryDescriptor should be 0x08 bytes in size");
static_assert(sizeof(EntryDescriptor<GranuleSize::_16KB, TableLvl::_3>) == 8, "EntryDescriptor should be 0x08 bytes in size");
static_assert(sizeof(EntryDescriptor<GranuleSize::_64KB, TableLvl::_2>) == 8, "EntryDescriptor should be 0x08 bytes in size");
static_assert(sizeof(EntryDescriptor<GranuleSize::_64KB, TableLvl::_3>) == 8, "EntryDescriptor should be 0x08 bytes in size");

}  // namespace types
}  // namespace mm
}  // namespace arm64
}  // namespace arch

#endif  // ARCH_ARM64_TRANSLATION_DESCRIPTOR_H_
