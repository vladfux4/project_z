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

  Process(mm::UniquePointer<mm::Memory::VirtualAddressSpace,
                            mm::PhysicalAllocator>&& space)
      : space_(std::move(space)), func_(nullptr) {}

  void Exec() {
    if (func_ != nullptr) {
      func_();
    }
  }

  void SetExec(Function func) { func_ = func; }

  mm::Memory::VirtualAddressSpace& AddressSpace() { return *space_; }

 private:
  mm::UniquePointer<mm::Memory::VirtualAddressSpace, mm::PhysicalAllocator>
      space_;

  Function func_;
};

}  // namespace scheduler
}  // namespace kernel

#endif  // KERNEL_SCHEDULER_PROCESS_H_
