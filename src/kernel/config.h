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
#ifndef KERNEL_CONFIG_H_
#define KERNEL_CONFIG_H_

#include <stdint.h>
#include <stdio.h>
#include "kernel/types.h"

namespace kernel {
namespace mm {

constexpr PageSize KERNEL_PAGE_SIZE = PageSize::_4KB;
constexpr uint8_t KERNEL_ADDRESS_LENGTH = 39;

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_CONFIG_H_
