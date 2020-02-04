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
#include "arch/arm64/timer.h"

#include "kernel/logger.h"


extern "C" {
extern void enable_irq(void);
extern void disable_irq(void);
}

namespace arch {
namespace arm64 {

Timer::Timer(Handler& handler) : cnt_frq_(0), handler_(handler) {
  cnt_frq_ = ReadCntFrq();
  WriteCntvTval(cnt_frq_);  // clear cntv interrupt and set next 1 sec timer.
  RoutingCore0CntvToCore0Irq();

  LOG(VERBOSE) << "CNTFRQ  : " << cnt_frq_;
  LOG(VERBOSE) << "CNTV_TVAL  : " << ReadCntvTval();
}

void Timer::Tick() {
//  disable_irq();
  if (ReadCore0TimerPending() & 0x08) {
    WriteCntvTval(cnt_frq_);  // clear cntv interrupt and set next 1sec timer.
    
    LOG(VERBOSE) << "handler CNTV_TVAL: " << ReadCntvTval();
    LOG(VERBOSE) << "handler CNTVCT: " << ReadCntvCt();
    handler_.HandleTimer();
  }

//  enable_irq();
}

// namespace sys
}  // namespace arm64
}  // namespace arch
