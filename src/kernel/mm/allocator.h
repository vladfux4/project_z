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
#ifndef KERNEL_MM_ALLOCATOR_H_
#define KERNEL_MM_ALLOCATOR_H_

#include <stdint.h>
#include <cstddef>

namespace kernel {
namespace mm {

/**
 * @brief The Allocator class
 */
class Allocator {
 public:
  /**
   * @brief Destructor
   */
  virtual ~Allocator() {}

  /**
   * @brief Allocate
   *
   * @param size Size in bytes
   *
   * @return pointer
   */
  virtual void* Allocate(const size_t size) = 0;

  /**
   * @brief Allocate
   *
   * @param size Size in bytes
   * @param aligned Aligment in bytes
   *
   * @return pointer
   */
  virtual void* Allocate(const size_t size, const size_t aligned) = 0;

  /**
   * @brief Deallocate
   *
   * @param ptr Pointer on allocated area
   */
  virtual void Deallocate(void* ptr) = 0;
};

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_ALLOCATOR_H_
