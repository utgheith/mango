#pragma once

#include <cstdint>
#include <cstdio>
#include <format>
#include <iostream>
#include <string>
#include <type_traits>

template <typename T>
constexpr T max(T a, T b) {
  return a > b ? a : b;
}

template <uint16_t V>
struct Width {
  constexpr static uint16_t value = V;
};

template <typename T>
concept Big = T::value > 64;

template <typename T>
concept Small = T::value <= 64 && T::value > 0;

template <typename T>
concept Zero = T::value == 0;

template <typename T>
  requires Big<T> || Small<T> || Zero<T>
class NatImpl;

template <uint16_t N>
using Nat = NatImpl<Width<N>>;

template <Big N>
class NatImpl<N> {
 public:
  constexpr static unsigned WIDTH = (N::value < 0) ? 0 : N::value;
  constexpr static unsigned SLACK = WIDTH % 64;
  constexpr static uint64_t HIGH = (WIDTH > 64) ? WIDTH - 64 : 0;

  const uint64_t low;
  const NatImpl<Width<HIGH>> high;

  constexpr NatImpl(const uint64_t low = 0)
      : low(low << SLACK >> SLACK), high{} {}
  constexpr NatImpl(const NatImpl<Width<HIGH>> &high, uint64_t low)
      : low(low << SLACK >> SLACK), high(high) {}
};

template <Small N>
class NatImpl<N> {
 public:
  constexpr static uint16_t WIDTH = N::value;
  constexpr static uint16_t SLACK = 64 - WIDTH;

  const uint64_t low;

  constexpr NatImpl(const uint64_t l = 0) : low(l << SLACK >> SLACK) {}

  template <Zero T>
  constexpr const NatImpl<N> operator+(NatImpl<T> const &) const {
    return *this;
  }

  template <Small T>
  constexpr const Nat<max(T::value, N::value) + 1> operator+(
      NatImpl<T> const &rhs) const {
    constexpr uint16_t OUT_WIDTH = max(T::value, N::value) + 1;
    if constexpr (OUT_WIDTH <= 64) {
      return Nat<OUT_WIDTH>(low + rhs.low);
    } else {
      static_assert(false);
    }
  }
};

template <Zero N>
class NatImpl<N> {
 public:
  constexpr static uint16_t WIDTH = 0;
  constexpr NatImpl() {}

  template <typename T>
    requires Big<T> || Small<T> || Zero<T>
  constexpr const NatImpl<T> operator+(NatImpl<T> const &rhs) const {
    return rhs;
  }
};

template <typename T>
  requires Big<T> || Small<T> || Zero<T>
inline std::ostream &operator<<(std::ostream &os, const NatImpl<T> &nat) {
  if constexpr (T::value > 64) {
    os << nat.high << ":";
  }
  if constexpr (T::value > 0) {
    os << std::format("{}", nat.low);
  } else {
    os << "0";
  }
  return os;
}
