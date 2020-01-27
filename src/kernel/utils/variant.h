#ifndef KERNEL_UTILS_VARIANT_H_
#define KERNEL_UTILS_VARIANT_H_

#include <algorithm>
#include <cstdint>
#include <type_traits>

namespace utils {

template <typename DefaultType, typename... Args>
struct Variant {
  static constexpr auto kMaxSize = std::max({sizeof(Args)...});

  template <typename T>
  using is_in_variant = std::disjunction<std::is_same<T, Args>...>;

  Variant() {
    static_assert(is_in_variant<DefaultType>::value);
    new (reinterpret_cast<DefaultType*>(data)) DefaultType();
  }

  template <typename T>
  T& Get() {
    static_assert(is_in_variant<T>::value);
    return *reinterpret_cast<T*>(data);
  }

  template <typename T>
  Variant& operator=(const T& item) {
    static_assert(is_in_variant<T>::value);
    *reinterpret_cast<T*>(data) = item;
    return *this;
  }

  uint8_t data[kMaxSize];
};

}  // namespace utils

#endif  // KERNEL_UTILS_VARIANT_H_
