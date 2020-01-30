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
#ifndef ARCH_ARM64_SYSTEM_H_
#define ARCH_ARM64_SYSTEM_H_

#include <cstdint>

#include "kernel/utils/register.h"

namespace arch {
namespace arm64 {
namespace sys {

enum class IPS : uint8_t {
  _32_BIT = 0b000,
  _36_BIT = 0b001,
  _40_BIT = 0b010,
};

// Exception endianness
enum class EE : uint8_t {
  LITTLE = 0b0,
  BIG = 0b1,
};

// Endianness of explicit data accesses at EL0
enum class EOE : uint8_t {
  LITTLE = 0b0,
  BIG = 0b1,
};

// Access to DC ZVA instruction at EL0.
enum class DZE : uint8_t {
  PROHIBITED = 0b0,
  ALLOWED = 0b1,
};

struct SystemControlRegister
    : public utils::rtr::Register<
          SystemControlRegister, uint64_t,
          utils::rtr::Field<bool, 1>,  // @0 M - Enable the MMU.
          utils::rtr::Field<bool, 1>,  // @1 A - Alignment check enable bit.
          utils::rtr::Field<bool, 1>,  // @2 C - Data cache enable. This is an
                                       // enable bit for data caches at EL0 and
                                       // EL1. Data accesses to cacheable Normal
                                       // memory are cached.

          utils::rtr::Field<bool, 1>,  // @3 SA - Stack Alignment Check Enable
          utils::rtr::Field<bool, 1>,  // @4 SA0 - Stack Alignment Check Enable
                                       // for EL0.

          utils::rtr::Field<bool, 1>,  // @5 CP15BEN - CP15 barrier enable. If
                                       // implemented, it is an enable bit for
                                       // the AArch32 CP15 DMB, DSB, and ISB
                                       // barrier operations.

          utils::rtr::Field<bool, 1>,  // @6 reserved
          utils::rtr::Field<bool, 1>,  // @7 ITD - IT Disable
          utils::rtr::Field<bool, 1>,  // @8 SED - SETEND Disable. Disables
                                       // SETEND instructions at EL0 using
                                       // AArch32.

          utils::rtr::Field<bool, 1>,  // @9 UMA - User Mask Access. Controls
                                       // access to interrupt masks from EL0,
                                       // when EL0 is using AArch64

          utils::rtr::Field<uint8_t, 2>,  // @10 - 11 reserved
          utils::rtr::Field<bool, 1>,  // @12 I - Instruction cache enable. This
                                       // is an enable bit for instruction
                                       // caches at EL0 and EL1. Instruction
                                       // accesses to cacheable Normal memory
                                       // are cached.

          utils::rtr::Field<bool, 1>,  // @13 reserved
          utils::rtr::Field<DZE, 1>,   // @14 DZE
          utils::rtr::Field<bool, 1>,  // @15 UCT - When set, enables EL0 access
                                       // in AArch64 to the CTR_EL0 register.

          utils::rtr::Field<bool, 1>,  // @16 nTWI - Not trap WFI. A value of 1
                                       // means that WFI instructions are
                                       // executed as normal

          utils::rtr::Field<bool, 1>,  // @17 reserved
          utils::rtr::Field<bool, 1>,  // @18 nTWE - Not trap WFE. A value of 1
                                       // means that WFE instructions are
                                       // executed as normal

          utils::rtr::Field<bool, 1>,  // @19 WXN - Write permission implies XN
                                       // (eXecute Never).

          utils::rtr::Field<uint8_t, 4>,  // @20 - 23 reserved
          utils::rtr::Field<EOE, 1>,      // @24 EOE
          utils::rtr::Field<EE, 1>,       // @25 EE
          utils::rtr::Field<bool, 1>,     // @26 UCI - When set, enables
                                          // EL0 access in AArch64 for DC
                                          // CVAU, DC CIVAC, DC CVAC, and
                                          // IC IVAU instructions

          utils::rtr::Field<uint8_t, 5>  // @27 - 31 reserved
          > {
  using M = FieldAlias<0>;
  using A = FieldAlias<1>;
  using C = FieldAlias<2>;
  using SA = FieldAlias<3>;
  using SA0 = FieldAlias<4>;
  using CP15BEN = FieldAlias<5>;
  using ITD = FieldAlias<7>;
  using SED = FieldAlias<8>;
  using UMA = FieldAlias<9>;
  using I = FieldAlias<11>;
  using DZE = FieldAlias<13>;
  using UCT = FieldAlias<14>;
  using nTWI = FieldAlias<15>;
  using nTWE = FieldAlias<17>;
  using WXN = FieldAlias<18>;
  using EOE = FieldAlias<20>;
  using EE = FieldAlias<21>;
  using UCI = FieldAlias<22>;

  void ReadEl1() { asm volatile("mrs %0, sctlr_el1" : "=r"(value)); }
  void FlushToEl1() { asm volatile("msr sctlr_el1, %0" : : "r"(value)); }
};

}  // namespace sys
}  // namespace arm64
}  // namespace arch

#endif  // ARCH_ARM64_SYSTEM_H_
