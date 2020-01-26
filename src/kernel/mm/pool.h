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
#ifndef KERNEL_MM_POOL_H_
#define KERNEL_MM_POOL_H_

#include <cstddef>

namespace kernel {
namespace mm {

template <class Index, template <class, size_t = 0> class AllocatorBase>
class IndexPool {
 public:
  IndexPool(const Index size)
      : kSize(size), head_(0), free_items_(size), index_list_(nullptr) {
    index_list_ = IndexAllocator::Allocate(kSize);
    for (Index i = 0; i < kSize; ++i) {
      index_list_[i].next = (i + 1);
    }
  }

  ~IndexPool() { IndexAllocator::Deallocate(index_list_); }

  Index Allocate() {
    Index ret_val = kNoIndex;
    if (0 != free_items_) {
      ret_val = head_;
      free_items_--;
      head_ = index_list_[head_].next;
    }

    return ret_val;
  }

  void Deallocate(const Index index) {
    index_list_[index].next = head_;
    head_ = index;
    free_items_++;
  }

  Index Size() const { return kSize; }
  Index FreeItems() const { return free_items_; }

  static constexpr Index kNoIndex = static_cast<Index>(-1);

 private:
  struct IndexData {
    Index next;
  };

  using IndexAllocator = AllocatorBase<IndexData>;

  const Index kSize;
  Index head_;
  Index free_items_;
  IndexData* index_list_;
};

template <class T, class Index,
          template <class, size_t = 0> class AllocatorBase>
class Pool : public IndexPool<Index, AllocatorBase> {
 public:
  using IndexPoolType = IndexPool<Index, AllocatorBase>;
  using TypeAllocator = AllocatorBase<T>;

  Pool(const Index size)
      : IndexPoolType(size), buffer_(TypeAllocator::Allocate(size)) {}

  T* Allocate() {
    T* ret_val = nullptr;
    auto index = IndexPoolType::Allocate();
    if (IndexPoolType::kNoIndex != index) {
      ret_val = &buffer_[index];
    }

    return ret_val;
  }

  void Deallocate(const T* item) {
    Index index = (item - buffer_);
    IndexPoolType::Deallocate(ToIndex(item));
  }

  Index ToIndex(const T* item) { return (item - buffer_); }

 private:
  T* buffer_;
};

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_POOL_H_
