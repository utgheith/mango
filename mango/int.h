#pragma once

#include "nat.h"

namespace mango {

template <uint16_t N>
struct Int {
  const Nat<N> abs;
  const bool is_negative;

  constexpr Int(Nat<N> abs = {}, bool is_negative = false)
      : abs(abs), is_negative(is_negative) {}

  constexpr Int<N> operator-() const { return {abs, !is_negative}; }

  template <uint16_t M>
  constexpr Int<max(M, N) + 1> operator+(const Int<M>& rhs) const {
    if (is_negative == rhs.is_negative) {
      return {abs + rhs.abs, is_negative};
    } else {
      throw "";
    }
  }
};

////////////
// Output //
////////////

template <uint16_t N>
inline std::ostream& operator<<(std::ostream& os, const Int<N>& i) {
  if (i.is_negative) {
    os << "-(";
  }
  os << i.abs;
  if (i.is_negative) {
    os << ")";
  }
  return os;
}

}  // namespace mango
