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

namespace arch {
namespace arm64 {
namespace mm {

MMU::MMU()
    : table_(),
      vtable_(),
      tcr_() {
}

void MMU::Enable() {
  uint64_t r;

  asm volatile("dsb sy");
  // Set mem attr
  r = ((0x00ull << (types::MEMORYATTR_DEVICE_NGNRNE * 8)) | \
       (0x04ull << (types::MEMORYATTR_DEVICE_NGNRE * 8)) | \
       (0x0cull << (types::MEMORYATTR_DEVICE_GRE * 8)) | \
       (0x44ull << (types::MEMORYATTR_NORMAL_NC * 8)) | \
       (0xffull << (types::MEMORYATTR_NORMAL * 8)));
  asm volatile ("msr mair_el1, %0" : : "r" (r));

  asm volatile ("msr ttbr0_el1, %0" : : "r" (table_.GetBasePtr()));
  asm volatile ("msr ttbr1_el1, %0" : : "r" (vtable_.GetBasePtr()));

  // set TCR
  tcr_ =
      TCR::ToRaw(TCR::IPS::_32_BIT) |
      TCR::ToRaw(TCR::TG1::_4KB) |
      TCR::ToRaw(TCR::SH1::INNER_SHAREABLE) |
      TCR::ToRaw(TCR::ORGN1::WRITE_BACK_CACHEABLE) |
      TCR::ToRaw(TCR::IRGN1::WRITE_BACK_CACHEABLE) |
      TCR::ToRaw(TCR::EPD1::WALK) |
      TCR::ToT1SZRaw(39) |
      TCR::ToRaw(TCR::TG0::_4KB) |
      TCR::ToRaw(TCR::SH0::INNER_SHAREABLE) |
      TCR::ToRaw(TCR::ORGN0::WRITE_BACK_CACHEABLE) |
      TCR::ToRaw(TCR::IRGN0::WRITE_BACK_CACHEABLE) |
      TCR::ToRaw(TCR::EPD0::WALK) |
      TCR::ToT0SZRaw(39);
  tcr_.Flush();

  // toggle some bits in system control register to enable page translation
  asm volatile ("isb; mrs %0, sctlr_el1" : "=r" (r));
  r |= 0xC00800;   // set mandatory reserved bits
  r |= (1<<12) |   // I, Instruction cache enable. This is an enable bit for instruction caches at EL0 and EL1
       (1<<4)  |   // SA0, tack Alignment Check Enable for EL0
       (1<<3)  |   // SA, Stack Alignment Check Enable
       (1<<2)  |   // C, Data cache enable. This is an enable bit for data caches at EL0 and EL1
       (1<<1)  |   // A, Alignment check enable bit
       (1<<0);     // set M, enable MMU
  asm volatile ("msr sctlr_el1, %0" : : "r" (r));
  asm volatile("isb");
}

}  // namespace mm
}  // namespace arm64
}  // namespace arch