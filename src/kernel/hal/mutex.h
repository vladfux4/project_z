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
#ifndef KERNEL_HAL_MUTEX_H_
#define KERNEL_HAL_MUTEX_H_

#include "gen/arch_types_gen.h"

namespace kernel {
namespace hal {

typedef arch::Mutex Mutex;

}  // namespace hal
}  // namespace kernel

#endif  // KERNEL_HAL_MUTEX_H_
