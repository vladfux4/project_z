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
#ifndef ARCH_ARM64_TCR_H_
#define ARCH_ARM64_TCR_H_

#include <stdint.h>

namespace arch {
namespace arm64 {

/**
 * @brief The Translation Control Register class
 */
class TCR {
 public:
  enum class TG1 {
    _4KB,
    _16KB,
    _64KB,
  };

  /**
   * @brief Constructor
   */
  constexpr TCR();

  /**
   * @brief Constructor
   *
   * @param raw RAW register value
   */
  constexpr TCR(const uint64_t raw);

  /**
   * @brief Convert TG1 to raw value
   *
   * @param tg1
   *
   * @return value
   */
  static inline constexpr uint64_t ToRaw(const TG1 tg1);

  /**
   * @brief Set TG1
   *
   * @param tg1
   */
  inline void Set(const TG1 tg1);

  /**
   * @brief Get TG1
   *
   * @return TG1
   */
  inline TG1 GetTG1() const;

 private:
  static constexpr uint64_t TG1_SHIFT = 37;
  static constexpr uint64_t TG1_MASK = (0b11ULL << TG1_SHIFT);
  static constexpr uint64_t TG1_VALUE_4KB = (0b10ULL);
  static constexpr uint64_t TG1_VALUE_16KB = (0b01ULL);
  static constexpr uint64_t TG1_VALUE_64KB = (0b11ULL);

  uint64_t raw_;
};

constexpr TCR::TCR() : raw_() {
}

constexpr TCR::TCR(const uint64_t raw) : raw_(raw) {
}

inline constexpr uint64_t TCR::ToRaw(const TCR::TG1 tg1) {
  switch (tg1) {
    case TG1::_4KB: {
      return (TG1_VALUE_4KB << TG1_SHIFT);
    }
    case TG1::_16KB: {
      return (TG1_VALUE_16KB << TG1_SHIFT);
    }
    case TG1::_64KB: {
      return (TG1_VALUE_64KB << TG1_SHIFT);
    }
  }

  return UINT64_MAX;
}

inline void TCR::Set(const TCR::TG1 tg1) {
  raw_ &= ~TG1_MASK;
  raw_ |= ToRaw(tg1);
}

inline TCR::TG1 TCR::GetTG1() const {
  uint32_t value = ((TG1_MASK & raw_) >> TG1_SHIFT);

  switch (value) {
    case TG1_VALUE_4KB: {
      return TG1::_4KB;
    }
    case TG1_VALUE_16KB: {
      return TG1::_16KB;
    }
    case TG1_VALUE_64KB: {
      return TG1::_64KB;
    }
    default: {
      //assert();
    }
  }

  return TG1::_4KB;
}

}  // namespace arm64
}  // namespace arch

#endif  // ARCH_ARM64_TCR_H_
