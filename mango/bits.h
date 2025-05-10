#pragma once

#include <cstdint>
#include <cstdio>
// #include <format>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

#include "common.h"
#include "nat.h"

namespace mango {

template <uint16_t N> struct Bits;

///////////////
// BitsState //
///////////////

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

  template <uint16_t M>
  constexpr BitsState(const BitsState<M> &rhs)
      : low(rhs.get_low() & MASK), high(rhs.get_high()) {}

  constexpr uint64_t get_low() const noexcept { return low; }
  constexpr auto get_high() const noexcept { return high; }

  constexpr uint64_t get(uint64_t i) const noexcept {
    if (i == 0) {
      return low & MASK;
    } else {
      high.get(i - 1);
    }
  }
};

//////////////////
// BitsState<0> //
//////////////////

template <> struct BitsState<0> {
  constexpr static uint16_t WIDTH = 0;
  constexpr static uint16_t SLACK = 64;
  constexpr static uint64_t MASK = 0;

  constexpr BitsState(const uint64_t) noexcept {}

  constexpr BitsState(const Bits<0> &, const uint64_t) noexcept {}

  template <uint16_t M> constexpr BitsState(const BitsState<M> &) {}

  constexpr uint64_t get(uint64_t) const noexcept { return 0; }
  constexpr uint64_t get_low() const noexcept { return 0; }
  constexpr const Bits<0> get_high() const noexcept;
};

/////////////
// Bits<N> //
/////////////

template <uint16_t N> struct Bits : public BitsState<N> {
  constexpr static uint16_t WIDTH = BitsState<N>::WIDTH;
  constexpr static uint16_t SLACK = BitsState<N>::SLACK;
  constexpr static uint64_t MASK = BitsState<N>::MASK;

  constexpr Bits() : BitsState<N>(0) {}

  template <typename T>
    requires std::is_integral_v<T>
  constexpr Bits(const T v) : BitsState<N>(uint64_t(int64_t(v))) {}

  constexpr Bits(const Bits<safe_sub(N, 64)> &high_, const uint64_t low_)
      : BitsState<N>(high_, low_) {}

  template <uint16_t M>
  constexpr Bits(const Bits<M> &rhs) : BitsState<N>(rhs) {}

  constexpr uint64_t get(const uint16_t i) const noexcept {
    if constexpr (N == 0) {
      return 0;
    } else {
      if (i == 0) {
        return this->get_low();
      } else {
        return this->get_high().get(i - 1);
      }
    }
  }

  // addition

  template <uint16_t M, uint64_t carry_in>
  consteval static uint16_t add_width() noexcept {
    if constexpr ((!carry_in) && (N == 0)) {
      return M;
    } else if constexpr ((!carry_in) && (M == 0)) {
      return N;
    } else {
      return max(M, N) + 1;
    }
  }

  template <uint16_t M, bool carry_in>
  using AddType = Bits<add_width<M, carry_in>()>;

  template <uint16_t M, uint64_t carry_in>
  constexpr AddType<M, carry_in> add(const Bits<M> &rhs) const noexcept {
    if constexpr (carry_in == 0) {
      if constexpr (M == 0) {
        return *this;
      } else if (N == 0) {
        return rhs;
      }
    }

    const auto low_left = this->get_low();
    const auto low_out = low_left + rhs.get_low() + carry_in;
    const uint64_t carry_out = (low_out < low_left) ? 1 : 0;

    if constexpr (AddType<M, carry_in>::SLACK == 0) {
      if (carry_out) {
        return {
            this->get_high().template add<safe_sub(M, 64), 1>(rhs.get_high()),
            low_out};
      }
    }
    return {this->get_high().template add<safe_sub(M, 64), 0>(rhs.get_high()),
            low_out};
  }

  template <uint16_t Shift>
  constexpr Bits<safe_sub(N, Shift)> shr() const noexcept {
    const auto l = this->get_low();
    const auto h = this->get_high();

    if constexpr (Shift == 0) {
      return *this;
    } else if constexpr (Shift >= N) {
      return Bits<0>{};
    } else if constexpr (N <= 64) {
      return {l >> Shift};
    } else if constexpr (Shift >= 64) {
      return h.template shr<Shift - 64>();
    } else {
      static_assert(Shift < 64);
      static_assert(N > 64);
      return {h.template shr<Shift>(),
              (h.get_low() << (64 - Shift)) | (l >> Shift)};
    }
  }

  template <uint16_t High, uint64_t Low>
    requires(High >= Low) && (High < N)
  constexpr Bits<High - Low + 1> extract() const noexcept {
    if constexpr (Low > 64) {
      return this->get_high().template extract<High - 64, Low - 64>();
    } else {
      return {this->shr<Low>()};
    }
  }

  constexpr bool is_signed() const noexcept {
    if constexpr (N == 0)
      return false;
    else
      return extract<N - 1, N - 1>().low == 1;
  }

  template <uint16_t M>
    requires(M <= N)
  constexpr Bits<M> trim() const noexcept {
    return Bits<M>{*this};
  }

  template <uint16_t M>
    requires(M >= N)
  constexpr Bits<M> zero_extend() const noexcept {
    return Bits<M>{*this};
  }

  template <uint16_t M>
    requires(M >= N)
  constexpr Bits<M> sign_extend() const noexcept {
#if 1
    if constexpr (M <= N) {
      return {*this};
    } else if constexpr (N > 64) {
      return {this->get_high().template sign_extend<M - 64>(), this->get_low()};
    } else {
      const auto signed_low =
          (int64_t(this->get_low()) << (64 - N)) >> (64 - N);
      if (signed_low < 0) {
        if constexpr (M <= 64) {
          return {uint64_t(signed_low)};
        } else {
          return {~Bits<M - 64>{0}, uint64_t(signed_low)};
        }
      } else {
        if constexpr (M <= 64) {
          return {this->get_low()};
        } else {
          return {Bits<M - 64>{0}, this->get_low()};
        }
      }
    }
#else
    // This is correct but confuses g++ and makes it produce inefficient code
    if (is_signed()) {
      return (~Bits<M - N>{0}).concat(*this);
    } else {
      return zero_extend<M>();
    }
#endif
  }

  template <uint16_t M>
  constexpr auto operator+(const Bits<M> &rhs) const noexcept {
    return add<M, 0>(rhs);
  }

  template <uint16_t M>
  constexpr Bits<max(M, N) + 1> operator-(const Bits<M> &rhs) const noexcept {
    if constexpr (M > N) {
      return this->template sign_extend<M + 1>() +
             ~(rhs.template sign_extend<M + 1>()) + Bits<1>{1};
    } else {
      return this->template sign_extend<N + 1>() +
             ~(rhs.template sign_extend<N + 1>()) + Bits<1>{1};
    }
  }

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

  template <uint16_t F>
  constexpr const Bits<uint16_t(mango::safe_max(uint16_t(F + uint16_t(1)), N))>
  flip_bit() const noexcept {
    if constexpr (F < 64) {
      const auto mask = uint64_t(1) << F;
      return {this->get_high(), this->get_low() ^ mask};
    } else {
      return {this->get_high().template flip_bit<safe_sub(F, 64)>(),
              this->get_low()};
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

/////////////////////
// type conversion //
/////////////////////

template <uint64_t... Vs>
constexpr Bits<Nat<Vs...>{}.bit_size()> to_bits(const Nat<Vs...> n) noexcept {
  if constexpr (sizeof...(Vs) == 0) {
    return Bits<0>{};
  } else {
    return {to_bits(n.high()), n.low};
  }
}

/////////////
// factory //
/////////////

constexpr Bits<0> bits() { return Bits<0>{}; }

template <typename T> constexpr auto bits(T v) {
  return Bits<sizeof(T) * 8>{static_cast<uint64_t>(v)};
}

template <typename T, typename... Ts> constexpr auto bits(T v, Ts... vs) {
  return bits(v).concat(bits(vs...));
}

} // namespace mango

////////////
// Output //
////////////

template <uint16_t N>
inline std::ostream &operator<<(std::ostream &os, const mango::Bits<N> &bits) {
  if constexpr (N > 64) {
    os << bits.get_high() << ":";
  }
  os << bits.get_low();
  return os;
}

// comparison

template <uint16_t N, uint16_t M>
constexpr int operator<=>(const mango::Bits<N> &lhs,
                          const mango::Bits<M> &rhs) noexcept {
  return int(lhs.cmp(rhs));
}
