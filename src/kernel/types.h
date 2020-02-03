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
#ifndef KERNEL_TYPES_H_
#define KERNEL_TYPES_H_

#include <cstddef>
#include <cstdint>

namespace kernel {
namespace mm {

/**
 * @brief The Page size enum
 */
enum class PageSize {
  _4KB,
  _16KB,
  _64KB,
};

template <PageSize size>
struct PageSizeInfo {};

template <>
struct PageSizeInfo<PageSize::_4KB> {
  static constexpr size_t in_bytes = (1ULL << 12);
};

template <>
struct PageSizeInfo<PageSize::_16KB> {
  static constexpr size_t in_bytes = (1ULL << 14);
};

template <>
struct PageSizeInfo<PageSize::_64KB> {
  static constexpr size_t in_bytes = (1ULL << 16);
};

template <PageSize kPageSize>
struct Page {
  uint8_t data[PageSizeInfo<kPageSize>::in_bytes];
};

static_assert(sizeof(Page<kernel::mm::PageSize::_4KB>) ==
                  PageSizeInfo<kernel::mm::PageSize::_4KB>::in_bytes,
              "Wrong Page size");

static_assert(sizeof(Page<kernel::mm::PageSize::_16KB>) ==
                  PageSizeInfo<kernel::mm::PageSize::_16KB>::in_bytes,
              "Wrong Page size");

static_assert(sizeof(Page<kernel::mm::PageSize::_64KB>) ==
                  PageSizeInfo<kernel::mm::PageSize::_64KB>::in_bytes,
              "Wrong Page size");

}  // namespace mm
}  // namespace kernel

template <typename Type, typename Container>
decltype(Container().template Get<Type>()) Get(Container& container) {
  return container.template Get<Type>();
}

#endif  // KERNEL_TYPES_H_
