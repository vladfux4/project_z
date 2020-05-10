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
#include "kernel/scheduler/process.h"

#include <utility>

namespace kernel {
namespace scheduler {

extern "C" {
[[noreturn]] void ProcessBootstrap(Process* process) { process->Bootstrap(); }
}

Process::Process(mm::UniquePointer<mm::AddressSpace,
                                   mm::SlabAllocator>&& space,
                 const char* name, Function func, void* sp)
    : space_(std::move(space)), func_(func), name_(name) {
  constexpr auto spsr = Context::Spsr::MakeValue(
      Context::Spsr::M_LEVEL(arch::arm64::sys::ExceptionLevel::EL1_T),
      Context::Spsr::F(true), Context::Spsr::A(true), Context::Spsr::D(true));
  context_.spsr.Set(spsr);
  context_.registers = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  context_.registers.x0 = reinterpret_cast<uint64_t>(this);
  context_.elr = reinterpret_cast<void*>(ProcessBootstrap);
  context_.sp = sp;
  context_.translation_table = space_->translation_table.GetBase();

  LOG(DEBUG) << "SP: " << context_.sp;
  LOG(DEBUG) << "SPSR: " << context_.spsr.value;
  LOG(DEBUG) << "ELR: " << reinterpret_cast<void*>(context_.elr);
}

[[noreturn]] void Process::Bootstrap() {
  func_();
  while (true) {
  }
}

}  // namespace scheduler
}  // namespace kernel
