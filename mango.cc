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
#endif

constexpr Nat<> zero{};
constexpr Nat<0> z{};
constexpr Nat<0, 0> zz{};

TEST(mango, cmp) {
  EXPECT_EQ(cmp(~int64_t(0), uint64_t(0)), Cmp::LT);
  EXPECT_EQ(cmp(INT64_MIN, UINT64_MAX), Cmp::LT);
  EXPECT_EQ(cmp(uint64_t(0), ~int64_t(0)), Cmp::GT);
  EXPECT_EQ(cmp(UINT64_MAX, INT64_MIN), Cmp::GT);
}

TEST(Nat, IsZero) {
  EXPECT_TRUE(zero.is_zero());
  EXPECT_TRUE(z.is_zero());
  EXPECT_TRUE(zz.is_zero());
  EXPECT_TRUE(zero == z);
  EXPECT_TRUE(z == zz);
  EXPECT_TRUE(z.succ() == zz.succ());
  EXPECT_FALSE(zero.succ().is_zero());
  EXPECT_FALSE(z.succ().is_zero());
  EXPECT_FALSE(zz.succ().is_zero());
  EXPECT_TRUE(zero.succ().succ() > zero);
  EXPECT_TRUE(z.succ().succ() > z);
  EXPECT_TRUE(zz.succ().succ() > zz);
  EXPECT_FALSE(Nat<1>{}.is_zero());

  auto x = Nat<~uint64_t(0)>{};
  auto y = x.succ();

  EXPECT_TRUE(x < y);
  EXPECT_TRUE(y == (Nat<0, 1>{}));

  EXPECT_TRUE(-x == (Neg<~uint64_t(0)>{}));

  EXPECT_EQ(z.bit_size(), 0);
  EXPECT_EQ(zz.bit_size(), 0);
  EXPECT_EQ(zero.bit_size(), 0);
  EXPECT_EQ((Nat<1, 0, 1>{}).bit_size(), 1 + 64 + 64);
}

TEST(CtNat, low) {
  EXPECT_EQ((Nat<1, 2, 3>{}).low, 1);
  EXPECT_EQ(Nat<5>{}.low, 5);
  EXPECT_EQ(Nat<>{}.low, 0);
}

TEST(CtNat, high) {
  EXPECT_TRUE((Nat<1, 2, 3>{}).high() == (Nat<2, 3>{}));
  EXPECT_TRUE((Nat<5>{}).high() == Nat<>{});
  EXPECT_TRUE((Nat<>{}).high() == Nat<>{});
}

TEST(CtNat, bit_size) {
  EXPECT_EQ((Nat<1, 2, 3>{}).bit_size(), 64 + 64 + 2);
  EXPECT_EQ((Nat<5>{}).bit_size(), 3);
  EXPECT_EQ((Nat<>{}).bit_size(), 0);
}

TEST(CtNat, is_zero) {
  EXPECT_FALSE((Nat<1, 2, 3>{}).is_zero());
  EXPECT_FALSE((Nat<5>{}).is_zero());
  EXPECT_TRUE((Nat<>{}).is_zero());
}

TEST(CtNat, inject_right) {
  EXPECT_TRUE((Nat<1, 2, 3>{}).template inject_right<0>() ==
              (Nat<0, 1, 2, 3>{}));
  EXPECT_TRUE((Nat<5>{}).template inject_right<1>() == (Nat<1, 5>{}));
  EXPECT_TRUE((Nat<>{}).template inject_right<2>() == Nat<2>{});
}

TEST(CtNat, succ) {
  EXPECT_TRUE((Nat<1, 2, 3>{}).succ() == (Nat<2, 2, 3>{}));
  EXPECT_TRUE((Nat<5>{}).succ() == (Nat<6>{}));
  EXPECT_TRUE((Nat<>{}).succ() == Nat<1>{});
  EXPECT_TRUE((Nat<~uint64_t(0)>{}).succ() == (Nat<0, 1>{}));
  EXPECT_TRUE((Nat<~uint64_t(0), ~uint64_t(0), 1>{}).succ() ==
              (Nat<0, 0, 2>{}));
}

TEST(CtNat, add) {
  EXPECT_TRUE((Nat<1, 2, 3>{} + Nat<4, 5>{}) == (Nat<5, 7, 3>{}));
  EXPECT_TRUE((Nat<5>{} + Nat<6>{}) == (Nat<11>{}));
  EXPECT_TRUE((Nat<>{} + Nat<1, 2>{}) == (Nat<1, 2>{}));
  EXPECT_TRUE((Nat<1, 2, 3>{} + Nat<0, 0, 1>{}) == (Nat<1, 2, 4>{}));
  EXPECT_TRUE(Nat<~uint64_t(0)>{} + Nat<1>{} == (Nat<0, 1>{}));
  EXPECT_TRUE(((Nat<~uint64_t(0), ~uint64_t(0)>{}) + Nat<5>{}) ==
              (Nat<4, 0, 1>{}));
}

TEST(CtNat, Sub) {
  EXPECT_TRUE((Nat<8>{} - Nat<3>{}) == Nat<5>{});

  const auto x = Nat<1, 0, 1>{};
  const auto y = Nat<2, 0, 1>{};
  EXPECT_TRUE(y - x == Nat<1>{});
  EXPECT_TRUE(x - y == Neg<1>{});

  // EXPECT_EQ(y+x, (Nat<3, 0, 2>{}));
  // EXPECT_EQ(y-x, (Nat<1>{}));
}

constexpr auto nat_values = std::make_tuple(
    Nat<1, 2, 3>{}, Nat<4, 5>{}, Nat<5>{}, Nat<6>{}, Nat<>{}, Nat<1, 2>{},
    Nat<0, 0, 1>{}, Nat<~uint64_t(0)>{}, Nat<~uint64_t(0), ~uint64_t(0), 1>{});

template <uint64_t... Vs> void one(const Nat<Vs...> n) {
  ASSERT_TRUE(n == -(-n));
  ASSERT_TRUE(n == n.succ() - Nat<1>{});
  ASSERT_TRUE(n - n == Nat<>{});
  ASSERT_TRUE(n - n.succ() == Neg<1>{});
  ASSERT_TRUE(n + (-n) == Nat<>{});

  auto neg = -n;
  ASSERT_TRUE(neg + n == Nat<>{});
}

TEST(CtNat, Misc) {

  std::apply([](auto &&...args) { ((one(args)), ...); }, nat_values);
}

TEST(RtInt, Simple) {

  constexpr auto i = Int<Nat<3>, Nat<29>>{Nat<17>{}};
  EXPECT_TRUE(i.min == Nat<3>{});
  EXPECT_TRUE(i.max == Nat<29>{});
  EXPECT_TRUE(i.range == Nat<27>{});
  EXPECT_EQ(i.bitsize, 5);
  EXPECT_EQ(i.get(0), 14);

  constexpr auto j = Int<Neg<4>, Nat<2>>{Neg<3>{}};
  EXPECT_TRUE(j.min == Neg<4>{});
  EXPECT_TRUE(j.max == Nat<2>{});
  EXPECT_TRUE(j.range == Nat<7>{});
  EXPECT_EQ(j.bitsize, 3);
  EXPECT_EQ(j.get(0), 1);

  const auto k = i + j;
  EXPECT_TRUE(k.min == Neg<1>{});
  EXPECT_TRUE(k.max == Nat<31>{});
  EXPECT_TRUE(k.range == Nat<33>{});
  EXPECT_EQ(k.bitsize, 6);
  EXPECT_EQ(k.get(0), 15);
}

TEST(CtNat, ShiftLeft) {
  EXPECT_TRUE(Nat<1>{} << Nat<3>{} == Nat<8>{});
  EXPECT_TRUE(Nat<1>{} << Nat<64>{} == (Nat<0, 1>{}));
}

TEST(UnsignedInt, Simple) {
  const UnsignedInt<0> u0{};
  EXPECT_TRUE(u0.min == Nat<>{});
  EXPECT_TRUE(u0.max == Nat<>{});

  const UnsignedInt<3> u3{};
  EXPECT_TRUE(u3.min == Nat<>{});

  EXPECT_TRUE(u3.max == Nat<7>{});
}

TEST(SignedInt, Simple) {
  const SignedInt<0> s0{};
  EXPECT_TRUE(s0.min == Nat<>{});
  EXPECT_TRUE(s0.max == Nat<>{});

  const SignedInt<3> s3{};
  EXPECT_TRUE(s3.min == Neg<4>{});
  EXPECT_TRUE(s3.max == Nat<3>{});

  const auto v = UInt(Nat<3>{}) + UInt(Nat<7>{});
  EXPECT_TRUE(v.min == Nat<>{});
  EXPECT_TRUE(v.max == Nat<10>{});
  EXPECT_TRUE(v.get(0) == 10);

  // const auto v = SInt<3>{3} + SInt<7>{2};
  // EXPECT_EQ(v.min, Neg<67>{});
}

int main(int argc, char **argv) {
  printf("hello\n");
  testing::InitGoogleTest(&argc, argv);
  auto out = RUN_ALL_TESTS();

  std::cout << Nat<0, ~uint64_t(0), 1>{}.succ() << std::endl;

  return out;
}
