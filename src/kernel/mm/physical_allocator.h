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
#include "kernel/mm/boot_allocator.h"

namespace kernel {
namespace mm {

struct PageSlabAllocatorBase {
 public:
  static inline size_t used_objects = 0;

  static void LogInfo() {
    LOG(DEBUG) << "Used objects: " << used_objects;
  }
};

template <typename T, size_t kAlignment>
struct PageSlabAllocator : public PageSlabAllocatorBase {
 public:
  struct Node {
    using Index = std::size_t;
    static constexpr Index kPageSize = PageSizeInfo<KERNEL_PAGE_SIZE>::in_bytes;
    static constexpr Index kPoolBufferSize = (kPageSize - sizeof (Node*));
    static constexpr Index kPoolSize = StaticPoolSize<Index, T>::ForBuffer(kPoolBufferSize);

    Node() : next(nullptr), pool() {}

    Node* next;
    StaticPool<T, kPoolSize, Index> pool;
  };
  static_assert (sizeof (Node) <= Node::kPageSize);

  static T* Allocate() {
    if (!root_) {
      root_ = new (PagePoolAllocator<Node>::Allocate()) Node();
      LOG(VERBOSE) << "Alloc root: " << root_ << " type size: " << sizeof (T);
    }

    Node* node = root_;
    while (node != nullptr) {
      if (0 != node->pool.FreeSlots()) {
        used_objects++;
        return node->pool.Allocate();
      }

      if (node->next == nullptr) {
        node->next = new (PagePoolAllocator<Node>::Allocate()) Node();
        node = node->next;
      }
    }

    return nullptr;
  }

  static void Deallocate(T* address) {
    Node* prev = nullptr;
    Node* node = root_;

    while (node != nullptr) {
      if (node->pool.IsRelated(address)) {
        used_objects--;
        node->pool.Deallocate(address);

        if (node->pool.Empty()) {
          if (prev != nullptr) {
            prev->next = node->next;
          } else {
            root_ = node->next;
          }

          node->~Node();
          PagePoolAllocator<Node>::Deallocate(node);
        }

        break;
      } else {
        prev = node;
        node = node->next;
      }
    }
  }

 private:
  static Node* root_;
};

template <typename T, size_t kAlignment>
typename PageSlabAllocator<T, kAlignment>::Node*
PageSlabAllocator<T, kAlignment>::root_ = nullptr;

template <typename T, typename Spec = void>
struct AllocatorSelector {
};

template <typename T>
struct AllocatorSelector<
    T,
    std::enable_if_t<sizeof(T) < PageSizeInfo<KERNEL_PAGE_SIZE>::in_bytes>> {
  template <typename ValueType, size_t kValueAlignment>
  using Type = PageSlabAllocator<ValueType, kValueAlignment>;
};

template <typename T>
struct AllocatorSelector<
    T,
    std::enable_if_t<sizeof(T) == PageSizeInfo<KERNEL_PAGE_SIZE>::in_bytes>> {
  template <typename ValueType, size_t kValueAlignment>
  using Type = PagePoolAllocator<ValueType, kValueAlignment>;
};

template <typename T, size_t kAlignment = 0>
struct SlabAllocator : AllocatorSelector<T>::template Type<T, kAlignment> {
  template <typename... Args>
  static T* Make(Args&&... args) {
    return new (SlabAllocator::Allocate()) T(std::forward<Args>(args)...);
  }
};

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_PHYSICAL_ALLOCATOR_H_
