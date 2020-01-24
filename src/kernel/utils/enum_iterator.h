#ifndef KERNEL_UTILS_ENUM_ITERATOR_H_
#define KERNEL_UTILS_ENUM_ITERATOR_H_

#include <cstdint>
#include <type_traits>

namespace utils {

template <typename T,
          typename std::underlying_type<T>::type kEnd =
              static_cast<typename std::underlying_type<T>::type>(-1)>
class EnumIterator {
 public:
  using UnderlyingType = typename std::underlying_type<T>::type;

  constexpr EnumIterator(T value) : value_(value) {}

  const auto& Value() { return value_; }
  static const auto& End() { return end_; }

  const UnderlyingType& Int() {
    return *reinterpret_cast<UnderlyingType*>(&value_);
  }

  EnumIterator& operator++(int) {
    value_ = static_cast<T>(Int() + 1);
    return *this;
  }

  EnumIterator& operator--(int) {
    value_ = static_cast<T>(Int() - 1);
    return *this;
  }

  bool operator!=(const EnumIterator& obj) const {
    return (obj.value_ != value_);
  }

 private:
  static const EnumIterator end_;
  T value_;
};

template <typename T, typename std::underlying_type<T>::type kEnd>
const EnumIterator<T, kEnd> EnumIterator<T, kEnd>::end_ =
    EnumIterator(static_cast<T>(kEnd));

}  // namespace utils

#endif  // KERNEL_UTILS_ENUM_ITERATOR_H_
