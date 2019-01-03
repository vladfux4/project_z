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
#include "kernel/mm/boot_allocator.h"

namespace kernel {
namespace mm {

BootAllocator::BootAllocator(uint8_t* start_ptr)
    : head_(start_ptr) {
}

BootAllocator::~BootAllocator() {
}

void* BootAllocator::Allocate(const size_t size) {
  uint8_t* ret_val = head_;
  head_ += size;
  return  ret_val;
}

void *BootAllocator::Allocate(const size_t size, const size_t aligned) {
  while (0 != (reinterpret_cast<size_t>(head_) % aligned)) { head_++; }
  uint8_t* ret_val = head_;
  head_ += size;
  return  ret_val;
}

void BootAllocator::Deallocate(void* ptr) { //delete is not supported
}

}  // namespace mm
}  // namespace kernel
