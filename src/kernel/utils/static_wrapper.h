#ifndef KERNEL_UTILS_STATIC_WRAPPER_H_
#define KERNEL_UTILS_STATIC_WRAPPER_H_

#include <cassert>
#include <utility>

namespace utils {

template <typename Type>
class StaticWrapper {
 public:
  using Pointer = Type*;

  StaticWrapper() = delete;
  StaticWrapper(const StaticWrapper&) = delete;
  StaticWrapper(StaticWrapper&&) = delete;
  StaticWrapper& operator=(const StaticWrapper&) = delete;

  static void Make(Type& obj) {
    assert(ptr_ == nullptr);
    ptr_ = &obj;
  }

  static Type& Value() {
    assert(ptr_ != nullptr);
    return *ptr_;
  }

 private:
  static Pointer ptr_;
};

template <typename Type>
typename StaticWrapper<Type>::Pointer StaticWrapper<Type>::ptr_ = nullptr;

}  // namespace utils

#endif  // KERNEL_UTILS_STATIC_WRAPPER_H_
