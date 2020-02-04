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
#ifndef KERNEL_SCHEDULER_PROCESS_H_
#define KERNEL_SCHEDULER_PROCESS_H_

#include <cstddef>
#include <cstdint>

#include "gen/arch_types_gen.h"
#include "kernel/config.h"
#include "kernel/mm/memory.h"
#include "kernel/mm/unique_ptr.h"

namespace kernel {
namespace scheduler {

class Process {
 public:
  using Function = void (*)();

  struct Context {
    struct Registers {
      uint64_t x0;
      uint64_t x1;
      uint64_t x2;
      uint64_t x3;
      uint64_t x4;
      uint64_t x5;
      uint64_t x6;
      uint64_t x7;
      uint64_t x8;
      uint64_t x9;
      uint64_t x10;
      uint64_t x11;
      uint64_t x12;
      uint64_t x13;
      uint64_t x14;
      uint64_t x15;
      uint64_t x16;
      uint64_t x17;
      uint64_t x18;
      uint64_t x19;
      uint64_t x20;
      uint64_t x21;
      uint64_t x22;
      uint64_t x23;
      uint64_t x24;
      uint64_t x25;
      uint64_t x26;
      uint64_t x27;
      uint64_t x28;
      uint64_t x29;
      uint64_t x30;
    };

    void* translation_table;
    Function elr;
    uint64_t spsr;
    void* sp;
    Registers registers;
  };

  static_assert(sizeof(Context) == ((31 * 8) + (4 * 8)));

  static constexpr auto kStackStart = 0xFFFFFFFFFFF00000;

  Process(mm::UniquePointer<mm::Memory::VirtualAddressSpace,
                            mm::PhysicalAllocator>&& space,
          const char* name, Function func, void* sp)
      : space_(std::move(space)), func_(func), name_(name) {
    context_.elr = func;
    context_.spsr = 0x3C4;
    context_.registers = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    context_.sp = sp;
    context_.translation_table = space_->translation_table.GetBase();

    LOG(DEBUG) << "SP: " << context_.sp;
    LOG(DEBUG) << "SPSR: " << context_.spsr;
    LOG(DEBUG) << "ELR: " << reinterpret_cast<void*>(context_.elr);
  }

  void Exec() { func_(); }

  const char* Name() { return name_; }

  Context* GetContext() { return &context_; }

  mm::Memory::VirtualAddressSpace& AddressSpace() { return *space_; }

 private:
  mm::UniquePointer<mm::Memory::VirtualAddressSpace, mm::PhysicalAllocator>
      space_;

  Function func_;
  const char* name_;
public:
  Context context_;
};

}  // namespace scheduler
}  // namespace kernel

#endif  // KERNEL_SCHEDULER_PROCESS_H_
