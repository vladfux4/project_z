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
#include <cstdint>

namespace kernel {
namespace mm {

template <class Index>
class IndexPool {
 public:
  struct IndexData {
    Index next;
  };

  IndexPool(IndexData* index_list, Index size)
      : size_(size), head_(0), free_items_(size), index_list_(index_list) {
    for (Index i = 0; i < size_; ++i) {
      index_list_[i].next = (i + 1);
    }
  }

  ~IndexPool() {}

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

  void Cut(const Index size) {
    if ((size < size_) && (free_items_ == size_)) {
      size_ = size;
      free_items_ = size;
    }
  }

  Index Size() const { return size_; }
  Index FreeSlots() const { return free_items_; }
  bool Empty() const { return Size() == FreeSlots(); }

  static constexpr Index kNoIndex = static_cast<Index>(-1);

 protected:

  Index size_;
  Index head_;
  Index free_items_;
  IndexData* index_list_;
};

template <class Index, template <class, size_t = 0> class AllocatorBase>
class DynamicIndexPool : public IndexPool<Index> {
 public:
  DynamicIndexPool(const Index size)
      : IndexPool<Index>(IndexAllocator::Allocate(size), size) {
  }

  ~DynamicIndexPool() { IndexAllocator::Deallocate(this->index_list_); }

 private:
  using IndexAllocator = AllocatorBase<typename IndexPool<Index>::IndexData>;
};

template <class Index, std::size_t kSize>
class StaticIndexPool : public IndexPool<Index> {
 public:
  StaticIndexPool()
      : IndexPool<Index>(buffer_, kSize) {
  }

 private:
  typename IndexPool<Index>::IndexData buffer_[kSize];
};

template <typename T, std::size_t kSize, class Index>
class StaticPool : public StaticIndexPool<Index, kSize> {
 public:
  StaticPool()
      : StaticIndexPool<Index, kSize>() {
  }

  T* Allocate() {
    T* ret_val = nullptr;
    auto index = IndexPool<Index>::Allocate();
    if (IndexPool<Index>::kNoIndex != index) {
      ret_val = &reinterpret_cast<T*>(data_)[index];
    }

    return ret_val;
  }

  void Deallocate(const T* item) {
    Index index = (item - reinterpret_cast<T*>(data_));
    IndexPool<Index>::Deallocate(index);
  }

  bool IsRelated(const T* item) {
    T* buffer = reinterpret_cast<T*>(data_);
    return (item >= buffer && item < (buffer + kSize));
  }

 private:
  uint8_t data_[sizeof(T) * kSize];
};

template <typename Index, typename T>
struct StaticPoolSize {
  static constexpr std::size_t ForBuffer(const std::size_t bytes) {
    return (bytes - sizeof(IndexPool<Index>)) / (sizeof(typename IndexPool<Index>::IndexData) + sizeof(T));
  }
};

static_assert (StaticPoolSize<std::size_t, std::size_t>::ForBuffer(4096) == 254);
static_assert (sizeof(IndexPool<std::size_t>) + sizeof(std::size_t) + sizeof(std::size_t)
               == sizeof(StaticPool<std::size_t, 1, std::size_t>));
static_assert (sizeof(IndexPool<std::size_t>) + (sizeof(std::size_t) + sizeof(std::size_t))*2
               == sizeof(StaticPool<std::size_t, 2, std::size_t>));

template <class T, class Index,
          template <class, size_t = 0> class AllocatorBase>
class Pool : public DynamicIndexPool<Index, AllocatorBase> {
 public:
  using IndexPoolType = DynamicIndexPool<Index, AllocatorBase>;
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

  Index AllocateByIndex() { return IndexPoolType::Allocate(); }

  void DeallocateByIndex(const Index index) {
    IndexPoolType::Deallocate(index);
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
