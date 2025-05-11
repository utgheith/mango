

#include <iostream>

#include "mango/bits.h"
#include "mango/int.h"
#include "mango/masked_bits.h"
#include "mango/nat.h"
#include <gtest/gtest.h>

#include "inspect.cc"

using namespace mango;

auto biso(const Bits<32> x) { return x.sign_extend<65>(); }

TEST(Bits, Simple) {
  const Bits<3> x{5};
  const Bits<64> y{77};
  const Bits<1000> big{x};

  EXPECT_EQ(x.low, 5);
  EXPECT_EQ(y.low, 77);
  EXPECT_EQ(big.low, 5);

  EXPECT_EQ(big.WIDTH, 1000);

  const auto c = x.concat(x);
  EXPECT_EQ(c.WIDTH, 6);
  EXPECT_EQ(c.low, 0b101101);
}

TEST(Bits, DefaultConstructor) {
  Bits<0> n;
  EXPECT_EQ(n.get_low(), 0);
  EXPECT_EQ(n.WIDTH, 0);
  EXPECT_EQ(n.SLACK, 64);
  EXPECT_EQ(n.MASK, 0);
}

TEST(Bits, Constructor) {
  {
    Bits<2> n{5};
    EXPECT_EQ(n.WIDTH, 2);
    EXPECT_EQ(n.SLACK, 62);
    EXPECT_EQ(n.MASK, 0x3);
    EXPECT_EQ(n.high, Bits<0>{});
    EXPECT_EQ(n.low, 1);
  }

  Bits<7> n{5};
  EXPECT_EQ(n.WIDTH, 7);
  EXPECT_EQ(n.SLACK, 57);
  EXPECT_EQ(n.MASK, 0x7f);
  EXPECT_EQ(n.high, Bits<0>{});
  EXPECT_EQ(n.low, 5);

  Bits<100> n2{17};
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

TEST(Bits, add) {
  EXPECT_EQ((Bits<0>{} + Bits<0>{}).WIDTH, 0);
  EXPECT_EQ((Bits<0>{} + Bits<1>{}).WIDTH, 1);
  EXPECT_EQ((Bits<1>{} + Bits<0>{}).WIDTH, 1);
  EXPECT_EQ((Bits<1>{} + Bits<1>{}).WIDTH, 2);
  {
    const auto a = Bits<64>{0} + Bits<1>{1};
    EXPECT_EQ(a.WIDTH, 65);
    EXPECT_EQ(a.get(0), 1);
    EXPECT_EQ(a.get(1), 0);
  }

  {
    const auto a = Bits<1>{1} + Bits<1>{1};
    EXPECT_EQ(a.WIDTH, 2);
    EXPECT_EQ(a.get(0), 2);
    EXPECT_EQ(a.get(1), 0);
  }

  {
    const auto a = ~Bits<64>{0} + Bits<1>{1};
    EXPECT_EQ(a.WIDTH, 65);
    EXPECT_EQ(a.get(0), 0);
    EXPECT_EQ(a.get(1), 1);
  }

  {
    const auto out = add(Bits<2>(1).concat(~Bits<64>(0)), Bits<61>(1));
    EXPECT_EQ(out.WIDTH, 67);
  }
}

TEST(Bits, concat) {
  const Bits<65> a{1};
  auto b = a.concat(a);
  EXPECT_EQ(b.WIDTH, 130);
  EXPECT_EQ(b.get(0), 1);
  EXPECT_EQ(b.get(1), 2);
  EXPECT_EQ(b.get(2), 0);
}

TEST(Bits, shr) {
  const Bits<66> a{2, 77};
  const auto b = a.shr<65>();
  EXPECT_EQ(b.WIDTH, 1);
  EXPECT_EQ(b.get(0), 1);
}

TEST(Bits, extract) {
  const Bits<66> a{2, 77};
  const auto b = a.extract<65, 63>();
  EXPECT_EQ(b.WIDTH, 3);
  EXPECT_EQ(b.get(0), 4);
}

TEST(Bits, trim) {
  const auto a = ~Bits<65>{0};
  EXPECT_EQ(a.WIDTH, 65);
  EXPECT_EQ(a.get(0), UINT64_MAX);
  EXPECT_EQ(a.get(1), 1);
  const auto b = a.trim<63>();
  EXPECT_EQ(b.WIDTH, 63);
  EXPECT_EQ(b.get(0), INT64_MAX);
  EXPECT_EQ(b.get(1), 0);
}

TEST(Bits, zero_extend) {
  const auto a = ~Bits<63>{INT64_MAX};
  const auto b = a.zero_extend<65>();
  EXPECT_EQ(b.WIDTH, 65);
  EXPECT_EQ(b.get(0), a.get(0));
  EXPECT_EQ(b.get(1), 0);
}

TEST(Bits, is_signed) {
  EXPECT_TRUE(Bits<3>{4}.is_signed());
  EXPECT_FALSE(Bits<4>{4}.is_signed());
}

TEST(Bits, sign_extend) {
  auto a = Bits<3>{4};
  auto b = a.sign_extend<129>();
  EXPECT_EQ(b.WIDTH, 129);
  EXPECT_TRUE(b.is_signed());
  EXPECT_EQ(~(b.get(0)), 3);
  EXPECT_EQ(~(b.get(1)), 0);
  EXPECT_EQ(b.get(2), 1);
}

TEST(Bits, sub) {
  for (int32_t i = -2; i < 2; i++) {
    for (int32_t j = -2; j < 2; j++) {
      auto diff = Bits<2>{i} - Bits<2>{j};
      EXPECT_EQ(diff.WIDTH, 3);

      EXPECT_EQ(int32_t(diff.sign_extend<64>().low), i - j);
    }
  }

  auto z1 = Bits<1>{1} - ~Bits<200>(0);
  EXPECT_EQ(z1.WIDTH, 201);
  EXPECT_EQ(z1.get(0), 0);
  EXPECT_EQ(z1.get(1), 0);
  EXPECT_EQ(z1.get(2), 0);

  auto z2 = Bits<2>{1} - ~Bits<200>(0);
  EXPECT_EQ(z2.WIDTH, 201);
  EXPECT_EQ(z2.get(0), 2);
  EXPECT_EQ(z2.get(1), 0);
  EXPECT_EQ(z2.get(2), 0);
}

TEST(Bits, flip_bit) {
  auto o = flip_bit(Bits<129>{0});
  EXPECT_EQ(o.WIDTH, 129);
  EXPECT_EQ(o.get(0), 0);
  EXPECT_EQ(o.get(1), 0);
  EXPECT_EQ(o.get(2), 1);

  auto o2 = Bits<1>{1}.concat(Bits<128>{0}).flip_bit<128>();
  EXPECT_EQ(o2.get(0), 0);
  EXPECT_EQ(o2.get(1), 0);
  EXPECT_EQ(o2.get(2), 0);
  EXPECT_EQ(o2, Bits<0>{});

  {
    auto o = Bits<0>{}.flip_bit<2000>();
    EXPECT_EQ(o.WIDTH, 2001);
    EXPECT_TRUE((o.extract<1999, 0>()) == Bits<0>{});
    EXPECT_EQ(o.shr<2000>(), Bits<1>{1});
  }
}

TEST(MaskedBits, simple) {
  {
    const auto a =
        adc(MaskedBits<32, Nat<0x80000000>, Nat<0x80000000>>{0x12345678});
    EXPECT_EQ(a.WIDTH, 64);
  }
  {
    const auto a =
        adc(MaskedBits<32, Nat<0x80000000>, Nat<0x00000000>>{0x12345678});
    EXPECT_EQ(a.WIDTH, 32);
  }
}

#if 0

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

TEST(Nat, Misc) {

  std::apply([](auto &&...args) { ((one(args)), ...); }, nat_values);
}

TEST(Int, Simple) {

  const auto i = Int<Nat<3>, Nat<29>>{Nat<17>{}};
  EXPECT_TRUE(i.min == Nat<3>{});
  EXPECT_TRUE(i.max == Nat<29>{});
  EXPECT_TRUE(i.range == Nat<27>{});
  EXPECT_EQ(i.bitsize, 5);
  EXPECT_EQ(i.get(0), 14);

  const auto j = Int<Neg<4>, Nat<2>>{Neg<3>{}};
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

TEST(Nat, ShiftLeft) {
  EXPECT_TRUE((Nat<1>{} << Nat<3>{}) == Nat<8>{});
  EXPECT_TRUE((Nat<1>{} << Nat<64>{}) == (Nat<0, 1>{}));
}

TEST(Nat, ShiftRight) {
  EXPECT_TRUE((Nat<8>{} >> Nat<3>{}) == Nat<1>{});
  EXPECT_TRUE(((Nat<0, 1>{}) >> Nat<64>{}) == (Nat<1>{}));
}

TEST(UnsignedInt, Simple) {
  const UnsignedInt<0> u0{};
  EXPECT_TRUE(u0.min == Nat<>{});
  EXPECT_TRUE(u0.max == Nat<>{});

  const UnsignedInt<3> u3{};
  EXPECT_TRUE(u3.min == Nat<>{});

  EXPECT_TRUE(u3.max == Nat<7>{});

  auto a = UInt(Nat<3>{});
  EXPECT_TRUE(a.min == Nat<3>{});
  EXPECT_TRUE(a.max == Nat<3>{});
  EXPECT_EQ(a.get(0), 0);

  const auto v = a + UInt(Nat<7>{});
  EXPECT_TRUE(v.min == Nat<10>{});
  EXPECT_TRUE(v.max == Nat<10>{});
  EXPECT_TRUE(v.get(0) == 0);

  {
    Bits<3> b3{2};
    const auto u3 = UInt(b3);
    EXPECT_TRUE(u3.min == Nat<>{});
    EXPECT_TRUE(u3.max == Nat<7>{});
    EXPECT_EQ(b3.get(0), 2);
  }
}

TEST(SignedInt, Simple) {
  const SignedInt<0> s0{};
  EXPECT_TRUE(s0.min == Nat<>{});
  EXPECT_TRUE(s0.max == Nat<>{});

  const SignedInt<3> s3{};
  EXPECT_TRUE(s3.min == Neg<4>{});
  EXPECT_TRUE(s3.max == Nat<3>{});

  {
    auto s = SInt(Bits<1>{1});
    EXPECT_TRUE(s.min == Neg<1>{});
    EXPECT_TRUE(s.max == Nat<>{});
    EXPECT_EQ(s.biased_bits, Bits<0>{});
  }

  {
    auto s = SInt(Bits<1>{0});
    EXPECT_TRUE(s.min == Neg<1>{});
    EXPECT_TRUE(s.max == Nat<>{});
    EXPECT_EQ(s.biased_bits, Bits<1>{1});
  }
}

TEST(Inspect, all) {
  const auto n12 = add(Nat<5>{}, Nat<7>{});
  EXPECT_EQ(n12.get(0), 12);
  EXPECT_EQ(n12.bit_size(), 4);
  EXPECT_FALSE(n12.is_zero());

  const auto b19 = add(Bits<7>{2}, Bits<12>{3});
  EXPECT_EQ(b19.WIDTH, 13);
  EXPECT_EQ(b19.get(0), 5);
  EXPECT_EQ(b19.MASK, 0x1FFF);

  const auto b67 = add(Bits<2>{1}.concat(~Bits<64>{0}), Bits<61>{1});
  EXPECT_EQ(b67.WIDTH, 67);
  EXPECT_EQ(b67.get_high().MASK, 7);
  EXPECT_EQ(b67.get(0), 0);
  EXPECT_EQ(b67.get(1), 2);

  auto u13 = add(UInt(Bits<12>{12}), UInt(Bits<5>{5}));
  EXPECT_EQ(u13.biased_bits.get(0), 17);
  EXPECT_EQ(u13.min.get(0), 0);
  EXPECT_EQ(u13.max.get(0), (1 << 12) - 1 + (1 << 5) - 1);
}

int main(int argc, char **argv) {
  printf("hello\n");
  testing::InitGoogleTest(&argc, argv);
  auto out = RUN_ALL_TESTS();

  std::cout << Nat<0, ~uint64_t(0), 1>{}.succ() << std::endl;

  return out;
}
