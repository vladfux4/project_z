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

namespace arch {
namespace arm64 {
namespace mm {

MMU::MMU(const uint8_t address_length)
    : kAddressLength(address_length), tcr_() {}

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

  // set TCR
  tcr_ = TCR::ToRaw(TCR::IPS::_32_BIT) | TCR::ToRaw(TCR::TG1::_4KB) |
         TCR::ToRaw(TCR::SH1::INNER_SHAREABLE) |
         TCR::ToRaw(TCR::ORGN1::WRITE_BACK_CACHEABLE) |
         TCR::ToRaw(TCR::IRGN1::WRITE_BACK_CACHEABLE) |
         TCR::ToRaw(TCR::EPD1::WALK) | TCR::ToT1SZRaw(kAddressLength) |
         TCR::ToRaw(TCR::TG0::_4KB) | TCR::ToRaw(TCR::SH0::INNER_SHAREABLE) |
         TCR::ToRaw(TCR::ORGN0::WRITE_BACK_CACHEABLE) |
         TCR::ToRaw(TCR::IRGN0::WRITE_BACK_CACHEABLE) |
         TCR::ToRaw(TCR::EPD0::WALK) | TCR::ToT0SZRaw(kAddressLength);
  tcr_.Flush();

  // toggle some bits in system control register to enable page translation
  asm volatile("isb; mrs %0, sctlr_el1" : "=r"(r));
  r |= 0xC00800;    // set mandatory reserved bits
  r |= (1 << 12) |  // I, Instruction cache enable. This is an enable bit for
                    // instruction caches at EL0 and EL1
       (1 << 4) |   // SA0, stack Alignment Check Enable for EL0
       (1 << 3) |   // SA, Stack Alignment Check Enable
       (1 << 2) |   // C, Data cache enable. This is an enable bit for data
                    // caches at EL0 and EL1
       (1 << 1) |   // A, Alignment check enable bit
       (1 << 0);    // set M, enable MMU
  asm volatile("msr sctlr_el1, %0" : : "r"(r));
  asm volatile("isb");
}

}  // namespace mm
}  // namespace arm64
}  // namespace arch
