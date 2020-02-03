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
#ifndef ARCH_ARM64_TIMER_H_
#define ARCH_ARM64_TIMER_H_

#include <cstdint>

#include "kernel/utils/register.h"

namespace arch {
namespace arm64 {

class Timer {
 public:
  struct Handler {
    virtual void HandleTimer() = 0;
  };

  Timer(Handler& handler);

  void Enable() { EnableCntv(); }

  void Tick();

 private:
  // Memory-Mapped I/O output
  static void MmIoWrite(intptr_t reg, uint32_t data) {
    *(volatile uint32_t*)reg = data;
  }

  // Memory-Mapped I/O input
  static uint32_t MmIoRead(intptr_t reg) { return *(volatile uint32_t*)reg; }

  uint32_t ReadCntFrq() {
    uint32_t val;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(val));
    return val;
  }

  uint32_t ReadCntvTval() {
    uint32_t val;
    asm volatile("mrs %0, cntv_tval_el0" : "=r"(val));
    return val;
  }

  void WriteCntvTval(uint32_t val) {
    asm volatile("msr cntv_tval_el0, %0" ::"r"(val));
  }

  uint32_t ReadCore0TimerPending() { return MmIoRead(CORE0_IRQ_SOURCE); }

  void RoutingCore0CntvToCore0Irq() { MmIoWrite(CORE0_TIMER_IRQCNTL, 0x08); }

  uint64_t ReadCntvCt(void) {
    uint64_t val;
    asm volatile("mrs %0, cntvct_el0" : "=r"(val));
    return val;
  }

  void EnableCntv() {
    uint32_t cntv_ctl = 1;
    asm volatile("msr cntv_ctl_el0, %0" ::"r"(cntv_ctl));
  }

  static constexpr auto CORE0_IRQ_SOURCE = 0x40000060;
  static constexpr auto CORE0_TIMER_IRQCNTL = 0x40000040;

  uint32_t cnt_frq_;
  Handler& handler_;
};

}  // namespace arm64
}  // namespace arch

#endif  // ARCH_ARM64_TIMER_H_
