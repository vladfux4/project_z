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
#ifndef KERNEL_MM_PAGE_POOL_H_
#define KERNEL_MM_PAGE_POOL_H_

#include <cstddef>
#include <cstdint>

#include "gen/arch_types_gen.h"
#include "kernel/config.h"
#include "kernel/mm/boot_allocator.h"
#include "kernel/mm/pool.h"

namespace kernel {
namespace mm {

struct __attribute__((__packed__)) PageInfo {
  bool value;
};

class PagePool : public Pool<PageInfo, size_t, BootAllocator> {
 public:
  PagePool(const size_t length)
      : Pool(length / PageSizeInfo<KERNEL_PAGE_SIZE>::in_bytes) {}
};

class PhysicalPagePool : public PagePool {
 public:
  static void Construct(const size_t length) {
    static bool create = true;
    if (create) {
      ref_ = BootAllocator<PhysicalPagePool>::Allocate();
      new (ref_) PhysicalPagePool(length);
      create = false;
    }
  }

  static PhysicalPagePool* Get() { return ref_; }

  uint8_t* BeginAddress() { return begin_; }
  void SetBeginAddress(uint8_t* address) { begin_ = address; }

 private:
  PhysicalPagePool(const size_t length) : PagePool(length), begin_(nullptr) {}

  static PhysicalPagePool* ref_;

  uint8_t* begin_;
};

template <typename T, size_t kAlignment>
struct PhysicalPagePoolAllocator {
  static_assert(sizeof(T) <= PageSizeInfo<KERNEL_PAGE_SIZE>::in_bytes);

  static T* Allocate() {
    auto pool = PhysicalPagePool::Get();
    auto info = pool->Allocate();
    auto index = pool->ToIndex(info);
    uint8_t* address = pool->BeginAddress() +
                       (PageSizeInfo<KERNEL_PAGE_SIZE>::in_bytes * index);

    DDBG_LOG("PhysicalPagePoolAllocator alloc index: ",
             reinterpret_cast<size_t>(index));
    DDBG_LOG("PhysicalPagePoolAllocator alloc address: ",
             reinterpret_cast<size_t>(address));
    return reinterpret_cast<T*>(address);
  }

  static void Deallocate(T* address) {
    auto pool = PhysicalPagePool::Get();

    DDBG_LOG("PhysicalPagePoolAllocator dealloc address: ",
             reinterpret_cast<size_t>(address));
    auto index = ((reinterpret_cast<uint8_t*>(address) - pool->BeginAddress()) /
                  PageSizeInfo<KERNEL_PAGE_SIZE>::in_bytes);
    DDBG_LOG("PhysicalPagePoolAllocator dealloc index: ",
             reinterpret_cast<size_t>(index));

    pool->DeallocateByIndex(index);
  }
};

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_PAGE_POOL_H_
