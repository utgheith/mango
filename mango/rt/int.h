#pragma once

#include <cstdint>
#include <mango/ct/nat.h>

namespace mango::rt {

template <typename Min, typename Max> struct Int {
  constexpr static Min min{};
  constexpr static Max max{};
  static_assert(min <= max, "Min must be less than or equal to Max");
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
    static_assert(value >= min);
    static_assert(value <= max);
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
    static_assert(bitsize ==
                  ((lhs.bitsize > rhs.bitsize) ? lhs.bitsize : rhs.bitsize) +
                      1);
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
} // namespace mango::rt