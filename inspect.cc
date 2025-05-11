#include <mango/bits.h>
#include <mango/int.h>
#include <mango/masked_bits.h>
#include <mango/nat.h>

using namespace mango;

const Nat<12> add(const Nat<5> a, const Nat<7> b) { return a + b; }

auto add(const Bits<7> &a, const Bits<12> &b) { return a + b; }

auto add(const Bits<66> &a, const Bits<61> &b) { return a + b; }

auto add(const UnsignedInt<12> a, const UnsignedInt<5> b) noexcept {
  return a + b;
}

auto flip_bit(const Bits<129> b) noexcept { return b.flip_bit<128>(); }

auto shr(const Nat<0x12345678, 0xf0f0f0f0f0> x) noexcept {
  return x >> Nat<4>{};
}

auto is64(MaskedBits<32, Nat<0x80000000>, Nat<0x80000000>> ins) noexcept {
  return ins.template extract<31, 31>();
}

template <typename InsType> auto adc(const InsType ins) noexcept {
  const auto sf = ins.template extract<31, 31>();
  const auto data_size = (sf == Nat<1>{}) ? 64 : 32;
  mango::Bits<data_size> data{};
  return ~data;
}

template auto adc<MaskedBits<32, Nat<0x80000000>, Nat<0x80000000>>>(
    const MaskedBits<32, Nat<0x80000000>, Nat<0x80000000>> ins);
template auto adc<MaskedBits<32, Nat<0x80000000>, Nat<0x00000000>>>(
    const MaskedBits<32, Nat<0x80000000>, Nat<0x00000000>> ins);
