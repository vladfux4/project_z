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
#ifndef KERNEL_MM_BOOT_ALLOCATOR_H_
#define KERNEL_MM_BOOT_ALLOCATOR_H_

#include <stdint.h>
#include <cstddef>

namespace kernel {
namespace mm {

/**
 * @brief The Memory Pool class
 */
template <class T>
class BootAllocator {
 public:
  /**
   * @brief Constructor
   *
   * @param start_ptr Memory area begin pointer
   */
  BootAllocator(uint8_t* start_ptr);

  /**
   * @brief Destructor
   */
  ~BootAllocator();

  T* Allocate(const size_t count = 1);
  T* AllocateAligned(const size_t aligned, const size_t count = 1);
  void Deallocate(T* ptr);

  /**
   * @brief Get end pointer
   *
   * @return pointer
   */
  inline const uint8_t* GetEnd() { return head_; }

 private:
  uint8_t* head_;
};

template <class T>
BootAllocator<T>::BootAllocator(uint8_t* start_ptr)
    : head_(start_ptr) {
}

template <class T>
BootAllocator<T>::~BootAllocator() {
}

template <class T>
T* BootAllocator<T>::Allocate(const size_t count) {
  T* ret_val = reinterpret_cast<T*>(head_);
  head_ += (sizeof(T) * count);
  return ret_val;
}

template <class T>
T* BootAllocator<T>::AllocateAligned(const size_t aligned, const size_t count) {
  while (0 != (reinterpret_cast<size_t>(head_) % aligned)) { head_++; }

  return Allocate(count);
}

template <class T>
void BootAllocator<T>::Deallocate(T* ptr) { //delete is not supported
}

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_BOOT_ALLOCATOR_H_
