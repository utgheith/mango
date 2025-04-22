#pragma once

#include <iostream>

#include "int.h"

namespace mango {

template <uint16_t A, Int<A> MIN, uint16_t B, Int<B> MAX> struct RangedInt {
  // static_assert(MIN <= MAX, "MIN must be less than or equal to MAX");

  Int<max(A, B)> v;

  template <uint16_t M> constexpr RangedInt(const Nat<M> &m) : v(Int{m}) {}

  template <uint16_t M> constexpr RangedInt(const Int<M> &m) : v(m) {}

  template <uint16_t A2, Int<A2> MIN2, uint16_t B2, Int<B2> MAX2>
  constexpr RangedInt<max(A, A2) + 1, MIN + MIN2, max(B, B2) + 1, MAX + MAX2>
  operator+(const RangedInt<A2, MIN2, B2, MAX2> &rhs) const {
    return {v + rhs.v};
  }
};

template <uint16_t N>
constexpr RangedInt<N, Int<N>{}, N, ~Int<N>{}> unsignedInt(Nat<N> n) {
  return {n};
}

} // namespace mango

template <uint16_t A, mango::Int<A> MIN, uint16_t B, mango::Int<B> MAX>
std::ostream &operator<<(std::ostream &os,
                         const mango::RangedInt<A, MIN, B, MAX> &ri) {
  return os << "RangedInt<" << A << ", " << MIN << ", " << B << ", " << MAX
            << ">{" << ri.v << "}";
}
