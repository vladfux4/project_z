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
#ifndef KERNEL_MM_SHARED_PTR_H_
#define KERNEL_MM_SHARED_PTR_H_

#include <cstddef>
#include <utility>

namespace kernel {
namespace mm {

class ReferenceCounter {
 public:
  ReferenceCounter() : counter_(0){};

  ReferenceCounter(const ReferenceCounter&) = delete;
  ReferenceCounter& operator=(const ReferenceCounter&) = delete;

  void reset() { counter_ = 0; }
  std::size_t Get() { return counter_; }

  void operator++() { counter_++; }
  void operator++(int) { counter_++; }
  void operator--() { counter_--; }
  void operator--(int) { counter_--; }

 private:
  std::size_t counter_{};
};


template <typename T, template <class, size_t> class AllocatorBase,
          size_t kAlignment = 0>
class SharedPointer {
 public:

  template <typename Type>
  using Allocator = AllocatorBase<Type, kAlignment>;

  explicit SharedPointer(T* ptr = nullptr) {
    counter_ = new (Allocator<ReferenceCounter>::Allocate()) ReferenceCounter();
    ptr_ = ptr;
    if (ptr) {
      (*counter_)++;
    }
  }

  SharedPointer(SharedPointer& sp) {
    ptr_ = sp.ptr_;
    counter_ = sp.counter_;
    (*counter_)++;
  }

  SharedPointer& operator=(SharedPointer const&) = delete;

  std::size_t use_count() { return counter_->Get(); }
  T* Get() { return ptr_; }

  T& operator*() { return *ptr_; }
  T* operator->() { return ptr_; }

  ~SharedPointer() {
    (*counter_)--;
    if (counter_->Get() == 0) {
      Allocator<ReferenceCounter>::Deallocate(counter_);
      ptr_->~T();
      Allocator<T>::Deallocate(ptr_);
    }
  }

  template <typename... Args>
  static SharedPointer Make(Args&&... args) {
    T* ptr = Allocator<T>::Allocate();
    new (ptr) T(std::forward<Args>(args)...);
    return SharedPointer(ptr);
  }

 private:
  ReferenceCounter* counter_;
  T* ptr_;
};

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_SHARED_PTR_H_
