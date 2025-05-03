#pragma once

#include <cassert>
#include <cstdint>
#include <iostream>
#include <mango/nat.h>
#include <type_traits>

namespace mango {

template <typename Min, typename Max> struct Int {
  constexpr static Min min{};
  constexpr static Max max{};
  static_assert(Min{} <= Max{}, "Min must be less than or equal to Max");
  constexpr static auto range = max - min + Nat<1>{};
  constexpr static uint64_t bitsize = range.bit_size();
  constexpr static uint64_t LEN =
      (bitsize + 63) / 64; // Number of uint64_t words needed
  uint64_t state[LEN];

  constexpr Int() noexcept {
    for (uint64_t i = 0; i < LEN; ++i) {
      state[i] = 0;
    }
  }

  constexpr uint64_t get(const uint64_t i) const noexcept {
    return (i < LEN) ? state[i] : 0;
  }

  template <typename T> constexpr Int(const T value) noexcept {
    assert(min <= value);
    assert(max >= value);
    auto diff = value - min;
    for (uint64_t i = 0; i < LEN; ++i) {
      state[i] = diff.get(i);
    }
  }

  template <typename A, typename B>
  constexpr Int(const A &lhs, const B &rhs) noexcept {
    static_assert(
        (bitsize - ((A::bitsize > B::bitsize) ? A::bitsize : B::bitsize)) < 2);
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

///////////////// UnsignedInt /////////////////////

template <uint16_t N>
using UnsignedInt = Int<Nat<>, decltype((Nat<1>{} << Nat<N>{}) - Nat<1>{})>;

template <uint64_t N>
struct SignedInt : Int<decltype(-(Nat<1>{} << Nat<(N - 1)>{})),
                       decltype((Nat<1>{} << Nat<(N - 1)>{}) - Nat<1>{})> {};

template <> struct SignedInt<0> : Int<Nat<>, Nat<>> {};

template <typename Min, typename Max>
std::ostream &operator<<(std::ostream &os, const Int<Min, Max> &value) {
  os << "Int<" << value.min << ", " << value.max << "> = ";
  for (uint64_t i = 0; i < value.LEN; ++i) {
    os << std::format("{:016X}", value.get(value.LEN - i));
  }

  return os;
}

template <uint64_t... Vs>
constexpr Int<Nat<>, Nat<Vs...>> UInt(const Nat<Vs...> n) noexcept {
  return Int<Nat<>, Nat<Vs...>>{n};
}

} // namespace mango

template <typename T, typename MIN, typename MAX>
  requires std::is_integral_v<T>
constexpr auto operator-(const T lhs,
                         const mango::Int<MIN, MAX> &rhs) noexcept {
  return -(rhs - lhs);
}
