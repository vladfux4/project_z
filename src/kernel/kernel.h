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
#ifndef KERNEL_KERNEL_H_
#define KERNEL_KERNEL_H_

#include "arch/arm64/timer.h"
#include "kernel/mm/memory.h"
#include "kernel/scheduler/scheduler.h"
#include "kernel/utils/static_wrapper.h"

namespace kernel {

class Supervisor {
 public:
  class Handler {
   public:
    virtual void HandleSvc() = 0;
  };

  Supervisor(Handler& handler) : handler_(handler) {}

  void Handle() { handler_.HandleSvc(); }

 private:
  Handler& handler_;
};

/**
 * @brief The Routine class
 */
class Kernel : public arch::arm64::Timer::Handler, public Supervisor::Handler {
 public:
  using StaticScheduler = utils::StaticWrapper<scheduler::Scheduler>;
  using StaticSysTimer = utils::StaticWrapper<arch::arm64::Timer>;
  using StaticSuperviser = utils::StaticWrapper<Supervisor>;

  /**
   * @brief Constructor
   */
  Kernel();

  /**
   * @brief Run
   */
  void Routine();

  void HandleTimer() override;
  void HandleSvc() override;

  /**
   * @brief Destructor
   */
  ~Kernel();

 private:
  /**
   * @brief Init kernel
   */
  void Init();

  mm::Memory memory_;
  scheduler::Scheduler scheduler_;
  arch::arm64::Timer sys_timer_;
  Supervisor superviser_;
};

}  // namespace kernel

#endif  // KERNEL_KERNEL_H_
