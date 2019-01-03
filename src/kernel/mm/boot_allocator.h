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

#include "kernel/mm/allocator.h"

namespace kernel {
namespace mm {

/**
 * @brief The Memory Pool class
 */
class BootAllocator : public Allocator {
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
  virtual ~BootAllocator();

  /// INTERFACE_START(kernel::Allocator)
  virtual void* Allocate(const size_t size);
  virtual void* Allocate(const size_t size, const size_t aligned);
  virtual void Deallocate(void* ptr);
  /// INTERFACE_END(kernel::Allocator)

  /**
   * @brief Get end pointer
   *
   * @return pointer
   */
  inline const uint8_t* GetEnd() { return head_; }

 private:
  uint8_t* head_;
};

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_BOOT_ALLOCATOR_H_
