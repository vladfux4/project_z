#ifndef KERNEL_UTILS_STATIC_WRAPPER_H_
#define KERNEL_UTILS_STATIC_WRAPPER_H_

#include <cassert>

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
    Pointer& storage = Storage();
    assert(storage == nullptr);
    storage = &obj;
  }

  static Type& Value() {
    Pointer ret_val = Storage();
    assert(ret_val != nullptr);
    return *ret_val;
  }

 private:
  static Pointer& Storage() {
    static Pointer ptr = nullptr;
    return ptr;
  }
};

}  // namespace utils

#endif  // KERNEL_UTILS_STATIC_WRAPPER_H_
