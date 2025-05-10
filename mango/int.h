#pragma once

#include <cassert>
#include <cstdint>
#include <iostream>
#include <mango/bits.h>
#include <mango/nat.h>
#include <type_traits>

namespace mango {

template <typename Min, typename Max> struct Int {
  constexpr static Min min{};
  constexpr static Max max{};
  static_assert(Min{} <= Max{}, "Min must be less than or equal to Max");
  constexpr static auto range = max - min + Nat<1>{};
  constexpr static uint64_t bitsize = range.bit_size();
  Bits<bitsize> biased_bits; // value - Min

  constexpr Int() noexcept : biased_bits{0} {}

  // TODO this can be confusing. Should it be public?
  constexpr uint64_t get(uint64_t i) const noexcept {
    return biased_bits.get(i);
  }

  constexpr Int(const Bits<bitsize> &raw_bits, const bool) noexcept
      : biased_bits{raw_bits} {}

  template <typename T>
  constexpr Int(const T value) noexcept : biased_bits{to_bits(value - min)} {
    assert(min <= value);
    assert(max >= value);
  }

  template <typename Min2, typename Max2>
  constexpr const Int<decltype(Min{} + Min2{}), decltype(Max{} + Max2{})>
  operator+(const Int<Min2, Max2> &other) const noexcept {
    return {biased_bits + other.biased_bits, true};
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
    os << value.get(value.LEN - i);
  }

  return os;
}

template <uint64_t... Vs>
constexpr Int<Nat<Vs...>, Nat<Vs...>> UInt(const Nat<Vs...>) noexcept {
  return Int<Nat<Vs...>, Nat<Vs...>>{Bits<0>{}, true};
}

template <uint16_t N> constexpr UnsignedInt<N> UInt(const Bits<N> &b) noexcept {
  return {b, true};
}


  
} // namespace mango

template <typename T, typename MIN, typename MAX>
  requires std::is_integral_v<T>
constexpr auto operator-(const T lhs,
                         const mango::Int<MIN, MAX> &rhs) noexcept {
  return -(rhs - lhs);
}
