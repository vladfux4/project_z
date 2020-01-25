#include "kernel/utils/register.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace utils {
namespace rtr {

struct Register_0
    : rtr::StaticRegister<std::size_t, rtr::Field<uint8_t, 4>,
                          rtr::Field<uint8_t>, rtr::Field<uint16_t, 16>> {
  using Field_1 = FieldAlias<0>;
  using Field_2 = FieldAlias<1>;
  using Field_3 = FieldAlias<2>;
};

TEST(Register, MakeValue) {
  constexpr auto value_0 = Register_0::MakeValue(Register_0::Field_1(0xAA),
                                                 Register_0::Field_2(0xBB));
  EXPECT_EQ(value_0, 0xBBA);
  constexpr auto value_1 = Register_0::MakeValue(Register_0::Field_2(0xBB),
                                                 Register_0::Field_1(0xAA),
                                                 Register_0::Field_3(0xCCDD));
  EXPECT_EQ(value_1, 0xCCDDBBA);
}

TEST(Register, GetValue) {
  constexpr auto value = Register_0::MakeValue(Register_0::Field_2(0xBB),
                                               Register_0::Field_3(0xCCDD));
  EXPECT_EQ(Register_0::GetValue<Register_0::Field_1>(value), 0);
  EXPECT_EQ(Register_0::GetValue<Register_0::Field_2>(value), 0xBB);
  EXPECT_EQ(Register_0::GetValue<Register_0::Field_3>(value), 0xCCDD);
}

struct Register_1
    : rtr::StaticRegister<std::size_t, rtr::Field<uint8_t, 4>,
                          rtr::Field<uint32_t>, rtr::Field<uint8_t>,
                          rtr::Field<uint16_t>> {
  using Field_1 = FieldAlias<0>;
  using Field_2 = FieldAlias<1>;
  using Field_3 = FieldAlias<2>;
  using Field_4 = FieldAlias<3>;
};

TEST(Register, MakeMask) {
  EXPECT_EQ(Register_1::MakeMask(Register_1::Field_2(), Register_1::Field_4()),
            0xFFFF00FFFFFFFF0);
}

TEST(Register, SetValue) {
  constexpr auto value = Register_1::MakeValue(Register_1::Field_2(0xAABBCCDD),
                                               Register_1::Field_3(0xEE));
  EXPECT_EQ(value, 0xEEAABBCCDD0);
  constexpr auto set = Register_1::Set(value, Register_1::Field_2(0xAABB),
                                       Register_1::Field_4(0xC));
  EXPECT_EQ(set, 0xCEE0000AABB0);
}

}  // namespace rtr
}  // namespace utils
