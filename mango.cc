#include "mango/mango.h"

#include <iostream>

#include "mango/int.h"
#include "mango/nat.h"
#include "gtest/gtest.h"

using namespace mango;

#if 0
constexpr Nat<3> x{5};
constexpr Nat<64> y{77};
constexpr Nat<0> zero;
constexpr Nat<1000> big{x};
#endif

TEST(Nat, DefaultConstructor) {
  Nat<0> n;
  EXPECT_EQ(n.get_low(), 0);
  EXPECT_EQ(n.WIDTH, 0);
  EXPECT_EQ(n.SLACK, 64);
  EXPECT_EQ(n.MASK, 0);
}

TEST(Nat, Constructor) {
  {
    Nat<2> n{5};
    EXPECT_EQ(n.WIDTH, 2);
    EXPECT_EQ(n.SLACK, 62);
    EXPECT_EQ(n.MASK, 0x3);
    EXPECT_EQ(n.high, Nat<0>{});
    EXPECT_EQ(n.low, 1);
  }

  Nat<7> n{5};
  EXPECT_EQ(n.WIDTH, 7);
  EXPECT_EQ(n.SLACK, 57);
  EXPECT_EQ(n.MASK, 0x7f);
  EXPECT_EQ(n.high, Nat<0>{});
  EXPECT_EQ(n.low, 5);

  Nat<100> n2{17};
  EXPECT_EQ(n2.low, 17);
  EXPECT_EQ(n2.WIDTH, 100);
  EXPECT_EQ(n2.SLACK, 0);
  EXPECT_EQ(n2.MASK, ~uint64_t{0});
  const auto h = n2.get_high();
  EXPECT_EQ(h.WIDTH, 36);
  EXPECT_EQ(h.SLACK, 28);
  EXPECT_EQ(h.MASK, uint64_t{0xfffffffff});
  EXPECT_EQ(h.low, 0);
}

TEST(Nat, Add) {
  Nat<4> n{5};
  Nat<6> m{7};
  auto r = n + m;
  EXPECT_EQ(r.WIDTH, 7);
  EXPECT_EQ(r.low, 12);
}

TEST(Nat, Factory) {
  auto x = nat();
  EXPECT_EQ(x.WIDTH, 0);
  EXPECT_EQ(nat(7).WIDTH, 32);
  EXPECT_EQ(nat(7, 8).WIDTH, 64);
  EXPECT_EQ(nat(0x1, 0x2), nat(UINT64_C(0x0000000100000002)));
}

TEST(UInt, DefaultConstructor) {
  UInt<0> i{};
  EXPECT_EQ(i.WIDTH, 0);
  EXPECT_EQ(i.MIN_VALUE, nat());
  EXPECT_EQ(i.MAX_VALUE, nat());
  EXPECT_EQ(i.abs.get_low(), 0);

  UInt<3> i3{5};
  EXPECT_EQ(i3.WIDTH, 3);
  EXPECT_EQ(i3.MIN_VALUE, nat());
  EXPECT_EQ(i3.MAX_VALUE, nat(7));
  EXPECT_EQ(i3.abs.get_low(), 5);
}

TEST(UInt, Add) {
  UInt<4> i{5};
  UInt<6> j{7};
  auto r = i + j;
  EXPECT_EQ(r.WIDTH, 7);
  EXPECT_EQ(r.MIN_VALUE, nat());
  EXPECT_EQ(r.MAX_VALUE, i.MAX_VALUE + j.MAX_VALUE);
  EXPECT_EQ(r.abs.low, 12);
}

int main(int argc, char **argv) {
  printf("hello\n");
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}