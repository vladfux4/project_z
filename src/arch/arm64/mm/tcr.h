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
#ifndef ARCH_ARM64_MM_TCR_H_
#define ARCH_ARM64_MM_TCR_H_

#include <stdint.h>

namespace arch {
namespace arm64 {
namespace mm {

/**
 * @brief The Translation Control Register class
 */
class TCR {
 public:
  static constexpr uint64_t IPS_SHIFT = 32;
  static constexpr uint64_t IPS_MASK = (0b111ULL << IPS_SHIFT);
  enum class IPS : uint64_t {
    _32_BIT = (0b000ULL << IPS_SHIFT),
    _36_BIT = (0b001ULL << IPS_SHIFT),
    _40_BIT = (0b010ULL << IPS_SHIFT),
  };

  static constexpr uint64_t TG1_SHIFT = 30;
  static constexpr uint64_t TG1_MASK = (0b11ULL << TG1_SHIFT);
  enum class TG1 : uint64_t {
    _4KB = (0b10ULL << TG1_SHIFT),
    _16KB = (0b01ULL << TG1_SHIFT),
    _64KB = (0b11ULL << TG1_SHIFT),
  };

  static constexpr uint64_t SH1_SHIFT = 28;
  static constexpr uint64_t SH1_MASK = (0b11ULL << SH1_SHIFT);
  enum class SH1 : uint64_t {
    NON_SHAREABLE = (0b00ULL << SH1_SHIFT),
    OUTER_SHAREABLE = (0b10ULL << SH1_SHIFT),
    INNER_SHAREABLE = (0b11ULL << SH1_SHIFT),
  };

  static constexpr uint64_t ORGN1_SHIFT = 26;
  static constexpr uint64_t ORGN1_MASK = (0b11ULL << ORGN1_SHIFT);
  enum class ORGN1 : uint64_t {
    NON_CACHEABLE = (0b00ULL << ORGN1_SHIFT),
    WRITE_BACK_CACHEABLE = (0b01ULL << ORGN1_SHIFT),
    WRITE_THRU_CACHEABLE = (0b10ULL << ORGN1_SHIFT),
    WRITE_BACK_NON_CACHEABLE = (0b11ULL << ORGN1_SHIFT),
  };

  static constexpr uint64_t IRGN1_SHIFT = 24;
  static constexpr uint64_t IRGN1_MASK = (0b11ULL << IRGN1_SHIFT);
  enum class IRGN1 : uint64_t {
    NON_CACHEABLE = (0b00ULL << IRGN1_SHIFT),
    WRITE_BACK_CACHEABLE = (0b01ULL << IRGN1_SHIFT),
    WRITE_THRU_CACHEABLE = (0b10ULL << IRGN1_SHIFT),
    WRITE_BACK_NON_CACHEABLE = (0b11ULL << IRGN1_SHIFT),
  };

  static constexpr uint64_t EPD1_SHIFT = 23;
  static constexpr uint64_t EPD1_MASK = (0b1ULL << EPD1_SHIFT);
  enum class EPD1 : uint64_t {
    WALK = (0b0ULL << EPD1_SHIFT),
    GENERATE_FAULT = (0b1ULL << EPD1_SHIFT),
  };

  static constexpr uint64_t T1SZ_SHIFT = 16;
  static constexpr uint64_t T1SZ_MASK = (0b111111ULL << T1SZ_SHIFT);

  static constexpr uint64_t TG0_SHIFT = 14;
  static constexpr uint64_t TG0_MASK = (0b11ULL << TG0_SHIFT);
  enum class TG0 : uint64_t {
    _4KB = (0b00ULL << TG0_SHIFT),
    _16KB = (0b10ULL << TG0_SHIFT),
    _64KB = (0b01ULL << TG0_SHIFT),
  };

  static constexpr uint64_t SH0_SHIFT = 12;
  static constexpr uint64_t SH0_MASK = (0b11ULL << SH0_SHIFT);
  enum class SH0 : uint64_t {
    NON_SHAREABLE = (0b00ULL << SH0_SHIFT),
    OUTER_SHAREABLE = (0b10ULL << SH0_SHIFT),
    INNER_SHAREABLE = (0b11ULL << SH0_SHIFT),
  };

  static constexpr uint64_t ORGN0_SHIFT = 10;
  static constexpr uint64_t ORGN0_MASK = (0b11ULL << ORGN0_SHIFT);
  enum class ORGN0 : uint64_t {
    NON_CACHEABLE = (0b00ULL << ORGN0_SHIFT),
    WRITE_BACK_CACHEABLE = (0b01ULL << ORGN0_SHIFT),
    WRITE_THRU_CACHEABLE = (0b10ULL << ORGN0_SHIFT),
    WRITE_BACK_NON_CACHEABLE = (0b11ULL << ORGN0_SHIFT),
  };

  static constexpr uint64_t IRGN0_SHIFT = 8;
  static constexpr uint64_t IRGN0_MASK = (0b11ULL << IRGN0_SHIFT);
  enum class IRGN0 : uint64_t {
    NON_CACHEABLE = (0b00ULL << IRGN0_SHIFT),
    WRITE_BACK_CACHEABLE = (0b01ULL << IRGN0_SHIFT),
    WRITE_THRU_CACHEABLE = (0b10ULL << IRGN0_SHIFT),
    WRITE_BACK_NON_CACHEABLE = (0b11ULL << IRGN0_SHIFT),
  };

  static constexpr uint64_t EPD0_SHIFT = 7;
  static constexpr uint64_t EPD0_MASK = (0b1ULL << EPD0_SHIFT);
  enum class EPD0 : uint64_t {
    WALK = (0b0ULL << EPD0_SHIFT),
    GENERATE_FAULT = (0b1ULL << EPD0_SHIFT),
  };

  static constexpr uint64_t T0SZ_SHIFT = 0;
  static constexpr uint64_t T0SZ_MASK = (0b111111ULL << T0SZ_SHIFT);

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
  static __attribute__((always_inline)) constexpr uint64_t ToT1SZRaw(const uint8_t val);
  static __attribute__((always_inline)) constexpr uint64_t ToT0SZRaw(const uint8_t val);

  inline void Set(const IPS ips);
  inline IPS GetIPS() const;

  inline void Set(const TG1 tg1);
  inline TG1 GetTG1() const;

  inline void Set(const SH1 sh1);
  inline SH1 GetSH1() const;

  inline void Set(const ORGN1 orgn1);
  inline ORGN1 GetORGN1() const;

  inline void Set(const IRGN1 irgn1);
  inline IRGN1 GetIRGN1() const;

  inline void Set(const EPD1 epd1);
  inline EPD1 GetEPD1() const;

  inline void SetT1SZ(const uint8_t val);
  inline uint8_t GetT1SZ() const;

  inline void Set(const TG0 tg0);
  inline TG0 GetTG0() const;

  inline void Set(const SH0 sh0);
  inline IPS GetSH0() const;

  inline void Set(const ORGN0 orgn0);
  inline ORGN0 GetORGN0() const;

  inline void Set(const IRGN0 irgn1);
  inline IRGN0 GetIRGN0() const;

  inline void Set(const EPD0 epd1);
  inline EPD0 GetEPD0() const;

  inline void SetT0SZ(const uint8_t val);
  inline uint8_t GetT0SZ() const;

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

__attribute__((always_inline)) constexpr uint64_t TCR::ToT1SZRaw(const uint8_t val) {
  return (static_cast<uint64_t>(64 - val) << T1SZ_SHIFT);
}

__attribute__((always_inline)) constexpr uint64_t TCR::ToT0SZRaw(const uint8_t val) {
  return (static_cast<uint64_t>(64 - val) << T0SZ_SHIFT);
}

inline void TCR::Set(const TCR::IPS ips) {
  raw_ &= ~IPS_MASK;
  raw_ |= ToRaw(ips);
}

inline TCR::IPS TCR::GetIPS() const {
  return static_cast<IPS>(IPS_MASK & raw_);
}

inline void TCR::Set(const TCR::TG1 tg1) {
  raw_ &= ~TG1_MASK;
  raw_ |= ToRaw(tg1);
}

inline TCR::TG1 TCR::GetTG1() const {
  return static_cast<TG1>(TG1_MASK & raw_);
}

inline void TCR::Set(const TCR::SH1 sh1) {
  raw_ &= ~SH1_MASK;
  raw_ |= ToRaw(sh1);
}

inline TCR::SH1 TCR::GetSH1() const {
  return static_cast<SH1>(SH1_MASK & raw_);
}

inline void TCR::Set(const TCR::ORGN1 orgn1) {
  raw_ &= ~ORGN1_MASK;
  raw_ |= ToRaw(orgn1);
}

inline TCR::ORGN1 TCR::GetORGN1() const {
  return static_cast<ORGN1>(ORGN1_MASK & raw_);
}

inline void TCR::Set(const TCR::IRGN1 irgn1) {
  raw_ &= ~IRGN1_MASK;
  raw_ |= ToRaw(irgn1);
}

inline TCR::IRGN1 TCR::GetIRGN1() const {
  return static_cast<IRGN1>(IRGN1_MASK & raw_);
}

inline void TCR::Set(const TCR::EPD1 edp1) {
  raw_ &= ~EPD1_MASK;
  raw_ |= ToRaw(edp1);
}

inline TCR::EPD1 TCR::GetEPD1() const {
  return static_cast<EPD1>(EPD1_MASK & raw_);
}

inline void TCR::SetT1SZ(const uint8_t val) {
  raw_ &= ~T1SZ_MASK;
  raw_ |= ToT1SZRaw(val);
}

inline uint8_t TCR::GetT1SZ() const {
  return (64 - ((T1SZ_MASK & raw_) >> T1SZ_SHIFT));
}

inline void TCR::Set(const TCR::TG0 tg0) {
  raw_ &= ~TG0_MASK;
  raw_ |= ToRaw(tg0);
}

inline TCR::TG0 TCR::GetTG0() const {
  return static_cast<TG0>(TG0_MASK & raw_);
}

inline void TCR::Set(const TCR::SH0 sh0) {
  raw_ &= ~SH0_MASK;
  raw_ |= ToRaw(sh0);
}

inline TCR::IPS TCR::GetSH0() const {
  return static_cast<IPS>(SH0_MASK & raw_);
}

inline void TCR::Set(const TCR::ORGN0 orgn0) {
  raw_ &= ~ORGN0_MASK;
  raw_ |= ToRaw(orgn0);
}

inline TCR::ORGN0 TCR::GetORGN0() const {
  return static_cast<ORGN0>(ORGN0_MASK & raw_);
}

inline void TCR::Set(const TCR::IRGN0 irgn0) {
  raw_ &= ~IRGN0_MASK;
  raw_ |= ToRaw(irgn0);
}

inline TCR::IRGN0 TCR::GetIRGN0() const {
  return static_cast<IRGN0>(IRGN0_MASK & raw_);
}

inline void TCR::Set(const TCR::EPD0 edp0) {
  raw_ &= ~EPD0_MASK;
  raw_ |= ToRaw(edp0);
}

inline TCR::EPD0 TCR::GetEPD0() const {
  return static_cast<EPD0>(EPD0_MASK & raw_);
}

inline void TCR::SetT0SZ(const uint8_t val) {
  raw_ &= ~T0SZ_MASK;
  raw_ |= ToT0SZRaw(val);
}

inline uint8_t TCR::GetT0SZ() const {
  return (64 - ((T0SZ_MASK & raw_) >> T0SZ_SHIFT));
}

}  // namespace mm
}  // namespace arm64
}  // namespace arch

#endif  // ARCH_ARM64_MM_TCR_H_
