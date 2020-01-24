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
#ifndef KERNEL_MM_MEMORY_H_
#define KERNEL_MM_MEMORY_H_

#include <stdint.h>
#include <cstddef>

#include "arch/arm64/mm/translation_table.h"
#include "gen/arch_types_gen.h"
#include "kernel/config.h"
#include "kernel/mm/boot_allocator.h"
#include "kernel/mm/page_registry.h"

namespace kernel {
namespace mm {

/**
 * @brief The Memory Pool class
 */
class Memory {
 public:
  Memory();
  ~Memory();

  void Init();

 private:
  typedef arch::arm64::mm::TranslationTable<
      KERNEL_PAGE_SIZE, KERNEL_ADDRESS_LENGTH, BootAllocator>
      TranslationTable;

  arch::mm::MMU mmu_;
  TranslationTable::Allocator boot_allocator_;
  PageRegistry* pages_;
  TranslationTable p_table_;
  TranslationTable v_table_;
};

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_MEMORY_H_
