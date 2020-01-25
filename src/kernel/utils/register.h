#ifndef KERNEL_UTILS_REGISTER_H_
#define KERNEL_UTILS_REGISTER_H_

#include <cstdint>
#include <tuple>

namespace utils {
namespace rtr {

template <typename T>
constexpr std::size_t SizeInBits() {
  return sizeof(T) * 8;
}

template <typename T, std::size_t LENGTH = SizeInBits<T>()>
struct Field {
 public:
  using Type = T;
  constexpr static std::size_t length = LENGTH;

  static_assert(length <= SizeInBits<T>(), "Field type does not fit length");
};

template <template <std::size_t> class FieldAlias, std::size_t It,
          std::size_t Index>
struct Offset {
  constexpr static std::size_t Value(std::size_t offset = 0) {
    std::size_t result = offset;
    result +=
        Offset<FieldAlias, It + 1, Index - 1>::Value(FieldAlias<It>::length);
    return result;
  }
};

template <template <std::size_t> class FieldAlias, std::size_t It>
struct Offset<FieldAlias, It, 0> {
  constexpr static std::size_t Value(std::size_t offset = 0) { return offset; }
};

template <typename RawType, typename... Fields>
struct StaticRegister {
  using RegisterType = RawType;

  template <typename Field, std::size_t Index>
  struct FieldIndex {
    using Type = typename Field::Type;

    static constexpr std::size_t length = Field::length;
    static constexpr RawType mask = (static_cast<RawType>(1) << length) - 1;
    static constexpr std::size_t index = Index;

    constexpr FieldIndex(Type value) : value(value) {}
    constexpr FieldIndex() : value() {}

    Type value;
  };

  template <std::size_t... I>
  static auto MakeFieldIndexTuple(std::index_sequence<I...>) {
    return std::tuple<FieldIndex<Fields, I>...>{};
  }

  using FieldIndexTuple =
      decltype(MakeFieldIndexTuple(std::index_sequence_for<Fields...>{}));

  template <std::size_t Index>
  using FieldAlias = std::tuple_element_t<Index, FieldIndexTuple>;

  template <typename T>
  using is_in_register = std::is_same<FieldAlias<T::index>, T>;

  template <std::size_t Index>
  constexpr static std::size_t GetOffset() {
    return Offset<FieldAlias, 0, Index>::Value();
  }

  template <typename... Args>
  constexpr static RawType MakeValue(const Args... args) {
    return (DoMakeValue(args) | ...);
  }

  template <typename Arg>
  constexpr static RawType DoMakeValue(const Arg arg) {
    static_assert(is_in_register<Arg>::value,
                  "Input field parameter is not member of register");
    return static_cast<RawType>((static_cast<RawType>(arg.value) & arg.mask)
                                << GetOffset<Arg::index>());
  }

  template <typename Arg>
  constexpr static typename Arg::Type GetValue(const RawType raw) {
    static_assert(is_in_register<Arg>::value,
                  "Input field parameter is not member of register");
    return static_cast<typename Arg::Type>((raw >> GetOffset<Arg::index>()) &
                                           Arg::mask);
  }

  template <typename... Args>
  constexpr static RawType MakeMask(const Args... args) {
    return (DoMakeMask(args) | ...);
  }

  template <typename Arg>
  constexpr static RawType DoMakeMask(const Arg arg) {
    (void)arg;
    static_assert(is_in_register<Arg>::value,
                  "Input field parameter is not member of register");
    return Arg::mask << GetOffset<Arg::index>();
  }

  template <typename... Args>
  constexpr static RawType Set(const RawType raw, const Args... args) {
    auto result = raw;
    result &= ~MakeMask(args...);
    result |= MakeValue(args...);
    return result;
  }

  static_assert((Fields::length + ...) <= SizeInBits<RawType>(),
                "Fields length is out of base type size");
};

template <typename Register>
struct ValidateRegister {
  static_assert(sizeof(Register) ==
                    sizeof(typename Register::StaticInterface::RegisterType),
                "Register object is broken");
  static constexpr bool value = true;
};

template <typename DerivedType, typename RawType, typename... Fields>
struct Register : StaticRegister<RawType, Fields...> {
  using StaticInterface = StaticRegister<RawType, Fields...>;

  static DerivedType* Cast(RawType* address) {
    ValidateRegister<DerivedType>::value;
    return reinterpret_cast<DerivedType*>(address);
  }

  template <typename... Args>
  void Set(const Args... args) {
    value = StaticInterface::Set(value, args...);
  }

  template <class Arg>
  auto Get() {
    return StaticInterface::template GetValue<Arg>(value);
  }

  RawType value;
};

}  // namespace rtr
}  // namespace utils

#endif  // KERNEL_UTILS_REGISTER_H_
