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

#include <cstdint>

#include "arch/arm64/exceptions.h"
#include "kernel/kernel.h"
#include "kernel/logger.h"

extern "C" {

extern uint8_t exception_vectors;

uint64_t c_sync_handler() {
  return arch::arm64::Exceptions::StaticInterface::Value().HandleSync();
}

uint64_t c_irq_handler() {
  return arch::arm64::Exceptions::StaticInterface::Value().HandleIrq();
}

kernel::scheduler::Process::Context* kernel_get_current_context() {
  auto* process = kernel::Kernel::StaticScheduler::Value().CurrentProcess();
  return (process == nullptr) ? nullptr : process->GetContext();
}

kernel::scheduler::Process::Context* kernel_get_context_to_switch() {
  auto* process = kernel::Kernel::StaticScheduler::Value().ProcessToSwitch();
  return (process == nullptr) ? nullptr : process->GetContext();
}
}

namespace arch {
namespace arm64 {

Exceptions::Exceptions() {
  asm volatile("msr	vbar_el1, %0" ::"r"(&exception_vectors));

  StaticInterface::Make(*this);
}

void Exceptions::EnableIrq() { asm volatile("msr daifclr, #2"); }

void Exceptions::DisableIrq() { asm volatile("msr daifset, #2"); }

uint64_t Exceptions::HandleSync() {
  LOG(INFO) << "SVC";
  kernel::Kernel::StaticSupervisor::Value().Handle();

  auto& scheduler = kernel::Kernel::StaticScheduler::Value();
  if (scheduler.CurrentProcess() != scheduler.ProcessToSwitch()) {
    return KERNEL_EXCEPTION_HANDLE_SWITCH_CONTEXT;
  }

  return KERNEL_EXCEPTION_HANDLE_NONE;
}

uint64_t Exceptions::HandleIrq() {
  LOG(INFO) << "IRQ";
  kernel::Kernel::StaticSysTimer::Value().Tick();

  auto& scheduler = kernel::Kernel::StaticScheduler::Value();
  if (scheduler.CurrentProcess() != scheduler.ProcessToSwitch()) {
    return KERNEL_EXCEPTION_HANDLE_SWITCH_CONTEXT;
  }

  return KERNEL_EXCEPTION_HANDLE_NONE;
}

}  // namespace arm64
}  // namespace arch
