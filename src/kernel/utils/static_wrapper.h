#ifndef KERNEL_UTILS_STATIC_WRAPPER_H_
#define KERNEL_UTILS_STATIC_WRAPPER_H_

namespace utils {

template <typename Type>
class StaticWrapper {
 public:
  using Pointer = Type*;

  StaticWrapper() = delete;
  StaticWrapper(const StaticWrapper&) = delete;
  StaticWrapper(StaticWrapper&&) = delete;
  StaticWrapper& operator=(const StaticWrapper&) = delete;

  static void Init(Type& obj) {
    Pointer& storage = Storage();
    if (storage == nullptr) {
      storage = &obj;
    }
  }

  static Pointer Get() { return Storage(); }

 private:
  static Pointer& Storage() {
    static Pointer ptr = nullptr;
    return ptr;
  }
};

}  // namespace utils

#endif  // KERNEL_UTILS_STATIC_WRAPPER_H_
