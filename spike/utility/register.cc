#include <iostream>

#include "register.h"

struct Register_0 : rtr::StaticRegister<std::size_t, rtr::Field<uint8_t, 4>, rtr::Field<uint8_t>, rtr::Field<uint16_t, 16>> {
    using Field_1 = FieldAlias<0>;
    using Field_2 = FieldAlias<1>;
    using Field_3 = FieldAlias<2>;
};

struct Register_1 : rtr::StaticRegister<std::size_t,
                        rtr::Field<uint8_t, 4>,
                        rtr::Field<uint32_t>,
                        rtr::Field<uint8_t>,
                        rtr::Field<uint16_t>
                     > {
    using Field_1 = FieldAlias<0>;
    using Field_2 = FieldAlias<1>;
    using Field_3 = FieldAlias<2>;
    using Field_4 = FieldAlias<3>;
};

//struct Register_2 : public rtr::StaticRegister<Field<uint8_t, 9>> {}; // ERROR

enum class Enum_0 {
  OFF = 0,
  ON = 1,
  Z = 2,
};

struct Register_3 : public rtr::StaticRegister<uint8_t, rtr::Field<Enum_0, 4>, rtr::Field<uint8_t, 4>> {
    using Field_1 = FieldAlias<0>;
    using Field_2 = FieldAlias<1>;
};

struct Register_4 : rtr::Register<Register_4, uint8_t,
        rtr::Field<Enum_0, 4>, rtr::Field<uint8_t, 4>> {
    using Field_1 = FieldAlias<0>;
    using Field_2 = FieldAlias<1>;
};
static_assert (rtr::ValidateRegister<Register_4>::value, "");

int main() {
  {
    constexpr auto value = Register_0::MakeValue(Register_0::Field_1{0xAA}, Register_0::Field_2{0xBB});
    std::cout << "reg: " << std::hex << value << std::endl;

    std::cout << "f1: " << std::hex << static_cast<std::size_t>(Register_0::GetValue<Register_0::Field_1>(value)) << std::endl;
    std::cout << "f2: " << std::hex << static_cast<std::size_t>(Register_0::GetValue<Register_0::Field_2>(value)) << std::endl;
    std::cout << "f3: " << std::hex << static_cast<std::size_t>(Register_0::GetValue<Register_0::Field_3>(value)) << std::endl;
  }

  {
    constexpr auto value = Register_0::MakeValue(Register_0::Field_2{0xBB}, Register_0::Field_1{0xAA});
    std::cout << "reg: " << std::hex << value << std::endl;

    std::cout << "f1: " << std::hex << static_cast<std::size_t>(Register_0::GetValue<Register_0::Field_1>(value)) << std::endl;
    std::cout << "f2: " << std::hex << static_cast<std::size_t>(Register_0::GetValue<Register_0::Field_2>(value)) << std::endl;
    std::cout << "f3: " << std::hex << static_cast<std::size_t>(Register_0::GetValue<Register_0::Field_3>(value)) << std::endl;
  }

  {
    constexpr auto value = Register_0::MakeValue(Register_0::Field_2{0xBB}, Register_0::Field_1{0xAA}, Register_0::Field_3{0xCCDD});
    std::cout << "reg: " << std::hex << value << std::endl;

    std::cout << "f1: " << std::hex << static_cast<std::size_t>(Register_0::GetValue<Register_0::Field_1>(value)) << std::endl;
    std::cout << "f2: " << std::hex << static_cast<std::size_t>(Register_0::GetValue<Register_0::Field_2>(value)) << std::endl;
    std::cout << "f3: " << std::hex << static_cast<std::size_t>(Register_0::GetValue<Register_0::Field_3>(value)) << std::endl;
  }

  { // OK
    constexpr auto value = Register_1::MakeValue(Register_1::Field_2{0xAABBCCDD}, Register_1::Field_3{0xEE});
    std::cout << "reg: " << std::hex << value << std::endl;
    std::cout << "f2: " << std::hex << static_cast<std::size_t>(Register_1::GetValue<Register_1::Field_2>(value)) << std::endl;

    constexpr auto mask = Register_1::MakeMask(Register_1::Field_2{}, Register_1::Field_4{});
    std::cout << "mask: " << std::hex << mask << std::endl;

    constexpr auto set = Register_1::Set(value, Register_1::Field_2{0xAABB}, Register_1::Field_4{0xC});
    std::cout << "set: " << std::hex << set << std::endl;
  }

//  { // ERROR
//    constexpr auto value = r1.MakeValue(Register_0::Field_2{0xBB});
//    std::cout << "reg: " << std::hex << value << std::endl;
//    std::cout << "f2: " << std::hex << static_cast<std::size_t>(r1.GetValue<Register_0::Field_1>(value)) << std::endl;
//  }

  {
    constexpr auto value = Register_3::MakeValue(Register_3::Field_1{Enum_0::ON}, Register_3::Field_2{0xF0});
    std::cout << "reg: " << std::hex << static_cast<std::size_t>(value) << std::endl;

    constexpr auto set = Register_3::Set(value, Register_3::Field_1{Enum_0::Z});
    std::cout << "set: " << std::hex << static_cast<std::size_t>(set) << std::endl;
  }
  
  {
    uint8_t buffer;
    auto r4 = Register_4::Cast(&buffer);
    r4->Set(Register_4::Field_1{Enum_0::Z});

    std::cout << "reg: " << std::hex << static_cast<std::size_t>(r4->value) << std::endl;
    
    r4->Set(Register_4::Field_1{Enum_0::ON});
    auto f1 = r4->Get<Register_4::Field_1>();

    std::cout << "f1: " << std::hex << static_cast<std::size_t>(f1) << std::endl;
  }

  return 0;
}

