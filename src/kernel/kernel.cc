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

extern uint8_t vectors;
extern void enable_irq(void);
extern void disable_irq(void);
}

void Function_1() {
  enable_irq();
  static uint64_t a = 0;

  while (true) {
    uint64_t* v_ptr =
        reinterpret_cast<uint64_t*>(0xFFFFFFFFFFE00050);  // mapped on 0x50
    *v_ptr = a;

    LOG(INFO) << "Process_1 value: " << a;
    for (size_t i = 0; i < 0x11FFFFFF; i++) {
      a++;
    }
  }
}

void Function_2() {
  enable_irq();

  while (true) {
    static uint64_t a = 0;

    uint64_t* v_ptr =
        reinterpret_cast<uint64_t*>(0xFFFFFFFFEEE00050);  // mapped on 0x50
    *v_ptr = a;

    uint64_t* ptr = reinterpret_cast<uint64_t*>(0x40);
    *ptr = a;

    LOG(INFO) << "Process_2 value: " << a;
    for (size_t i = 0; i < 0x11FFFFFF; i++) {
      a++;
    }
  }
}

namespace kernel {

static uint8_t __attribute__((aligned(4096))) kernel_storage[sizeof(Kernel)];

Kernel::Kernel()
    : memory_(), scheduler_(memory_), sys_timer_(*this), superviser_(*this) {
  StaticScheduler::Init(scheduler_);
  StaticSysTimer::Init(sys_timer_);
  StaticSuperviser::Init(superviser_);
}

Kernel::~Kernel() {}

void Kernel::Routine() {
  Init();

  LOG(INFO) << "Hello";
  LOG(INFO) << "Free pages: "
            << static_cast<size_t>(
                   kernel::mm::PhysicalPagePool::Get()->FreeItems());
  {
    auto process_1 = scheduler_.CreateProcess("Process_1", Function_1);
    process_1->AddressSpace().MapNewPage(
        reinterpret_cast<void*>(0xFFFFFFFFFFE00000));

    auto process_2 = scheduler_.CreateProcess("Process_2", Function_2);
    process_2->AddressSpace().MapNewPage(
        reinterpret_cast<void*>(0xFFFFFFFFEEE00050));

    scheduler_.enabled = true;
    sys_timer_.Enable();
    enable_irq();

    asm("svc #0");

    // address translation test code
    while (true) {
      static uint64_t a = 0;
      a++;
      uint64_t* ptr = reinterpret_cast<uint64_t*>(0x40);
      *ptr = a;

      LOG(INFO) << "Kernel delay start";
      for (size_t i = 0; i < 0x1FFFFFFF; i++) {
      }
      LOG(INFO) << "Kernel delay end";
    }
  }

  LOG(INFO) << "Free pages: "
            << static_cast<size_t>(
                   kernel::mm::PhysicalPagePool::Get()->FreeItems());
}

void Kernel::HandleTimer() {
  enable_irq();
  scheduler_.Tick();
  disable_irq();
}

void Kernel::HandleSvc() { scheduler_.Tick(); }

void Kernel::Init() {
  asm volatile("msr	vbar_el1, %0" : : "r"(&vectors));
  log::InitPrint();
  memory_.Init();
}

extern "C" {

kernel::scheduler::Process::Context* kernel_context_current() {
  auto* process = Kernel::StaticScheduler::Get()->CurrentProcess();
  return (process == nullptr) ? nullptr : process->GetContext();
}

kernel::scheduler::Process::Context* kernel_context_to_switch() {
  auto* process = Kernel::StaticScheduler::Get()->ProcessToSwitch();
  return (process == nullptr) ? nullptr : process->GetContext();
}

bool c_sync_handler() {
  LOG(INFO) << "SVC";
  Kernel::StaticSuperviser::Get()->Handle();

  auto scheduler = Kernel::StaticScheduler::Get();
  auto current = scheduler->CurrentProcess();
  auto next = scheduler->ProcessToSwitch();
  LOG(INFO) << "Switch: " << ((current) ? current->Name() : "Null") << " -> "
            << ((next) ? next->Name() : "Null");

  bool switch_context = (current != next);
  return switch_context;
}

bool c_irq_handler() {
  Kernel::StaticSysTimer::Get()->Tick();

  auto scheduler = Kernel::StaticScheduler::Get();
  auto current = scheduler->CurrentProcess();
  auto next = scheduler->ProcessToSwitch();
  LOG(INFO) << "Switch: " << ((current) ? current->Name() : "Null") << " -> "
            << ((next) ? next->Name() : "Null");

  bool switch_context = (current != next);
  return switch_context;
}

void KernelEntry() {
  auto kernel = new (reinterpret_cast<Kernel*>(kernel_storage)) Kernel();
  kernel->Routine();
}
}

}  // namespace kernel
