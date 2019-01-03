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

#include <stdint.h>
#include <assert.h>

#include "kernel/types.h"

namespace arch {
namespace arm64 {
namespace mm {

/**
 * @brief The Translation table lvl
 */
enum class TableLvl {
 _1,
 _2,
 _3,
};

namespace types {

/**
 * @brief The Descriptor entry type
 */
enum Entry {
  ENTRY_INVALID = 0b00U,
  ENTRY_BLOCK = 0b01U,
  ENTRY_TABLE = 0b11U,
};

enum AP {
  AP_NOEFFECT = 0b00U,
  AP_NO_EL0 = 0b01U,
  AP_NO_WRITE = 0b10U,
  AP_NO_WRITE_EL0_READ = 0b11U,
};

enum PXN {
  PXN_OFF = 0,
  PXN_ON = 1,
};

enum XN {
  XN_OFF = 0,
  XN_ON = 1,
};

enum NS {
  NS_OFF = 0,
  NS_ON = 1,
};

enum S2AP {
  S2AP_NORMAL = 0,
  S2AP_NOREAD_EL0 = 1,
  S2AP_NO_WRITE = 2,
};

enum SH {
  SH_NON_SHAREABLE = 0,
  SH_OUTER_SHAREABLE = 2, //   Outter shareable
  SH_INNER_SHAREABLE = 3, //   Inner shareable
};

enum AF {
  AF_OFF = 0,
  AF_ON = 1,
};

enum Contiguous {
  CONTIGUOUS_OFF = 0,
  CONTIGUOUS_ON = 1,
};

enum MemoryAttr {
  MEMORYATTR_DEVICE_NGNRNE = 0,
  MEMORYATTR_DEVICE_NGNRE = 1,
  MEMORYATTR_DEVICE_GRE = 2,
  MEMORYATTR_NORMAL_NC = 3,
  MEMORYATTR_NORMAL = 4,
};


}  // namespace types

/**
 * @brief The Translation table descriptor template
 */
template<kernel::mm::PageSize kPageSize>
struct __attribute__((__packed__)) TableDescriptor {
 public:
  TableDescriptor() {
    *(reinterpret_cast<uint64_t*>(&data)) = 0;
  }

  TableDescriptor(const types::Entry entry, const uint64_t address,
                  const types::PXN pxn, const types::XN xn,
                  const types::AP ap, const types::NS ns) {
    *(reinterpret_cast<uint64_t*>(&data)) = 0;

    data.entry = entry;
    data.address = address;
    data.pxn = pxn;
    data.xn = xn;
    data.ap = ap;
    data.ns = ns;
  }

  static constexpr uint8_t AddressStartBit() {
    switch (kPageSize) {
      case kernel::mm::PageSize::_4KB: { return 12; }
      case kernel::mm::PageSize::_16KB: { return 14; }
      case kernel::mm::PageSize::_64KB: { return 16; }
    }

    return 0xff;
  }

  static inline uint64_t ToTableAddress(const void* address) {
    return (reinterpret_cast<uint64_t>(address) >> AddressStartBit());
  }

  static inline void* ToAddress(const uint64_t address) {
    return reinterpret_cast<void*>(address << AddressStartBit());
  }

  struct Layout {
    types::Entry entry : 2; // @0-1 entry type
    uint64_t _reserved2 : (AddressStartBit() - 2); // @2-11 Set to 0
    uint64_t address : (48 - AddressStartBit()); // @12-47 36 Bits of address
    uint64_t _reserved48_58 : 11; // @48-58 Set to 0
    types::PXN pxn : 1; // @59 Never allow execution from a lower EL level
    types::XN xn : 1; // @60 Never allow translation from a lower EL level
    types::AP ap : 2; // @61-62 AP Table control .. see enumerate options
    types::NS ns : 1; // @63 Secure state, for accesses from Non-secure state this bit is RES0 and is ignored
  };

  Layout data;
};

static_assert(sizeof(TableDescriptor<kernel::mm::PageSize::_4KB>) == 8,
              "TableDescriptor should be 0x08 bytes in size");
static_assert(sizeof(TableDescriptor<kernel::mm::PageSize::_16KB>) == 8,
              "TableDescriptor should be 0x08 bytes in size");
static_assert(sizeof(TableDescriptor<kernel::mm::PageSize::_64KB>) == 8,
              "TableDescriptor should be 0x08 bytes in size");

/**
 * @brief The Translation table entry descriptor template
 */
template<kernel::mm::PageSize kPageSize, TableLvl kLvl>
struct __attribute__((__packed__)) EntryDescriptor {
 static_assert(!((kernel::mm::PageSize::_16KB == kPageSize) && (TableLvl::_1 == kLvl)),
               "Not allowed combination");
 static_assert(!((kernel::mm::PageSize::_64KB == kPageSize) && (TableLvl::_1 == kLvl)),
               "Not allowed combination");

 public:
  EntryDescriptor() {
    *(reinterpret_cast<uint64_t*>(&data)) = 0;
  }

  EntryDescriptor(const types::Entry entry, const uint64_t address,
                  const types::MemoryAttr mem_attr,
                  const types::S2AP s2ap, const types::SH sh,
                  const types::AF af, const types::Contiguous contiguous,
                  const types::XN xn) {
    *(reinterpret_cast<uint64_t*>(&data)) = 0;

    data.entry = entry;
    data.mem_attr = mem_attr;
    data.s2ap = s2ap;
    data.sh = sh;
    data.af = af;
    data.address = address;
    data.contiguous = contiguous;
    data.xn = xn;
  }

  static constexpr uint8_t AddressStartBit() {
    if (TableLvl::_3 == kLvl) {
      switch (kPageSize) {
        case kernel::mm::PageSize::_4KB: { return 12; }
        case kernel::mm::PageSize::_16KB: { return 14; }
        case kernel::mm::PageSize::_64KB: { return 16; }
      }
    }

    switch (kPageSize) {
      case kernel::mm::PageSize::_4KB: { return (TableLvl::_2 == kLvl) ? 21 : 30; }
      case kernel::mm::PageSize::_16KB: { return 25; }
      case kernel::mm::PageSize::_64KB: { return 29; }
    }

    return 0xff;
  }

  struct Layout {
    types::Entry entry : 2; // @0-1 Always 1 for a block table
    types::MemoryAttr mem_attr : 4; // @2-5
    types::S2AP s2ap : 2; // @6-7
    types::SH sh : 2; // @8-9
    types::AF af : 1; // @10 Accessable flag
    uint64_t _reserved1 : (AddressStartBit() - 11); // @11-(X-1) Set to 0
    uint64_t address : (48 - AddressStartBit()); // @X-47 N Bits of address
    uint64_t _reserved48_51 : 4; // @48-51 Set to 0
    types::Contiguous contiguous : 1; // @52 Contiguous
    uint64_t _reserved53 : 1; // @53 Set to 0
    types::XN xn : 1; // @54 No execute if bit set
    uint64_t _reserved55_63 : 9; // @55-63 Set to 0
  };

  Layout data;
};

static_assert(sizeof(EntryDescriptor<kernel::mm::PageSize::_4KB, TableLvl::_1>) == 8,
              "EntryDescriptor should be 0x08 bytes in size");
static_assert(sizeof(EntryDescriptor<kernel::mm::PageSize::_4KB, TableLvl::_2>) == 8,
              "EntryDescriptor should be 0x08 bytes in size");
static_assert(sizeof(EntryDescriptor<kernel::mm::PageSize::_4KB, TableLvl::_3>) == 8,
              "EntryDescriptor should be 0x08 bytes in size");
static_assert(sizeof(EntryDescriptor<kernel::mm::PageSize::_16KB, TableLvl::_2>) == 8,
              "EntryDescriptor should be 0x08 bytes in size");
static_assert(sizeof(EntryDescriptor<kernel::mm::PageSize::_16KB, TableLvl::_3>) == 8,
              "EntryDescriptor should be 0x08 bytes in size");
static_assert(sizeof(EntryDescriptor<kernel::mm::PageSize::_64KB, TableLvl::_2>) == 8,
              "EntryDescriptor should be 0x08 bytes in size");
static_assert(sizeof(EntryDescriptor<kernel::mm::PageSize::_64KB, TableLvl::_3>) == 8,
              "EntryDescriptor should be 0x08 bytes in size");

}  // namespace mm
}  // namespace arm64
}  // namespace arch

#endif  // ARCH_ARM64_MM_TRANSLATION_DESCRIPTOR_H_
