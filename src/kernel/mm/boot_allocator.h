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
#ifndef KERNEL_MM_BOOT_ALLOCATOR_H_
#define KERNEL_MM_BOOT_ALLOCATOR_H_

#include <cassert>
#include <cstddef>
#include <cstdint>

#include "kernel/mm/boot_stack.h"

namespace kernel {
namespace mm {

template <class T, size_t kAlignment = 0>
class BootAllocator {
 public:
  static T* Allocate() {
    DDBG_LOG("boot_allocator alloc");
    return reinterpret_cast<T*>(BootStack::Push(sizeof(T), kAlignment));
  }

  static T* Allocate(const size_t n) {
    DDBG_LOG("boot_allocator alloc count: ", n);
    return reinterpret_cast<T*>(BootStack::Push((sizeof(T) * n), kAlignment));
  }

  static void Deallocate(T* item) {
    (void)item;
    assert(false);
  }
};

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_BOOT_ALLOCATOR_H_
