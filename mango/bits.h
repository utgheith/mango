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

template <uint16_t N> struct Bits;

//////////////
// BitsState //
//////////////

template <uint16_t N> struct BitsState {
  constexpr static uint16_t WIDTH = N;
  constexpr static uint16_t SLACK = (N > 64) ? 0 : 64 - N;
  constexpr static uint64_t MASK =
      (SLACK == 0) ? ~uint64_t{0} : (uint64_t(1) << WIDTH) - 1;

  const uint64_t low;
  const Bits<safe_sub(N, 64)> high;

  constexpr BitsState(const uint64_t v) noexcept;

  constexpr BitsState(const Bits<safe_sub(N, 64)> &high_, const uint64_t low_)
      : low(low_ & MASK), high(high_) {}

  constexpr uint64_t get_low() const noexcept { return low; }

  constexpr const Bits<safe_sub(N, 64)> get_high() const noexcept {
    return high;
  }
};

template <> struct BitsState<0> {
  constexpr static uint16_t WIDTH = 0;
  constexpr static uint16_t SLACK = 64;
  constexpr static uint64_t MASK = 0;

  constexpr BitsState(const uint64_t) noexcept {}

  constexpr BitsState(const Bits<0> &, const uint64_t) noexcept {}

  constexpr uint64_t get_low() const noexcept { return 0; }
  constexpr const Bits<0> get_high() const noexcept;
};

////////////
// Bits<N> //
////////////

template <uint16_t N> struct Bits : public BitsState<N> {
  constexpr static uint16_t WIDTH = BitsState<N>::WIDTH;
  constexpr static uint16_t SLACK = BitsState<N>::SLACK;
  constexpr static uint64_t MASK = BitsState<N>::MASK;

  constexpr Bits(const uint64_t v = 0) : BitsState<N>(v) {}

  constexpr Bits(const Bits<safe_sub(N, 64)> &high_, const uint64_t low_)
      : BitsState<N>(high_, low_) {}

  // comparison operators

  template <uint16_t M>
  constexpr Cmp cmp(const Bits<M> &rhs, const Cmp prev = Cmp::EQ) const {
    if constexpr ((N == 0) && (M == 0)) {
      return prev;
    } else {
      if (this->get_low() == rhs.get_low()) {
        return this->get_high().cmp(rhs.get_high(), prev);
      } else if (this->get_low() > rhs.get_low()) {
        return this->get_high().cmp(rhs.get_high(), Cmp::GT);
      } else {
        return this->get_high().cmp(rhs.get_high(), Cmp::LT);
      }
    }
  }

  template <uint16_t M>
  constexpr bool operator==(const Bits<M> &rhs) const noexcept {
    return cmp(rhs) == Cmp::EQ;
  }

  // arithmetic operators

  constexpr const Bits<N + 1> succ() const {
    if (this->get_low() == MASK) {
      return {Bits<safe_sub(N + 1, 64)>{this->get_high().succ()}, 0};
    } else {
      return {Bits<safe_sub(N + 1, 64)>{this->get_high()}, this->get_low() + 1};
    }
  }

  constexpr const Bits<N> operator~() const {
    if constexpr (N == 0) {
      return *this;
    } else {
      return {~this->get_high(), ~this->get_low()};
    }
  }

  template <uint16_t M>
  constexpr const Bits<N + M> concat(const Bits<M> &rhs) const noexcept {
    if constexpr (M == 0) {
      return *this;
    } else if constexpr (M == 64) {
      return {*this, rhs.get_low()};
    } else if constexpr (M > 64) {
      return {concat(rhs.get_high()), rhs.get_low()};
    } else {
      const uint64_t new_low = (this->get_low() << M) | rhs.get_low();
      if constexpr ((N + M) <= 64) {
        return Bits<M + N>{new_low};
      } else {
        const Bits<N - 64 + M> new_high =
            this->get_high().concat(Bits<M>{this->get_low() >> (64 - M)});
        return {new_high, new_low};
      }
    }
  }
};

constexpr const Bits<0> BitsState<0>::get_high() const noexcept {
  return Bits<0>{};
}

template <uint16_t N>
constexpr BitsState<N>::BitsState(const uint64_t v) noexcept
    : low(v & MASK), high(Bits<safe_sub(N, 64)>{}) {
  static_assert(N > 0, "BitsState<N> requires N > 0");
}

/////////////
// factory //
/////////////

constexpr Bits<0> bits() { return Bits<0>{}; }

template <typename T> constexpr const auto bits(T v) {
  return Bits<sizeof(T) * 8>{static_cast<uint64_t>(v)};
}

template <typename T, typename... Ts> constexpr const auto bits(T v, Ts... vs) {
  return bits(v).concat(bits(vs...));
}

////////////
// Output //
////////////

template <uint16_t N>
inline std::ostream &operator<<(std::ostream &os, const Bits<N> &bits) {
  if constexpr (N > 64) {
    os << bits.get_high() << ":";
  }
  os << std::format("{:x}", bits.get_low());
  return os;
}

} // namespace mango

// comparison

template <uint16_t N, uint16_t M>
constexpr int operator<=>(const mango::Bits<N> &lhs,
                          const mango::Bits<M> &rhs) noexcept {
  return int(lhs.cmp(rhs));
}
