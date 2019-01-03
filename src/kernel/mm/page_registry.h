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
#ifndef KERNEL_MM_PAGE_MAP_
#define KERNEL_MM_PAGE_MAP_

#include <stdint.h>
#include <cstddef>

#include "kernel/mm/allocator.h"
#include "kernel/config.h"

#include "kernel/mm/pool.h"

namespace kernel {
namespace mm {

struct Page {
  uint8_t data;
};

/**
 * @brief The Page registry class
 */
class PageRegistry {
 public:
  PageRegistry(const uint8_t* begin, const uint8_t* end,
               Allocator& alloc);

 private:
  typedef Pool<Page, size_t> PagePool;

  PagePool* page_pool_;
};

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_PAGE_MAP_
