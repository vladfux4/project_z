#include <functional>
#include <iostream>

template<typename T, std::size_t LENGTH = sizeof (T) * 8>
struct Field {
 public:
  using Type = T;
  static constexpr std::size_t length = LENGTH;
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

  template<typename Field, std::size_t Index>
  struct FieldIndex {
    using Type = typename Field::Type;
    static constexpr std::size_t length = Field::length;
    static constexpr std::size_t mask = (static_cast<std::size_t>(1) << length) - 1;
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

  template<std::size_t Index>
  constexpr static std::size_t GetOffset() {
    return Offset<FieldAlias, 0, Index>::Value();
  }

  template<typename... Args>
  constexpr std::size_t MakeValue(Args... args) {
    return DoMakeValue(args...);
  }

  template<typename First, typename... Rest>
  constexpr std::size_t DoMakeValue(First arg, Rest... args) {
    std::size_t value = DoMakeValue(arg);
    value |= DoMakeValue(args...);
    return value;
  }

  template<typename First>
  constexpr std::size_t DoMakeValue(First arg) {
    return (static_cast<std::size_t>(arg.value) & First::mask) << GetOffset<First::index>();
  }
  
  template<typename First>
  constexpr auto GetValue(std::size_t raw) {
    return static_cast<typename First::Type>((raw >> GetOffset<First::index>()) & First::mask);
  }
};

struct Register_0 : public Register<Field<uint8_t, 4>, Field<uint8_t>, Field<uint16_t, 16>> {
    using Field_1 = FieldAlias<0>;
    using Field_2 = FieldAlias<1>;
    using Field_3 = FieldAlias<2>;
};

int main() {
  std::cout << "Hello World!" << std::endl;

  Register_0 r;

  {
    constexpr auto value = r.MakeValue(Register_0::Field_1{0xAA}, Register_0::Field_2{0xBB});
    std::cout << "reg: " << std::hex << value << std::endl;

    std::cout << "f1: " << std::hex << static_cast<std::size_t>(r.GetValue<Register_0::Field_1>(value)) << std::endl;
    std::cout << "f2: " << std::hex << static_cast<std::size_t>(r.GetValue<Register_0::Field_2>(value)) << std::endl;
    std::cout << "f3: " << std::hex << static_cast<std::size_t>(r.GetValue<Register_0::Field_3>(value)) << std::endl;
  }

  {
    constexpr auto value = r.MakeValue(Register_0::Field_2{0xBB}, Register_0::Field_1{0xAA});
    std::cout << "reg: " << std::hex << value << std::endl;
    
    std::cout << "f1: " << std::hex << static_cast<std::size_t>(r.GetValue<Register_0::Field_1>(value)) << std::endl;
    std::cout << "f2: " << std::hex << static_cast<std::size_t>(r.GetValue<Register_0::Field_2>(value)) << std::endl;
    std::cout << "f3: " << std::hex << static_cast<std::size_t>(r.GetValue<Register_0::Field_3>(value)) << std::endl;
  }

  {
    constexpr auto value = r.MakeValue(Register_0::Field_2{0xBB}, Register_0::Field_1{0xAA}, Register_0::Field_3{0xCCDD});
    std::cout << "reg: " << std::hex << value << std::endl;
    
    std::cout << "f1: " << std::hex << static_cast<std::size_t>(r.GetValue<Register_0::Field_1>(value)) << std::endl;
    std::cout << "f2: " << std::hex << static_cast<std::size_t>(r.GetValue<Register_0::Field_2>(value)) << std::endl;
    std::cout << "f3: " << std::hex << static_cast<std::size_t>(r.GetValue<Register_0::Field_3>(value)) << std::endl;
  }

  return 0;
}
