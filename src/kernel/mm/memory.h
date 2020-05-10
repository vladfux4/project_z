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

#include <cstddef>
#include <cstdint>

#include "arch/arm64/mm/translation_table.h"
#include "gen/arch_types_gen.h"
#include "kernel/config.h"
#include "kernel/mm/address_space.h"
#include "kernel/mm/boot_allocator.h"
#include "kernel/mm/physical_allocator.h"
#include "kernel/mm/unique_ptr.h"

namespace kernel {
namespace mm {

class Memory {
 public:
  Memory();

  void Select(AddressSpace& space);

  PagedRegion::Sptr CreatePagedRegion(const size_t count);
  AddressSpace::Uptr CreateAddressSpace();

 private:

  arch::mm::MMU mmu_;
  AddressSpace::Uptr p_space_;
};

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_MEMORY_H_
