#include "kernel/utils/variant.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace utils {

struct Foo {
  uint8_t a;
  uint8_t b;

  bool operator==(const Foo& obj) const { return (a == obj.a && b == obj.b); }
};

struct Bar {
  uint32_t a;
  uint16_t b;

  bool operator==(const Bar& obj) const { return (a == obj.a && b == obj.b); }
};

using TestVariant = Variant<uint8_t, uint8_t, Foo, Bar>;

TEST(Variant, Construct) {
  EXPECT_EQ(TestVariant::kMaxSize, sizeof(Bar));
  EXPECT_EQ(TestVariant::kMaxSize, sizeof(TestVariant));

  TestVariant var;

  auto foo = Foo{0xFF, 0xEE};
  var = foo;
  EXPECT_EQ(var.Get<Foo>(), foo);

  auto bar = Bar{0xAABBCCDD, 0xEEFF};
  var = bar;
  EXPECT_EQ(var.Get<Bar>(), bar);

  auto val = uint8_t(0xFF);
  var = val;
  EXPECT_EQ(var.Get<uint8_t>(), val);
}

}  // namespace utils
