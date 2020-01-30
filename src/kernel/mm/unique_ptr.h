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
#ifndef KERNEL_MM_UNIQUE_PTR_H_
#define KERNEL_MM_UNIQUE_PTR_H_

#include <cstddef>
#include <utility>

namespace kernel {
namespace mm {

template <typename T, template <class, size_t> class AllocatorBase,
          size_t kAlignment = 0>
class UniquePointer {
 public:
  using Allocator = AllocatorBase<T, kAlignment>;

  explicit UniquePointer(T* data) : data_(data) {}
  UniquePointer(UniquePointer&& ptr) : data_(ptr.data_) { ptr.data_ = nullptr; }

  ~UniquePointer() {
    if (data_ != nullptr) {
      Allocator::Deallocate(data_);
    }
  }

  UniquePointer(UniquePointer const&) = delete;
  UniquePointer& operator=(UniquePointer const&) = delete;

  T* operator->() const { return data_; }
  T& operator*() const { return *data_; }

  T* Get() const { return data_; }
  explicit operator bool() const { return data_; }

  T* Release() {
    T* result = data_;
    data_ = nullptr;
    return result;
  }

  template <typename... Args>
  static UniquePointer Make(Args&&... args) {
    T* ptr = Allocator::Allocate();
    new (ptr) T(std::forward<Args>(args)...);
    return UniquePointer(ptr);
  }

 private:
  T* data_;
};

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_UNIQUE_PTR_H_