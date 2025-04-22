#pragma once

#include <cstdint>
#include <cstdio>
#include <format>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

#include "mango.h"

namespace mango {

template <uint16_t N>
struct Nat;

template <uint16_t N>
struct Int;

////////////
// Nat<0> //
////////////

template <>
struct Nat<0> {
  constexpr static uint16_t WIDTH = 0;
  constexpr static uint16_t SLACK = 64;
  constexpr static uint16_t MASK = 0;
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
  constexpr const Nat<M> operator+(const Nat<M>& rhs) const {
    return rhs;
  }

  constexpr const Nat<0> operator~() const { return *this; }

  template <uint16_t M>
  constexpr bool operator==(const Nat<M>& rhs) const {
    if constexpr (M == 0) return true;
    if (rhs.low != 0) return false;
    return *this == rhs.upper();
  }

  template <uint16_t M>
  constexpr const Nat<M + 1> add_with_carry(const Nat<M>& rhs,
                                            const Nat<1>& carry_in) const;
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
  constexpr static uint64_t MASK =
      (SLACK == 0) ? ~uint64_t{0} : (uint64_t(1) << WIDTH) - 1;

  constexpr const Nat<safe_sub(N, 64)> upper() const { return high; }

  constexpr Nat(const uint64_t v = 0) : low(v << SLACK >> SLACK) {}

  constexpr Nat(const Nat<safe_sub(N, 64)>& high, const uint64_t low)
      : low(low << SLACK >> SLACK), high(high) {}

  template <uint16_t M>
  explicit constexpr Nat(Nat<M> const& src)
      : low(src.low << SLACK >> SLACK), high(src.upper()) {}

  constexpr const Int<N> pred() const;

  constexpr const Nat<N + 1> succ() const {
    if (low == MASK) {
      return {Nat<safe_sub(N + 1, 64)>{high.succ()}, 0};
    } else {
      return {Nat<safe_sub(N + 1, 64)>{high}, low + 1};
    }
  }

  constexpr const Nat<N> operator~() const { return {~high, ~low}; }

  template <uint16_t M>
  constexpr Cmp cmp(const Nat<M>& rhs, const Cmp prev = Cmp::EQ) const {
    if (low == rhs.low) {
      return high.cmp(rhs.upper(), prev);
    } else if (low > rhs.low) {
      return high.cmp(rhs.upper(), Cmp::GT);
    } else {
      return high.cmp(rhs.upper(), Cmp::LT);
    }
  }

  template <uint16_t M>
  constexpr const Nat<N> sub_with_borrow(const Nat<M>& rhs,
                                         const bool borrow) const {
    return {high.sub_with_borrow(rhs.upper(),
                                 borrow ? low <= rhs.low : low < rhs.low),
            (low - (borrow ? 1 : 0)) - rhs.low};
  }

  template <uint16_t M>
  constexpr const Nat<max(N, M) + 1> add_with_carry(
      const Nat<M>& rhs, const Nat<1>& carry_in) const {
    auto new_low = (low + rhs.low + carry_in.low) & Nat<max(N, M) + 1>::MASK;
    auto carry_out = (new_low < low || new_low < rhs.low);

    if (carry_out) {
      // no carry
      return {high + rhs.upper(), low + rhs.low};
    } else {
      // carry
      return {high.add_with_carry(rhs.upper(), Nat<1>{1}), new_low};
    }
  }

  template <uint16_t M>
  constexpr const Nat<max(N, M) + 1> operator+(const Nat<M>& rhs) const {
    return add_with_carry(rhs, Nat<1>{0});
  }

  template <uint16_t M>
  constexpr bool operator==(const Nat<M>& rhs) const {
    if (low != rhs.low) return false;
    return high == rhs.upper();
  }
};

constexpr const Nat<1> Nat<0>::succ() const { return Nat<1>{1}; }

template <uint16_t M>
constexpr const Nat<M + 1> Nat<0>::add_with_carry(
    const Nat<M>& rhs, const Nat<1>& carry_in) const {
  if (carry_in.low == 0) {
    return Nat<M + 1>{rhs};
  } else {
    return rhs.succ();
  }
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
