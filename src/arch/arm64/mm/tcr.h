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
#ifndef ARCH_ARM64_MM_TCR_H_
#define ARCH_ARM64_MM_TCR_H_

#include <cstdint>

#include "kernel/utils/register.h"

namespace arch {
namespace arm64 {
namespace mm {
namespace tcr {

enum class IPS : uint8_t {
  _32_BIT = 0b000,
  _36_BIT = 0b001,
  _40_BIT = 0b010,
};

enum class TG1 : uint8_t {
  _4KB = 0b10,
  _16KB = 0b01,
  _64KB = 0b11,
};

enum class SH1 : uint8_t {
  NON_SHAREABLE = 0b00,
  OUTER_SHAREABLE = 0b10,
  INNER_SHAREABLE = 0b11,
};

enum class ORGN1 : uint8_t {
  NON_CACHEABLE = 0b00,
  WRITE_BACK_CACHEABLE = 0b01,
  WRITE_THRU_CACHEABLE = 0b10,
  WRITE_BACK_NON_CACHEABLE = 0b11,
};

enum class IRGN1 : uint8_t {
  NON_CACHEABLE = 0b00,
  WRITE_BACK_CACHEABLE = 0b01,
  WRITE_THRU_CACHEABLE = 0b10,
  WRITE_BACK_NON_CACHEABLE = 0b11,
};

enum class EPD1 : uint8_t {
  WALK = 0b0,
  GENERATE_FAULT = 0b1,
};

enum class TG0 : uint8_t {
  _4KB = 0b00,
  _16KB = 0b10,
  _64KB = 0b01,
};

enum class SH0 : uint8_t {
  NON_SHAREABLE = 0b00,
  OUTER_SHAREABLE = 0b10,
  INNER_SHAREABLE = 0b11,
};

enum class ORGN0 : uint8_t {
  NON_CACHEABLE = 0b00,
  WRITE_BACK_CACHEABLE = 0b01,
  WRITE_THRU_CACHEABLE = 0b10,
  WRITE_BACK_NON_CACHEABLE = 0b11,
};

enum class IRGN0 : uint8_t {
  NON_CACHEABLE = 0b00,
  WRITE_BACK_CACHEABLE = 0b01,
  WRITE_THRU_CACHEABLE = 0b10,
  WRITE_BACK_NON_CACHEABLE = 0b11,
};

enum class EPD0 : uint8_t {
  WALK = 0b0,
  GENERATE_FAULT = 0b1,
};

struct TcrRegister
    : public utils::rtr::Register<
          TcrRegister, uint64_t, utils::rtr::Field<uint8_t, 6>,  // @0-5 T0SZ
          utils::rtr::Field<bool, 1>,                            // @6 reserved
          utils::rtr::Field<EPD0, 1>,                            // @7
          utils::rtr::Field<IRGN0, 2>,                           // @8-9
          utils::rtr::Field<ORGN0, 2>,                           // @10-11
          utils::rtr::Field<SH0, 2>,                             // @12-13
          utils::rtr::Field<TG0, 2>,                             // @14-15
          utils::rtr::Field<uint8_t, 6>,                         // @16-21 T1SZ
          utils::rtr::Field<bool, 1>,                            // @22 reserved
          utils::rtr::Field<EPD1, 1>,                            // @23
          utils::rtr::Field<IRGN1, 2>,                           // @24-25
          utils::rtr::Field<ORGN1, 2>,                           // @26-27
          utils::rtr::Field<SH1, 2>,                             // @28-29
          utils::rtr::Field<TG1, 2>,                             // @30-31
          utils::rtr::Field<IPS, 3>,                             // @32-34
          utils::rtr::Field<uint8_t, 2>,   // @35-36 reserved
          utils::rtr::Field<uint8_t, 2>,   // @37-38 TBI
          utils::rtr::Field<uint32_t, 25>  // @39-63 reserved
          > {
  using T0SZ = FieldAlias<0>;
  using EPD0 = FieldAlias<2>;
  using IRGN0 = FieldAlias<3>;
  using ORGN0 = FieldAlias<4>;
  using SH0 = FieldAlias<5>;
  using TG0 = FieldAlias<6>;
  using T1SZ = FieldAlias<7>;
  using EPD1 = FieldAlias<9>;
  using IRGN1 = FieldAlias<10>;
  using ORGN1 = FieldAlias<11>;
  using SH1 = FieldAlias<12>;
  using TG1 = FieldAlias<13>;
  using IPS = FieldAlias<14>;

  static constexpr uint8_t ConvertToTnSZ(const uint8_t address_length) {
    return (64 - address_length);
  }

  void FlushToEl1() { asm volatile("msr tcr_el1, %0; isb" : : "r"(value)); }
};

}  // namespace tcr
}  // namespace mm
}  // namespace arm64
}  // namespace arch

#endif  // ARCH_ARM64_MM_TCR_H_
