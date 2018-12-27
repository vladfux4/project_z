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
  static constexpr uint64_t IPS_SHIFT = 32;
  static constexpr uint64_t IPS_MASK = (0b111ULL << IPS_SHIFT);
  enum class IPS : uint64_t {
    _32B = (0b000ULL << IPS_SHIFT),
    _36B = (0b001ULL << IPS_SHIFT),
    _40B = (0b010ULL << IPS_SHIFT),
  };

  static constexpr uint64_t TG1_SHIFT = 30;
  static constexpr uint64_t TG1_MASK = (0b11ULL << TG1_SHIFT);
  enum class TG1 : uint64_t {
    _4KB = (0b10ULL << TG1_SHIFT),
    _16KB = (0b01ULL << TG1_SHIFT),
    _64KB = (0b11ULL << TG1_SHIFT),
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
   * @brief Convert to raw value
   *
   * @param value
   *
   * @return raw value
   */
  template<class T>
  static __attribute__((always_inline)) constexpr uint64_t ToRaw(const T value);

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

  /**
   * @brief Set IPS
   *
   * @param ips
   */
  inline void Set(const IPS ips);

  /**
   * @brief Get TG1
   *
   * @return TG1
   */
  inline IPS GetIPS() const;

 private:
  uint64_t raw_;
};

constexpr TCR::TCR() : raw_(0) {
}

constexpr TCR::TCR(const uint64_t raw) : raw_(raw) {
}

template<class T>
__attribute__((always_inline)) constexpr uint64_t TCR::ToRaw(const T value) {
  return static_cast<uint64_t>(value);
}

inline void TCR::Set(const TCR::TG1 tg1) {
  raw_ &= ~TG1_MASK;
  raw_ |= ToRaw(tg1);
}

inline TCR::TG1 TCR::GetTG1() const {
  return static_cast<TG1>((TG1_MASK & raw_) >> TG1_SHIFT);
}

inline void TCR::Set(const TCR::IPS ips) {
  raw_ &= ~IPS_MASK;
  raw_ |= ToRaw(ips);
}

inline TCR::IPS TCR::GetIPS() const {
  return static_cast<IPS>((IPS_MASK & raw_) >> IPS_SHIFT);
}

}  // namespace arm64
}  // namespace arch

#endif  // ARCH_ARM64_TCR_H_
