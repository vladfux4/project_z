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
#ifndef KERNEL_SCHEDULER_ROUTINE_STATIC_WRAPPER_H_
#define KERNEL_SCHEDULER_ROUTINE_STATIC_WRAPPER_H_

#include <stdint.h>

namespace kernel {
namespace scheduler {

/**
 * @brief The Routine static wrapper
 */
template <class Routine>
class RoutineStaticWrapper {
 public:
 /**
   * @brief Get location of routine object
   *
   * @return pointer on object
   */
  inline static Routine* GetRoutineLocation();

  /**
   * @brief Execute
   */
  static void Exec();
};

template<class Routine>
inline Routine* RoutineStaticWrapper<Routine>::GetRoutineLocation() {
  static uint8_t object[sizeof(Routine)];
  return reinterpret_cast<Routine*>(object);
}

template<class Routine>
void RoutineStaticWrapper<Routine>::Exec() {
  GetRoutineLocation()->Exec();
}

}  // namespace scheduler
}  // namespace kernel

#endif  // KERNEL_SCHEDULER_ROUTINE_STATIC_WRAPPER_H_
