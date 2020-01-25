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
#ifndef ARCH_ARM64_MM_MMU_H_
#define ARCH_ARM64_MM_MMU_H_

#include "arch/arm64/mm/tcr.h"

namespace arch {
namespace arm64 {
namespace mm {

/**
 * @brief The Memory Management Unit class
 */
class MMU {
 public:
  /**
   * @brief Constructor
   */
  MMU();

  /**
   * @brief Enable MMU
   */
  void Enable();

  /**
   * @brief Set user space translation table
   *
   * @param address Table address
   */
  inline void SetUserTable(void* address);

  /**
   * @brief Set kernel space translation table
   *
   * @param address Table address
   */
  inline void SetKernelTable(void* address);

 private:
  /**
   * @brief Set 0 translation table address
   */
  __attribute__((always_inline)) void SetTTBR0(void* address) {
    asm volatile("msr ttbr0_el1, %0" : : "r"(address));
  }

  /**
   * @brief Set 1 translation table address
   */
  __attribute__((always_inline)) void SetTTBR1(void* address) {
    asm volatile("msr ttbr1_el1, %0" : : "r"(address));
  }

  tcr::TcrRegister tcr_;
};

inline void MMU::SetUserTable(void* address) { SetTTBR0(address); }

inline void MMU::SetKernelTable(void* address) { SetTTBR1(address); }

}  // namespace mm
}  // namespace arm64
}  // namespace arch

#endif  // ARCH_ARM64_MM_MMU_H_
