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
#ifndef KERNEL_MM_PHYSICAL_ALLOCATOR_H_
#define KERNEL_MM_PHYSICAL_ALLOCATOR_H_

#include <cstddef>
#include <cstdint>

#include "kernel/mm/page_pool.h"

namespace kernel {
namespace mm {

template <typename T, size_t kAlignment>
struct SlabAllocator {  /// TODO Implement slab mechanism
  using PagePoolAllocator = PhysicalPagePoolAllocator<T, kAlignment>;
  static T* Allocate() { return PagePoolAllocator::Allocate(); }
  static void Deallocate(T* address) { PagePoolAllocator::Deallocate(address); }
};

template <typename T, typename Spec = void>
struct AllocatorSelector {
  template <typename ValueType, size_t kValueAlignment>
  using Type = SlabAllocator<ValueType, kValueAlignment>;
};

template <typename T>
struct AllocatorSelector<
    T,
    std::enable_if_t<sizeof(T) == PageSizeInfo<KERNEL_PAGE_SIZE>::in_bytes>> {
  template <typename ValueType, size_t kValueAlignment>
  using Type = PhysicalPagePoolAllocator<ValueType, kValueAlignment>;
};

template <typename T, size_t kAlignment = 0>
struct PhysicalAllocator : AllocatorSelector<T>::template Type<T, kAlignment> {
};

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_PHYSICAL_ALLOCATOR_H_
