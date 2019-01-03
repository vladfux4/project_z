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
#include "arch/arm64/mm/translation_table.h"
#include <cstddef>

namespace arch {
namespace arm64 {
namespace mm {

TranslationTable4K2MBlock::TranslationTable4K2MBlock()
    : table_lvl1_(),
      block_table_lvl2_() {
  // Just 2 valid entries mapping the 2GB in stage2
  for (size_t i = 0; i < kBlockTableCount; i++) {
    table_lvl1_.data[i] = Table4KLvl1::Entry(
        types::Entry::ENTRY_TABLE,
        Table4KLvl1::Entry::ToTableAddress(&(block_table_lvl2_[i].data[0])),
        types::PXN_OFF, types::XN_OFF, types::AP_NOEFFECT, types::NS_ON);
  }

  size_t base = 0;
  // 880Mb of ram
  for (; base < 440; base++) {
    block_table_lvl2_[0].data[base] = Table2MBlockLvl2::Entry(
        types::ENTRY_BLOCK, base,
        types::MEMORYATTR_NORMAL,
        types::S2AP_NORMAL, types::SH_INNER_SHAREABLE,
        types::AF_ON, types::CONTIGUOUS_OFF, types::XN_OFF);
  }

  // VC ram up to 0x3F000000
  for (; base < 512 - 8; base++) {
    block_table_lvl2_[0].data[base] = Table2MBlockLvl2::Entry(
        types::ENTRY_BLOCK, base,
        types::MEMORYATTR_NORMAL_NC,
        types::S2AP_NORMAL, types::SH_NON_SHAREABLE,
        types::AF_ON, types::CONTIGUOUS_OFF, types::XN_OFF);
  }

  // 16 MB peripherals at 0x3F000000 - 0x40000000
  for (; base < 512; base++) {
    block_table_lvl2_[0].data[base] = Table2MBlockLvl2::Entry(
        types::ENTRY_BLOCK, base,
        types::MEMORYATTR_DEVICE_NGNRNE,
        types::S2AP_NORMAL, types::SH_NON_SHAREABLE,
        types::AF_ON, types::CONTIGUOUS_OFF, types::XN_OFF);
  }

  // 2 MB for mailboxes at 0x40000000
  block_table_lvl2_[1].data[0] = Table2MBlockLvl2::Entry(
        types::ENTRY_BLOCK, base,
        types::MEMORYATTR_DEVICE_NGNRNE,
        types::S2AP_NORMAL, types::SH_NON_SHAREABLE,
              types::AF_ON, types::CONTIGUOUS_OFF, types::XN_OFF);
}

}  // namespace mm
}  // namespace arm64
}  // namespace arch
