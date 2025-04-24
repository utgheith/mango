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

template <uint16_t N> struct Nat;

template <uint16_t N> struct Sat;

//////////////
// NatState //
//////////////

template <uint16_t N> struct NatState {
  constexpr static uint16_t WIDTH = N;
  constexpr static uint16_t SLACK = (N > 64) ? 0 : 64 - N;
  constexpr static uint64_t MASK =
      (SLACK == 0) ? ~uint64_t{0} : (uint64_t(1) << WIDTH) - 1;

  const uint64_t low;
  const Nat<safe_sub(N, 64)> high;

  constexpr NatState(const uint64_t v) noexcept;

  constexpr NatState(const Nat<safe_sub(N, 64)> &high_, const uint64_t low_)
      : low(low_ & MASK), high(high_) {}

  constexpr uint64_t get_low() const noexcept { return low; }

  constexpr const Nat<safe_sub(N, 64)> get_high() const noexcept {
    return high;
  }
};

template <> struct NatState<0> {
  constexpr static uint16_t WIDTH = 0;
  constexpr static uint16_t SLACK = 64;
  constexpr static uint64_t MASK = 0;

  constexpr NatState(const uint64_t) noexcept {}

  constexpr NatState(const Nat<0> &, const uint64_t) noexcept {}

  constexpr uint64_t get_low() const noexcept { return 0; }
  constexpr const Nat<0> get_high() const noexcept;
};

////////////
// Nat<N> //
////////////

template <uint16_t N> struct Nat : public NatState<N> {
  constexpr static uint16_t WIDTH = NatState<N>::WIDTH;
  constexpr static uint16_t SLACK = NatState<N>::SLACK;
  constexpr static uint64_t MASK = NatState<N>::MASK;

  constexpr Nat(const uint64_t v = 0) : NatState<N>(v) {}

  constexpr Nat(const Nat<safe_sub(N, 64)> &high_, const uint64_t low_)
      : NatState<N>(high_, low_) {}

  // comparison operators

  template <uint16_t M>
  constexpr Cmp cmp(const Nat<M> &rhs, const Cmp prev = Cmp::EQ) const {
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
  constexpr bool operator==(const Nat<M> &rhs) const noexcept {
    return cmp(rhs) == Cmp::EQ;
  }

  // arithmetic operators

  constexpr const Sat<N> pred() const;

  constexpr const Nat<N + 1> succ() const {
    if (this->get_low() == MASK) {
      return {Nat<safe_sub(N + 1, 64)>{this->get_high().succ()}, 0};
    } else {
      return {Nat<safe_sub(N + 1, 64)>{this->get_high()}, this->get_low() + 1};
    }
  }

  constexpr const Nat<N> operator~() const {
    if constexpr (N == 0) {
      return *this;
    } else {
      return {~this->get_high(), ~this->get_low()};
    }
  }

  template <uint16_t M>
  constexpr const Sat<max(N, M)>
  sub_with_borrow(const Nat<M> &rhs, const bool borrow) const noexcept;

  template <uint16_t M>
  constexpr const Nat<max(N, M) + 1>
  add_with_carry(const Nat<M> &rhs, const Nat<1> &carry_in) const noexcept {
    if constexpr (M > N) {
      return rhs.add_with_carry(*this, carry_in);
    } else {
      static_assert(N >= M);
      static_assert(N >= 0);

      // mod 2^64
      auto new_low = this->get_low() + rhs.get_low() + carry_in.get_low();
      if constexpr (M >= 64) {
        static_assert(~MASK == uint64_t(0));
        if (new_low < this->get_low()) {
          // carry
          auto new_high =
              this->get_high().add_with_carry(rhs.get_high(), Nat<1>{1});
          return {new_high, new_low};
        } else {
          return {this->get_high() + rhs.get_high() + new_low};
        }
      } else {
        return {new_low};
      }
    }
  }

  template <uint16_t M>
  constexpr const Nat<max(N, M) + 1>
  operator+(const Nat<M> &rhs) const noexcept {
    return add_with_carry(rhs, Nat<1>{0});
  }

  template <uint16_t M>
  constexpr const Nat<N + M> concat(const Nat<M> &rhs) const noexcept {
    if constexpr (M == 0) {
      return *this;
    } else if constexpr (M == 64) {
      return {*this, rhs.get_low()};
    } else if constexpr (M > 64) {
      return {concat(rhs.get_high()), rhs.get_low()};
    } else {
      const uint64_t new_low = (this->get_low() << M) | rhs.get_low();
      if constexpr ((N + M) <= 64) {
        return Nat<M + N>{new_low};
      } else {
        const Nat<N - 64 + M> new_high =
            this->get_high().concat(Nat<M>{this->get_low() >> (64 - M)});
        return {new_high, new_low};
      }
    }
  }
};

constexpr const Nat<0> NatState<0>::get_high() const noexcept {
  return Nat<0>{};
}

template <uint16_t N>
constexpr NatState<N>::NatState(const uint64_t v) noexcept
    : low(v & MASK), high(Nat<safe_sub(N, 64)>{}) {
  static_assert(N > 0, "NatState<N> requires N > 0");
}

/////////////
// factory //
/////////////

constexpr Nat<0> nat() { return Nat<0>{}; }

template <typename T> constexpr const auto nat(T v) {
  return Nat<sizeof(T) * 8>{static_cast<uint64_t>(v)};
}

template <typename T, typename... Ts> constexpr const auto nat(T v, Ts... vs) {
  return nat(v).concat(nat(vs...));
}

/////////
// Sat //
/////////

template <uint16_t N> struct Sat {

  const Nat<N> abs;
  const bool is_negative = false;

  constexpr Sat(const Nat<N> &abs_, bool is_negative_ = false)
      : abs(abs_), is_negative(is_negative_) {}

  constexpr const Sat<N> operator-() const noexcept {
    return Sat<N>{abs, !is_negative};
  }

  template <uint16_t M>
  constexpr const Sat<max(N, N) + 1>
  operator+(const Sat<M> &rhs) const noexcept {
    if (is_negative == rhs.is_negative) {
      return {abs + rhs.abs, is_negative};
    } else {
      if (is_negative) {
        return rhs.abs - abs;
      } else {
        return abs - rhs.abs;
      }
    }
  }
};

////////////
// Output //
////////////

template <uint16_t N>
inline std::ostream &operator<<(std::ostream &os, const Nat<N> &nat) {
  if constexpr (N > 64) {
    os << nat.get_high() << ":";
  }
  os << std::format("{:x}", nat.get_low());
  return os;
}

} // namespace mango

// comparison

template <uint16_t N, uint16_t M>
constexpr int operator<=>(const mango::Nat<N> &lhs,
                          const mango::Nat<M> &rhs) noexcept {
  return int(lhs.cmp(rhs));
}
