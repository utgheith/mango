#pragma once

#include <cassert>
#include <cstdint>
#include <iostream>
#include <mango/ct/nat.h>
#include <type_traits>

namespace mango::rt {

template <typename Min, typename Max> struct Int {
  constexpr static Min min{};
  constexpr static Max max{};
  static_assert(Min{} <= Max{}, "Min must be less than or equal to Max");
  constexpr static auto range = max - min + ct::Nat<1>{};
  constexpr static uint64_t bitsize = range.bit_size();
  constexpr static uint64_t LEN =
      (bitsize + 63) / 64; // Number of uint64_t words needed
  uint64_t state[LEN];

  constexpr Int() noexcept {
    for (uint64_t i = 0; i < LEN; ++i) {
      state[i] = 0;
    }
  }

  template <typename T> constexpr Int(const T value) noexcept {
    assert(min <= value);
    assert(max >= value);
    auto diff = value - min;
    for (uint64_t i = 0; i < LEN; ++i) {
      state[i] = diff.get(i);
    }
  }

  constexpr uint64_t get(uint64_t i) const noexcept {
    return (i < LEN) ? state[i] : 0;
  }

  template <typename A, typename B>
  constexpr Int(const A &lhs, const B &rhs) noexcept {
#ifndef __clang__
    static_assert((bitsize - ((lhs.bitsize > rhs.bitsize) ? lhs.bitsize
                                                          : rhs.bitsize)) < 2);
#endif
    uint64_t carry = 0;
    for (uint64_t i = 0; i < LEN; ++i) {
      const uint64_t a = lhs.get(i);
      const uint64_t b = rhs.get(i);
      const uint64_t t = a + b + carry;
      carry = ((t < a) || (t < b)) ? 1 : 0;
      state[i] = t;
    }
  }

  template <typename Min2, typename Max2>
  constexpr auto operator+(const Int<Min2, Max2> &other) const noexcept {
    using OutType = Int<decltype(Min{} + Min2{}), decltype(Max{} + Max2{})>;

    return OutType{*this, other};
  }
};

///////////////// UInt /////////////////////

template <uint16_t N>
using UInt = Int<mango::ct::Nat<>,
                 decltype((mango::ct::Nat<1>{} << mango::ct::Nat<N>{}) -
                          mango::ct::Nat<1>{})>;

template <uint64_t N>
struct SInt : Int<decltype(-(mango::ct::Nat<1>{} << mango::ct::Nat<(N - 1)>{})),
                  decltype((mango::ct::Nat<1>{} << mango::ct::Nat<(N - 1)>{}) -
                           mango::ct::Nat<1>{})> {};

template <> struct SInt<0> : Int<mango::ct::Nat<>, mango::ct::Nat<>> {};

template <typename Min, typename Max>
std::ostream &operator<<(std::ostream &os, const Int<Min, Max> &value) {
  os << "Int<" << value.min << ", " << value.max << "> = ";
  for (uint64_t i = 0; i < value.LEN; ++i) {
    os << std::format("{:016X}", value.get(value.LEN - i));
  }

  return os;
}

template <uint64_t... Vs>
constexpr Int<mango::ct::Nat<>, mango::ct::Nat<Vs...>>
make_uint(const mango::ct::Nat<Vs...> n) noexcept {
  return Int<mango::ct::Nat<>, mango::ct::Nat<Vs...>>{n};
}

} // namespace mango::rt

template <typename T, typename MIN, typename MAX>
  requires std::is_integral_v<T>
constexpr auto operator-(const T lhs,
                         const mango::rt::Int<MIN, MAX> &rhs) noexcept {
  return -(rhs - lhs);
}