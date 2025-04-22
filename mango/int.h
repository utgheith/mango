#pragma once

#include "nat.h"

namespace mango {

template <uint16_t N, Nat<N> MIN = Nat<N>{}, Nat<N> MAX = ~Nat<N>{}>
struct UInt {
  const Nat<N> abs;

  constexpr UInt(Nat<N> abs = MIN) : abs(abs) {}

  constexpr UInt<N, ~MAX, ~MIN> operator~() const { return {~abs}; }

  template <uint16_t M, Nat<M> MIN2, Nat<M> MAX2>
  constexpr auto operator+(const UInt<M, MIN2, MAX2> &rhs) const {
    return UInt<max(N, M) + 1, MIN + MIN2, MAX + MAX2>{abs + rhs.abs};
  }
};

}; // namespace mango

////////////
// Output //
////////////

template <uint16_t N, mango::Nat<N> MIN, mango::Nat<N> MAX>
inline std::ostream &operator<<(std::ostream &os,
                                const mango::UInt<N, MIN, MAX> &i) {
  os << i.abs;
  return os;
}
