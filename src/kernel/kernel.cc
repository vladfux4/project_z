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

namespace kernel {

Kernel::Kernel() {
}

Kernel::~Kernel() {
}

}  // namespace kernel

extern "C" {
void KernelEntry() {
  while (true) {}
}
}

extern "C" {
//dummy operators for c++ support
void __cxa_pure_virtual(void) {
  // We might want to write some diagnostics to uart in this case
  while (true) {}
}
}

void operator delete(void*, unsigned long) {
}
