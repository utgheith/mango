#include "mango/mango.h"

#include <iostream>

#include "mango/ct/nat.h"
#include "mango/rt/int.h"
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

constexpr ct::Nat<> zero{};
constexpr ct::Nat<0> z{};
constexpr ct::Nat<0, 0> zz{};

TEST(Nat, IsZero) {
  EXPECT_TRUE(zero.is_zero());
  EXPECT_TRUE(z.is_zero());
  EXPECT_TRUE(zz.is_zero());
  EXPECT_EQ(zero, z);
  EXPECT_EQ(z, zz);
  EXPECT_EQ(z.succ(), zz.succ());
  EXPECT_FALSE(zero.succ().is_zero());
  EXPECT_FALSE(z.succ().is_zero());
  EXPECT_FALSE(zz.succ().is_zero());
  EXPECT_TRUE(zero.succ().succ() > zero);
  EXPECT_TRUE(z.succ().succ() > z);
  EXPECT_TRUE(zz.succ().succ() > zz);
  EXPECT_FALSE(ct::Nat<1>{}.is_zero());

  auto x = ct::Nat<~uint64_t(0)>{};
  auto y = x.succ();

  EXPECT_TRUE(x < y);
  EXPECT_EQ(y, (ct::Nat<0, 1>{}));

  EXPECT_EQ(-x, (ct::Neg<~uint64_t(0)>{}));

  EXPECT_EQ(z.bit_size(), 0);
  EXPECT_EQ(zz.bit_size(), 0);
  EXPECT_EQ(zero.bit_size(), 0);
  EXPECT_EQ((ct::Nat<1, 0, 1>{}).bit_size(), 1 + 64 + 64);
}

TEST(CtNat, low) {
  EXPECT_EQ((ct::Nat<1, 2, 3>{}).low, 1);
  EXPECT_EQ(ct::Nat<5>{}.low, 5);
  EXPECT_EQ(ct::Nat<>{}.low, 0);
}

TEST(CtNat, high) {
  EXPECT_EQ((ct::Nat<1, 2, 3>{}).high(), (ct::Nat<2, 3>{}));
  EXPECT_EQ((ct::Nat<5>{}).high(), ct::Nat<>{});
  EXPECT_EQ((ct::Nat<>{}).high(), ct::Nat<>{});
}

TEST(CtNat, bit_size) {
  EXPECT_EQ((ct::Nat<1, 2, 3>{}).bit_size(), 64 + 64 + 2);
  EXPECT_EQ((ct::Nat<5>{}).bit_size(), 3);
  EXPECT_EQ((ct::Nat<>{}).bit_size(), 0);
}

TEST(CtNat, is_zero) {
  EXPECT_FALSE((ct::Nat<1, 2, 3>{}).is_zero());
  EXPECT_FALSE((ct::Nat<5>{}).is_zero());
  EXPECT_TRUE((ct::Nat<>{}).is_zero());
}

TEST(CtNat, inject_right) {
  EXPECT_EQ((ct::Nat<1, 2, 3>{}).template inject_right<0>(),
            (ct::Nat<0, 1, 2, 3>{}));
  EXPECT_EQ((ct::Nat<5>{}).template inject_right<1>(), (ct::Nat<1, 5>{}));
  EXPECT_EQ((ct::Nat<>{}).template inject_right<2>(), ct::Nat<2>{});
}

TEST(CtNat, succ) {
  EXPECT_EQ((ct::Nat<1, 2, 3>{}).succ(), (ct::Nat<2, 2, 3>{}));
  EXPECT_EQ((ct::Nat<5>{}).succ(), (ct::Nat<6>{}));
  EXPECT_EQ((ct::Nat<>{}).succ(), ct::Nat<1>{});
  EXPECT_EQ((ct::Nat<~uint64_t(0)>{}).succ(), (ct::Nat<0, 1>{}));
  EXPECT_EQ((ct::Nat<~uint64_t(0), ~uint64_t(0), 1>{}).succ(),
            (ct::Nat<0, 0, 2>{}));
}

TEST(CtNat, add) {
  EXPECT_EQ((ct::Nat<1, 2, 3>{} + ct::Nat<4, 5>{}), (ct::Nat<5, 7, 3>{}));
  EXPECT_EQ((ct::Nat<5>{} + ct::Nat<6>{}), (ct::Nat<11>{}));
  EXPECT_EQ((ct::Nat<>{} + ct::Nat<1, 2>{}), (ct::Nat<1, 2>{}));
  EXPECT_EQ((ct::Nat<1, 2, 3>{} + ct::Nat<0, 0, 1>{}), (ct::Nat<1, 2, 4>{}));
  EXPECT_EQ(ct::Nat<~uint64_t(0)>{} + ct::Nat<1>{}, (ct::Nat<0, 1>{}));
  EXPECT_EQ((ct::Nat<~uint64_t(0), ~uint64_t(0)>{}) + ct::Nat<5>{},
            (ct::Nat<4, 0, 1>{}));
}

TEST(CtNat, Sub) {
  EXPECT_EQ(ct::Nat<8>{} - ct::Nat<3>{}, ct::Nat<5>{});

  const auto x = ct::Nat<1, 0, 1>{};
  const auto y = ct::Nat<2, 0, 1>{};
  EXPECT_EQ(y - x, ct::Nat<1>{});
  EXPECT_EQ(x - y, ct::Neg<1>{});

  // EXPECT_EQ(y+x, (ct::Nat<3, 0, 2>{}));
  // EXPECT_EQ(y-x, (ct::Nat<1>{}));
}

constexpr auto nat_values = std::make_tuple(
    ct::Nat<1, 2, 3>{}, ct::Nat<4, 5>{}, ct::Nat<5>{}, ct::Nat<6>{},
    ct::Nat<>{}, ct::Nat<1, 2>{}, ct::Nat<0, 0, 1>{}, ct::Nat<~uint64_t(0)>{},
    ct::Nat<~uint64_t(0), ~uint64_t(0), 1>{});

template <uint64_t... Vs> void one(const ct::Nat<Vs...> n) {
  ASSERT_EQ(n, -(-n));
  ASSERT_EQ(n, n.succ() - ct::Nat<1>{});
  ASSERT_EQ(n - n, ct::Nat<>{});
  ASSERT_EQ(n - n.succ(), ct::Neg<1>{});
  ASSERT_EQ(n + (-n), ct::Nat<>{});

  auto neg = -n;
  ASSERT_EQ(neg + n, ct::Nat<>{});
}

TEST(CtNat, Misc) {

  std::apply([](auto &&...args) { ((one(args)), ...); }, nat_values);
}

TEST(RtInt, Simple) {

  constexpr auto i = rt::Int<ct::Nat<3>, ct::Nat<29>>{ct::Nat<17>{}};
  EXPECT_EQ(i.min, ct::Nat<3>{});
  EXPECT_EQ(i.max, ct::Nat<29>{});
  EXPECT_EQ(i.range, ct::Nat<27>{});
  EXPECT_EQ(i.bitsize, 5);
  EXPECT_EQ(i.get(0), 14);

  constexpr auto j = rt::Int<ct::Neg<4>, ct::Nat<2>>{ct::Neg<3>{}};
  EXPECT_EQ(j.min, ct::Neg<4>{});
  EXPECT_EQ(j.max, ct::Nat<2>{});
  EXPECT_EQ(j.range, ct::Nat<7>{});
  EXPECT_EQ(j.bitsize, 3);
  EXPECT_EQ(j.get(0), 1);

  const auto k = i + j;
  EXPECT_EQ(k.min, ct::Neg<1>{});
  EXPECT_EQ(k.max, ct::Nat<31>{});
  EXPECT_EQ(k.range, ct::Nat<33>{});
  EXPECT_EQ(k.bitsize, 6);
  EXPECT_EQ(k.get(0), 15);
}

TEST(CtNat, ShiftLeft) {
  EXPECT_EQ(ct::Nat<1>{} << ct::Nat<3>{}, ct::Nat<8>{});
  EXPECT_EQ(ct::Nat<1>{} << ct::Nat<64>{}, (ct::Nat<0, 1>{}));
}

TEST(UInt, Simple) {
  const mango::rt::UInt<0> u0{};
  EXPECT_EQ(u0.min, ct::Nat<>{});
  EXPECT_EQ(u0.max, ct::Nat<>{});

  const mango::rt::UInt<3> u3{};
  EXPECT_EQ(u3.min, ct::Nat<>{});

  EXPECT_EQ(u3.max, ct::Nat<7>{});
}

TEST(SInt, Simple) {
  using namespace mango::rt;
  using namespace mango::ct;

  const SInt<0> s0{};
  EXPECT_EQ(s0.min, Nat<>{});
  EXPECT_EQ(s0.max, Nat<>{});

  const SInt<3> s3{};
  EXPECT_EQ(s3.min, Neg<4>{});
  EXPECT_EQ(s3.max, Nat<3>{});

  const auto v = make_uint(Nat<3>{}) + make_uint(Nat<7>{});
  EXPECT_EQ(v.min, Nat<>{});
  EXPECT_EQ(v.max, Nat<10>{});
  EXPECT_EQ(v.get(0), 10);

  // const auto v = SInt<3>{3} + SInt<7>{2};
  // EXPECT_EQ(v.min, Neg<67>{});
}

int main(int argc, char **argv) {
  printf("hello\n");
  testing::InitGoogleTest(&argc, argv);
  auto out = RUN_ALL_TESTS();

  std::cout << ct::Nat<0, ~uint64_t(0), 1>{}.succ() << std::endl;

  return out;
}