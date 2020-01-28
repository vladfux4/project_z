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
#ifndef KERNEL_MM_BOOT_STACK_H_
#define KERNEL_MM_BOOT_STACK_H_

#include <cstddef>
#include <cstdint>

#include "kernel/logger.h"

namespace kernel {
namespace mm {

class BootStack {
 public:
  static uint8_t* Push(const size_t bytes, const size_t alignment = 0) {
    if (0 != alignment) {
      while (0 != (reinterpret_cast<size_t>(head_) % alignment)) {
        head_++;
      }
    }

    uint8_t* ret_val = head_;
    head_ += bytes;

    LOG(VERBOSE) << "boot_stack alloc ptr:" << ret_val << " size:" << bytes;
    return ret_val;
  }

  static inline const uint8_t* GetHead() { return head_; }

 private:
  static uint8_t* head_;
};

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_BOOT_STACK_H_
