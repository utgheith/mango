#pragma once

#include <cstdint>
#include <cstdio>
#include <format>
#include <iostream>
#include <string>
#include <type_traits>

namespace mango {

typedef unsigned int uint128_t __attribute__((mode(TI)));

constexpr uint16_t safe_sub(uint16_t a, uint16_t b) {
  return (a < b) ? 0 : a - b;
}

template <uint16_t N>
struct Nat;

////////////
// Nat<0> //
////////////

template <>
struct Nat<0> {
  constexpr static uint64_t low = 0;

  constexpr Nat() {}

  template <uint16_t M>
  explicit constexpr Nat(Nat<M> const &) {}

  template <uint16_t M>
  constexpr operator const Nat<M>() const {
    return Nat<M>{};
  }

  constexpr const Nat<0> upper() const { return Nat<0>{}; }

  constexpr const Nat<1> inc() const;
};

////////////
// Nat<N> //
////////////

template <uint16_t N>
struct Nat {
  const uint64_t low;
  const Nat<safe_sub(N, 64)> high;
  constexpr static uint16_t WIDTH = N;
  constexpr static uint16_t SLACK = (N > 64) ? 0 : 64 - N;

  constexpr const Nat<safe_sub(N, 64)> upper() const { return high; }

  constexpr Nat(const uint64_t v = 0) : low(v << SLACK >> SLACK) {}

  constexpr Nat(const Nat<safe_sub(N, 64)> &high, const uint64_t low)
      : low(low << SLACK >> SLACK), high(high) {}

  template <uint16_t M>
  explicit constexpr Nat(Nat<M> const &src)
      : low(src.low << SLACK >> SLACK), high(src.upper()) {}

  constexpr const Nat<N + 1> inc() const {
    const auto low1 = low + 1;
    if constexpr (N < 64) {
      return Nat<N + 1>{low1};
    } else {
      const auto low128 = uint128_t(low) + 1;
      if (uint64_t(low128) == low1) {
        return Nat<N + 1>{Nat<N - 63>{high}, low1};
      } else {
        static_assert(SLACK == 0);
        return Nat<N + 1>{high.inc(), low1};
      }
    }
  }
};

constexpr const Nat<1> Nat<0>::inc() const { return Nat<1>{1}; }

////////////
// Output //
////////////

template <uint16_t N>
inline std::ostream &operator<<(std::ostream &os, const Nat<N> &nat) {
  if constexpr (N > 64) {
    os << nat.high << ":";
  }
  os << nat.low;
  return os;
}

}  // namespace mango
