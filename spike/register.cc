#include <functional>
#include <iostream>

template<typename T, std::size_t LENGTH = sizeof (T) * 8>
struct Field {
 public:
  using Type = T;
  constexpr static std::size_t length = LENGTH;

  static_assert (length <= sizeof (T) * 8, "Field type does not fit length");
};

template<template <std::size_t> class FieldAlias, std::size_t It, std::size_t Index>
struct Offset {
  constexpr static std::size_t Value(std::size_t offset = 0) {
    std::size_t result = offset;
    result += Offset<FieldAlias, It + 1, Index - 1>::Value(FieldAlias<It>::length);
    return result;
  }
};

template< template <std::size_t> class FieldAlias, std::size_t It>
struct Offset<FieldAlias, It, 0> {
  constexpr static std::size_t Value(std::size_t offset = 0) {
    return offset;
  }
};

template<typename... Fields>
struct Register {
  using RawType = std::size_t;

  template<typename Field, std::size_t Index>
  struct FieldIndex {
    using Type = typename Field::Type;

    static constexpr std::size_t length = Field::length;
    static constexpr RawType mask = (static_cast<RawType>(1) << length) - 1;
    static constexpr std::size_t index = Index;

    Type value;
  };

  template<std::size_t... I>
  static auto MakeFieldIndexTuple(std::index_sequence<I...>) {
    return std::tuple<FieldIndex<Fields, I>...>{};
  }

  using FieldIndexTuple = decltype (MakeFieldIndexTuple(std::index_sequence_for<Fields...>{}));

  template<std::size_t Index>
  using FieldAlias = typename std::tuple_element_t<Index, FieldIndexTuple>;

  template<typename T>
  using is_in_register = std::is_same<FieldAlias<T::index>, T>;

  template<std::size_t Index>
  constexpr static std::size_t GetOffset() {
    return Offset<FieldAlias, 0, Index>::Value();
  }

  template<typename... Args>
  constexpr static RawType MakeValue(Args... args) {
    return DoMakeValue(args...);
  }

  template<typename Arg, typename... Rest>
  constexpr static RawType DoMakeValue(Arg arg, Rest... args) {
    auto value = DoMakeValue(arg);
    value |= DoMakeValue(args...);
    return value;
  }

  template<typename Arg>
  constexpr static RawType DoMakeValue(Arg arg) {
    static_assert (is_in_register<Arg>::value,
        "Input field parameter is not member of register");
    return (static_cast<RawType>(arg.value) & Arg::mask) << GetOffset<Arg::index>();
  }

  template<typename Arg>
  constexpr static typename Arg::Type GetValue(RawType raw) {
    static_assert (is_in_register<Arg>::value,
        "Input field parameter is not member of register");
    return static_cast<typename Arg::Type>((raw >> GetOffset<Arg::index>()) & Arg::mask);
  }

  template<typename... Args>
  constexpr static RawType MakeMask(Args... args) {
    return DoMakeMask(args...);
  }

  template<typename Arg, typename... Rest>
  constexpr static RawType DoMakeMask(Arg arg, Rest... args) {
    auto mask = DoMakeMask(arg);
    mask |= DoMakeMask(args...);
    return mask;
  }

  template<typename Arg>
  constexpr static RawType DoMakeMask(Arg arg) {
    (void)arg;
    static_assert (is_in_register<Arg>::value,
        "Input field parameter is not member of register");
    return Arg::mask << GetOffset<Arg::index>();
  }

  template<typename... Args>
  constexpr static RawType Set(RawType raw, Args... args) {
    auto result = raw;
    result &= ~MakeMask(args...);
    result |= MakeValue(args...);
    return result;
  }
};

struct Register_0 : Register<Field<uint8_t, 4>, Field<uint8_t>, Field<uint16_t, 16>> {
    using Field_1 = FieldAlias<0>;
    using Field_2 = FieldAlias<1>;
    using Field_3 = FieldAlias<2>;
};

struct Register_1 : Register<
                        Field<uint8_t, 4>,
                        Field<uint32_t>,
                        Field<uint8_t>,
                        Field<uint16_t>
                     > {
    using Field_1 = FieldAlias<0>;
    using Field_2 = FieldAlias<1>;
    using Field_3 = FieldAlias<2>;
    using Field_4 = FieldAlias<3>;
};

//struct Register_2 : public Register<Field<uint8_t, 9>> {}; // ERROR

int main() {
  Register_0 r0;

  {
    constexpr auto value = r0.MakeValue(Register_0::Field_1{0xAA}, Register_0::Field_2{0xBB});
    std::cout << "reg: " << std::hex << value << std::endl;

    std::cout << "f1: " << std::hex << static_cast<std::size_t>(r0.GetValue<Register_0::Field_1>(value)) << std::endl;
    std::cout << "f2: " << std::hex << static_cast<std::size_t>(r0.GetValue<Register_0::Field_2>(value)) << std::endl;
    std::cout << "f3: " << std::hex << static_cast<std::size_t>(r0.GetValue<Register_0::Field_3>(value)) << std::endl;
  }

  {
    constexpr auto value = r0.MakeValue(Register_0::Field_2{0xBB}, Register_0::Field_1{0xAA});
    std::cout << "reg: " << std::hex << value << std::endl;

    std::cout << "f1: " << std::hex << static_cast<std::size_t>(r0.GetValue<Register_0::Field_1>(value)) << std::endl;
    std::cout << "f2: " << std::hex << static_cast<std::size_t>(r0.GetValue<Register_0::Field_2>(value)) << std::endl;
    std::cout << "f3: " << std::hex << static_cast<std::size_t>(r0.GetValue<Register_0::Field_3>(value)) << std::endl;
  }

  {
    constexpr auto value = r0.MakeValue(Register_0::Field_2{0xBB}, Register_0::Field_1{0xAA}, Register_0::Field_3{0xCCDD});
    std::cout << "reg: " << std::hex << value << std::endl;

    std::cout << "f1: " << std::hex << static_cast<std::size_t>(r0.GetValue<Register_0::Field_1>(value)) << std::endl;
    std::cout << "f2: " << std::hex << static_cast<std::size_t>(r0.GetValue<Register_0::Field_2>(value)) << std::endl;
    std::cout << "f3: " << std::hex << static_cast<std::size_t>(r0.GetValue<Register_0::Field_3>(value)) << std::endl;
  }

  Register_1 r1;
  { // OK
    constexpr auto value = r1.MakeValue(Register_1::Field_2{0xAABBCCDD}, Register_1::Field_3{0xEE});
    std::cout << "reg: " << std::hex << value << std::endl;
    std::cout << "f2: " << std::hex << static_cast<std::size_t>(r1.GetValue<Register_1::Field_2>(value)) << std::endl;

    constexpr auto mask = r1.MakeMask(Register_1::Field_2{}, Register_1::Field_4{});
    std::cout << "mask: " << std::hex << mask << std::endl;

    constexpr auto set = r1.Set(value, Register_1::Field_2{0xAABB}, Register_1::Field_4{0xC});
    std::cout << "set: " << std::hex << set << std::endl;
  }

//  { // ERROR
//    constexpr auto value = r1.MakeValue(Register_0::Field_2{0xBB});
//    std::cout << "reg: " << std::hex << value << std::endl;
//    std::cout << "f2: " << std::hex << static_cast<std::size_t>(r1.GetValue<Register_0::Field_1>(value)) << std::endl;
//  }

  return 0;
}

