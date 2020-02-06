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
#ifndef ARCH_ARM64_EXCEPTIONS_H_
#define ARCH_ARM64_EXCEPTIONS_H_

#include <cstdint>
#include "kernel/utils/static_wrapper.h"

#define KERNEL_EXCEPTION_HANDLE_NONE 0
#define KERNEL_EXCEPTION_HANDLE_SWITCH_CONTEXT 1

namespace arch {
namespace arm64 {

class Exceptions {
 public:
  using StaticInterface = utils::StaticWrapper<Exceptions>;

  Exceptions();

  uint64_t HandleSync();
  uint64_t HandleIrq();

  void EnableIrq();
  void DisableIrq();
};

}  // namespace arm64
}  // namespace arch

#endif  // ARCH_ARM64_EXCEPTIONS_H_
