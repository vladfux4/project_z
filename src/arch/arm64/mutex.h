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
#ifndef ARCH_ARM64_MUTEX_H_
#define ARCH_ARM64_MUTEX_H_

#include "kernel/hal/mutex_base.h"

namespace arch {
namespace arm64 {

/**
 * @brief The Routine class
 */
class Mutex : public kernel::hal::MutexBase {
 public:
  /**
   * @brief Constructor
   */
  Mutex();

  /**
   * @brief Destructor
   */
  virtual ~Mutex() override;

  /**
   * @brief Lock
   */
  virtual void Lock() override;

  /**
   * @brief Unlock
   */
  virtual void Unlock() override;
};

}  // namespace arm64
}  // namespace arch

#endif  // ARCH_ARM64_MUTEX_H_
