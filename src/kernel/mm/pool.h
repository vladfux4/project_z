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

#include <stdint.h>
#include "kernel/mm/allocator.h"

namespace kernel {
namespace mm {

/**
 * @brief The Memory Pool class
 */
template <class Item, class Index>
class Pool {
 public:
  /**
   * @brief Constructor
   */
  Pool(Item* buffer, const Index size, Allocator& allocator);

  /**
   * @brief Destructor
   */
  ~Pool();

  /**
   * @brief Allocate item in pool
   *
   * @return address
   */
  Item* Allocate();

  /**
   * @brief Deallocate item from pool
   *
   * @param item Pointer in item
   */
  void Deallocate(Item* item);

  /**
   * @brief Get size of pool
   *
   * @return elements count
   */
  inline Index Size() const { return kSize; }

  /**
   * @brief Get free items count
   *
   * @return count
   */
  inline Index FreeItems() const { return free_items_; }

 private:
  /**
   * @brief The Index Data struct
   */
  struct IndexData {
    Index next;
  };

  const Index kSize;
  Allocator& allocator_;
  Index head_;
  Index free_items_;
  IndexData* index_list_;
  Item* buffer_;
};

template<class Item, class Index>
Pool<Item, Index>::Pool(Item* buffer, const Index size,
                        Allocator& allocator)
    : kSize(size),
      allocator_(allocator),
      head_(0),
      free_items_(size),
      index_list_(nullptr),
      buffer_(buffer) {
  index_list_ = reinterpret_cast<IndexData*>(allocator.Allocate(kSize * sizeof(IndexData)));
  for (Index i = 0; i < kSize; ++i) {
    index_list_[i].next = (i + 1);
  }
}

template<class Item, class Index>
Pool<Item, Index>::~Pool() {
  allocator_.Deallocate(index_list_);
}

template<class Item, class Index>
Item* Pool<Item, Index>::Allocate() {
  Item* ret_val = nullptr;

  if (0 != free_items_) {
    ret_val = &buffer_[head_];
    free_items_--;
    head_ = index_list_[head_].next;
  }

  return ret_val;
}

template<class Item, class Index>
void Pool<Item, Index>::Deallocate(Item* item) {
  Index index = (item - buffer_);
  index_list_[index].next = head_;
  head_ = index;
  free_items_++;
}

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_POOL_H_
