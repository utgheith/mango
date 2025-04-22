#include "mango/mango.h"

#include <iostream>

#include "gtest/gtest.h"
#include "mango/int.h"
#include "mango/ranged_int.h"

using namespace mango;

#if 0
constexpr Nat<3> x{5};
constexpr Nat<64> y{77};
constexpr Nat<0> zero;
constexpr Nat<1000> big{x};
#endif

TEST(Nat, DefaultConstructor) {
  Nat<0> n;
  EXPECT_EQ(n.low, 0);
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
  const auto h = n2.upper();
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

int main(int argc, char **argv) {
  printf("hello\n");
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}