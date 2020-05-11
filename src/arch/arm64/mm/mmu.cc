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
#include "arch/arm64/mm/mmu.h"

#include "arch/arm64/mm/translation_descriptor.h"
#include "arch/arm64/system.h"
#include "kernel/config.h"

namespace arch {
namespace arm64 {
namespace mm {

MMU::MMU() : tcr_() {}

void MMU::Enable() {
  uint64_t r;

  asm volatile("dsb sy");
  // Set mem attr
  r = ((0x00ull << (static_cast<std::size_t>(MemoryAttr::DEVICE_NGNRNE) * 8)) |
       (0x04ull << (static_cast<std::size_t>(MemoryAttr::DEVICE_NGNRE) * 8)) |
       (0x0cull << (static_cast<std::size_t>(MemoryAttr::DEVICE_GRE) * 8)) |
       (0x44ull << (static_cast<std::size_t>(MemoryAttr::NORMAL_NC) * 8)) |
       (0xffull << (static_cast<std::size_t>(MemoryAttr::NORMAL) * 8)));
  asm volatile("msr mair_el1, %0" : : "r"(r));

  constexpr auto trc_value = tcr::TcrRegister::MakeValue(
      tcr::TcrRegister::IPS(tcr::IPS::_32_BIT),
      tcr::TcrRegister::TG1(tcr::TG1::_4KB),
      tcr::TcrRegister::SH1(tcr::SH1::INNER_SHAREABLE),
      tcr::TcrRegister::ORGN1(tcr::ORGN1::WRITE_BACK_CACHEABLE),
      tcr::TcrRegister::IRGN1(tcr::IRGN1::WRITE_BACK_CACHEABLE),
      tcr::TcrRegister::EPD1(tcr::EPD1::WALK),
      tcr::TcrRegister::T1SZ(
          tcr::TcrRegister::ConvertToTnSZ(kernel::mm::KERNEL_ADDRESS_LENGTH)),
      tcr::TcrRegister::TG0(tcr::TG0::_4KB),
      tcr::TcrRegister::SH0(tcr::SH0::INNER_SHAREABLE),
      tcr::TcrRegister::ORGN0(tcr::ORGN0::WRITE_BACK_CACHEABLE),
      tcr::TcrRegister::IRGN0(tcr::IRGN0::WRITE_BACK_CACHEABLE),
      tcr::TcrRegister::EPD0(tcr::EPD0::WALK),
      tcr::TcrRegister::T0SZ(
          tcr::TcrRegister::ConvertToTnSZ(kernel::mm::KERNEL_ADDRESS_LENGTH)));
  tcr_.Set(trc_value);
  tcr_.FlushToEl1();

  // toggle some bits in system control register to enable page translation
  asm volatile("isb");
  sys::SystemControlRegister scr;
  scr.ReadEl1();
  constexpr auto crc_value = sys::SystemControlRegister::MakeValue(
      sys::SystemControlRegister::I(true),
      sys::SystemControlRegister::SA0(true),
      sys::SystemControlRegister::SA(true), sys::SystemControlRegister::C(true),
      sys::SystemControlRegister::A(true), sys::SystemControlRegister::M(true));
  scr.Set(crc_value);
  scr.FlushToEl1();
  asm volatile("isb");
}

void MMU::SelectAddressSpace(AddressSpace&) {
}

}  // namespace mm
}  // namespace arm64
}  // namespace arch
