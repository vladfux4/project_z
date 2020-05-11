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

#include "kernel/config.h"
#include "kernel/mm/boot_allocator.h"
#include "kernel/mm/pool.h"
#include "kernel/utils/static_wrapper.h"

namespace kernel {
namespace mm {

struct __attribute__((__packed__)) PageInfo {
  bool value;
};

class PagePool : public Pool<PageInfo, size_t, BootAllocator> {
 public:
  static constexpr size_t GetPageCount(const size_t bytes) {
    return (bytes / PageSizeInfo<KERNEL_PAGE_SIZE>::in_bytes);
  }

  void CutBytes(const size_t length) { Pool::Cut(GetPageCount(length)); }

  uint8_t* BeginAddress() { return begin_; }
  void SetBeginAddress(uint8_t* address) { begin_ = address; }

  PagePool(const size_t length) : Pool(GetPageCount(length)), begin_(nullptr) {}

  void LogInfo() {
    LOG(DEBUG) << "Free: " << FreeSlots();
    LOG(DEBUG) << "Used pages: " << (Size() - FreeSlots());
  }

  uint8_t* begin_;
};

using StaticPagePool = utils::StaticWrapper<PagePool>;

template <typename T, size_t kAlignment = 0>
struct PagePoolAllocator {
  static_assert(sizeof(T) <= PageSizeInfo<KERNEL_PAGE_SIZE>::in_bytes);

  static T* Allocate() {
    auto& pool = StaticPagePool::Value();
    auto info = pool.Allocate();
    auto index = pool.ToIndex(info);
    uint8_t* address = pool.BeginAddress() +
                       (PageSizeInfo<KERNEL_PAGE_SIZE>::in_bytes * index);

    LOG(VERBOSE) << "Alloc index: " << index
                 << " address:" << address;
    return reinterpret_cast<T*>(address);
  }

  static void Deallocate(T* address) {
    auto& pool = StaticPagePool::Value();
    auto index = ((reinterpret_cast<uint8_t*>(address) - pool.BeginAddress()) /
                  PageSizeInfo<KERNEL_PAGE_SIZE>::in_bytes);

    address->~T();
    LOG(VERBOSE) << "Dealloc index: " << index
                 << " address:" << address;
    pool.DeallocateByIndex(index);
  }

 private:
};

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_PAGE_POOL_H_
