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

#include <stdint.h>

#include <cstddef>

#include "kernel/logger.h"
#include "kernel/mm/unique_ptr.h"

/// dummy operators for c++ support
extern "C" {
void __cxa_pure_virtual(void) {
  // We might want to write some diagnostics to uart in this case
  while (true) {
  }
}

void* memset(void* dst, int val, size_t size) {
  for (size_t i = 0; i < size; i++) {
    ((uint8_t*)dst)[i] = (uint8_t)val;
  }

  return dst;
}

__attribute__((__noreturn__)) void __assert_func(const char*, int, const char*,
                                                 const char*) {
  while (true) {
  }
}
}

void Function_1() {
  static uint64_t a = 0;
  a++;

  uint64_t* v_ptr =
      reinterpret_cast<uint64_t*>(0xFFFFFFFFFFE00050);  // mapped on 0x50
  *v_ptr = a;
}

void Function_2() {
  static uint64_t a = 0;
  a++;

  uint64_t* v_ptr =
      reinterpret_cast<uint64_t*>(0xFFFFFFFFFFF00060);  // mapped on 0x50
  *v_ptr = a;
}

namespace kernel {

static uint8_t __attribute__((aligned(4096))) kernel_storage[sizeof(Kernel)];

Kernel::Kernel() : memory_(), scheduler_(memory_) {}

void Kernel::Routine() {
  Init();
  Print("Init");

  using namespace arch::arm64::mm;

  auto process_1 = scheduler_.CreateProcess();
  process_1->space_->MapNewPage(reinterpret_cast<void*>(0xFFFFFFFFFFE00000));
  process_1->SetExec(Function_1);

  auto process_2 = scheduler_.CreateProcess();
  process_2->space_->MapNewPage(reinterpret_cast<void*>(0xFFFFFFFFFFF00000));
  process_2->SetExec(Function_2);

  // address translation test code
  while (true) {
    static uint64_t a = 0;
    a++;
    uint64_t* ptr = reinterpret_cast<uint64_t*>(0x40);
    *ptr = a;

    scheduler_.Select(*process_1);
    process_1->Exec();
    scheduler_.Select(*process_2);
    process_2->Exec();
  }
}

Kernel::~Kernel() {}

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
