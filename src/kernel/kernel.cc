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
#include "kernel/kernel.h"
#include "kernel/logger.h"

#include <stdint.h>
#include <cstddef>

/// dummy operators for c++ support
extern "C" {
void __cxa_pure_virtual(void) {
  // We might want to write some diagnostics to uart in this case
  while (true) {}
}

void* memset(void* dst, int val, size_t size) {
  for (size_t i = 0; i < size; i++) {
    ((uint8_t*)dst)[i] = (uint8_t)val;
  }

  return dst;
}
}

void operator delete(void*, unsigned long) {}
//void operator delete(void*, unsigned long, std::align_val_t) {}

// Default placement versions of operator new.
inline void* operator new (size_t, void* __p) { return __p; }
inline void* operator new[] (size_t, void* __p) { return __p; }

// Default placement versions of operator delete.
inline void operator delete (void*, void*) {}
inline void operator delete[] (void*, void*) {}
/// dummy operators end

namespace kernel {

static uint8_t __attribute__((aligned(4096)))
kernel_storage[sizeof(Kernel)];

Kernel::Kernel() : memory_() {
}

void Kernel::Routine() {
  Init();
  Print("Init");

  // address translation test code
  uint64_t* ptr = reinterpret_cast<uint64_t*>(0x40);
  uint64_t* v_ptr = reinterpret_cast<uint64_t*>(0xFFFFFFFFFFE00050); // mapped on 0x50
  while (true) {
    static uint64_t a = 0;
    a++;
    *ptr = a;
    *v_ptr = a;
    Print("Test");
  }
}

Kernel::~Kernel() {
}

void Kernel::Init() {
  InitPrint();
  memory_.Init();
}

extern "C" {
void KernelEntry() {
  auto kernel = new (reinterpret_cast<Kernel*>(kernel_storage)) Kernel();
  kernel->Routine();
}
}

}  // namespace kernel
