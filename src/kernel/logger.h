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
#ifndef KERNEL_LOGGER_H_
#define KERNEL_LOGGER_H_

#include <stdint.h>

#define DDEBUG

#ifdef DDEBUG
#define DDBG_LOG(fmt, ...) kernel::Print(fmt, ##__VA_ARGS__)
#else
#define DDBG_LOG(fmt)
#endif

namespace kernel {

void InitPrint();

void Print(const char* s);
void Print(const char* s, const uint64_t d);

}  // namespace kernel

#endif  // KERNEL_LOGGER_H_
