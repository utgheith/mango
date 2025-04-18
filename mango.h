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

template <typename T>
constexpr T max(T a, T b) {
  return (a > b) ? a : b;
}

template <uint16_t N>
struct Nat;

/////////////
// NatBase //
/////////////

struct NatBase {};

////////////
// Nat<0> //
////////////

template <>
struct Nat<0> : public NatBase {
  constexpr static uint64_t low = 0;

  constexpr Nat() {}

  template <uint16_t M>
  explicit constexpr Nat(Nat<M> const&) {}

  template <uint16_t M>
  constexpr operator const Nat<M>() const {
    return Nat<M>{};
  }

  constexpr const Nat<0> upper() const { return Nat<0>{}; }

  constexpr const Nat<1> succ() const;

  template <uint16_t M>
  constexpr const Nat<M + 1> add_with_carry(const Nat<M>& rhs,
                                            const Nat<1>& carry) const;

  template <uint16_t M>
  constexpr const Nat<M + 1> operator+(const Nat<M>& rhs) const;

  constexpr const Nat<0> operator~() const { return *this; }
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

  constexpr Nat(const Nat<safe_sub(N, 64)>& high, const uint64_t low)
      : low(low << SLACK >> SLACK), high(high) {}

  template <uint16_t M>
  explicit constexpr Nat(Nat<M> const& src)
      : low(src.low << SLACK >> SLACK), high(src.upper()) {}

  constexpr const Nat<N + 1> succ() const {
    const auto low1 = low + 1;
    if constexpr (N < 64) {
      return Nat<N + 1>{low1};
    } else {
      const auto low128 = uint128_t(low) + 1;
      if (uint64_t(low128) == low1) {
        return Nat<N + 1>{Nat<N - 63>{high}, low1};
      } else {
        static_assert(SLACK == 0);
        return Nat<N + 1>{high.succ(), low1};
      }
    }
  }

  constexpr const Nat<N> operator~() const { return {~high, ~low}; }

  template <uint16_t M>
  constexpr const Nat<max(N, M) + 1> add_with_carry(const Nat<M>& rhs,
                                                    const Nat<1>& carry) const {
    const auto low128 =
        uint128_t(low) + uint128_t(rhs.low) + uint128_t(carry.low);
    const auto low64 = uint64_t(low128);

    if (low128 != uint128_t(low64)) {
      // carry
      return {high.add_with_carry(rhs.upper(), Nat<1>{1}), low64};
    } else {
      return {high + rhs.upper(), low64};
    }
  }

  template <uint16_t M>
  constexpr const Nat<max(N, M) + 1> operator+(const Nat<M>& rhs) const {
    return add_with_carry(rhs, Nat<1>{0});
  }
};

constexpr const Nat<1> Nat<0>::succ() const { return Nat<1>{1}; }

template <uint16_t M>
constexpr const Nat<M + 1> Nat<0>::add_with_carry(const Nat<M>& rhs,
                                                  const Nat<1>& carry) const {
  if (carry.low == 0) {
    return Nat<M + 1>{rhs};
  } else {
    return rhs.succ();
  }
}

template <uint16_t M>
constexpr const Nat<M + 1> Nat<0>::operator+(const Nat<M>& rhs) const {
  return add_with_carry(rhs, Nat<1>{0});
}

////////////
// Output //
////////////

template <uint16_t N>
inline std::ostream& operator<<(std::ostream& os, const Nat<N>& nat) {
  if constexpr (N > 64) {
    os << nat.high << ":";
  }
  os << std::format("{:x}", nat.low);
  return os;
}

}  // namespace mango
