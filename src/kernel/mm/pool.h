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

namespace kernel {
namespace mm {

/**
 * @brief The Memory Pool class
 */
template <class Item, class Index, Index kSize>
class Pool {
 public:
  /**
   * @brief Constructor
   */
  Pool();
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

  Index head_;
  Index free_items_;
  IndexData index_list_[kSize];
  uint8_t buffer_[kSize * sizeof(Item)];
};

template<class Item, class Index, Index kSize>
Pool<Item, Index, kSize>::Pool()
    : head_(0),
      free_items_(kSize),
      index_list_(),
      buffer_() {
  for (Index i = 0; i < kSize; ++i) {
    index_list_[i].next = (i + 1);
  }
}

template<class Item, class Index, Index kSize>
Item* Pool<Item, Index, kSize>::Allocate() {
  Item* ret_val = nullptr;

  if (0 != free_items_) {
    ret_val = reinterpret_cast<Item*>(&buffer_[head_ * sizeof(Item)]);
    free_items_--;
    head_ = index_list_[head_].next;
  }

  return ret_val;
}

template<class Item, class Index, Index kSize>
void Pool<Item, Index, kSize>::Deallocate(Item* item) {
  Index index = (item - reinterpret_cast<Item*>(buffer_));
  index_list_[index].next = head_;
  head_ = index;
  free_items_++;
}

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_POOL_H_
