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
#ifndef KERNEL_SCHEDULER_SCHEDULER_H_
#define KERNEL_SCHEDULER_SCHEDULER_H_

#include <cstddef>
#include <cstdint>

#include "gen/arch_types_gen.h"
#include "kernel/config.h"
#include "kernel/mm/memory.h"
#include "kernel/mm/unique_ptr.h"
#include "kernel/scheduler/process.h"

namespace kernel {
namespace scheduler {

class Scheduler {
 public:
  Scheduler(mm::Memory& memory) : memory_(memory), process_count_(0) {}

//  mm::UniquePointer<Process, mm::SlabAllocator> CreateProcess(
//      const char* name, Process::Function func) {
//    auto space = memory_.CreateVirtualAddressSpace();

//    auto stack_page = space->MapNewPage(reinterpret_cast<void*>(
//        Process::kStackStart -
//        mm::PageSizeInfo<mm::KERNEL_PAGE_SIZE>::in_bytes));
//    LOG(DEBUG) << "Stack page allocation at: " << stack_page;
//    auto stack_ptr = reinterpret_cast<void*>(Process::kStackStart);

//    auto process = mm::UniquePointer<Process, mm::SlabAllocator>::Make(
//        std::move(space), name, func, stack_ptr);

//    processes_[process_count_] = process.Get();
//    process_count_++;

//    return process;
//  }

  void Tick() {
    if (enabled) {
      LOG(INFO) << "Scheduler Tick";
      enabled = false;
      DoTick();
      enabled = true;
    } else {
      LOG(INFO) << "Ignore Scheduler Tick";
    }
  }

  void DoTick() {
    // Here should be actual shceduling algo
    static size_t tick = 0;
    static size_t current_process = 0;
    current_process = (current_process + 1) % process_count_;

    if ((tick % 3) == 0) {
      current_process = 0;
    }
    tick++;

    Select(*processes_[current_process]);
  }

  void Select(Process& process) {
    current_process_ = next_process_;
    next_process_ = &process;

    LOG(INFO) << "Switch: "
              << ((current_process_) ? current_process_->Name() : "Null")
              << " ->" << ((next_process_) ? next_process_->Name() : "Null");
  }

  Process* CurrentProcess() { return current_process_; }
  Process* ProcessToSwitch() { return next_process_; }

  mm::Memory& memory_;
  Process* processes_[10];
  size_t process_count_;
  bool enabled = false;

  Process* current_process_ = nullptr;
  Process* next_process_ = nullptr;
};

}  // namespace scheduler
}  // namespace kernel

#endif  // KERNEL_SCHEDULER_SCHEDULER_H_
