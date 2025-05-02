#pragma once

#include <cassert>

namespace mango::ct {

// general case //
template <uint64_t... Vs> struct Nat; // Little endian
template <uint64_t... Vs> struct Neg;

/***************************/
/* special case: not empty */
/***************************/

template <uint64_t Low, uint64_t... High> struct Nat<Low, High...> {

  constexpr static uint64_t low = Low;

  consteval static const Nat<High...> high() noexcept { return {}; }

  consteval uint64_t bit_size() const noexcept {
    const auto t = Nat<High...>{}.bit_size();

    if (t == 0) {
      return 64 - clz(low);
    } else {
      return t + 64;
    }
  }

  consteval static uint64_t get(uint64_t i) noexcept {
    if (i == 0) {
      return low;
    } else {
      return high().get(i - 1);
    }
  }

  consteval bool is_zero() const noexcept {
    return Low == 0 && Nat<High...>{}.is_zero();
  }

  template <uint64_t new_low>
  consteval const Nat<new_low, Low, High...> inject_right() const noexcept {
    return {};
  }

  consteval auto succ() const noexcept {
    if constexpr (low == ~uint64_t{0}) {
      return Nat<High...>{}.succ().template inject_right<0>();
    } else {
      return Nat<low + 1, High...>{};
    }
  }

  ///////////////// Nat<...>::shift_left ////////////////

  template <uint64_t... Rs>
  consteval auto operator<<(const Nat<Rs...> rhs) const noexcept {
    if constexpr (rhs.is_zero()) {
      return *this;
    } else {
      if constexpr (rhs >= Nat<64>{}) {
        return Nat<0, Low, High...>{} << (rhs - Nat<64>{});
      } else {
        static_assert(rhs.low < 64);
        const auto new_high =
            (high() << Nat<rhs.low>{}) + Nat<(low >> (64 - rhs.low))>{};
        return new_high.template inject_right<Low << rhs.low>();
      }
    }
  }

  ///////////////// Nat<...>::addition ////////////////

  template <uint64_t... Rs>
  consteval auto add(const Nat<Rs...> rhs) const noexcept {
    const auto new_low = low + rhs.low;
    const auto new_carry = (new_low < low) || (new_low < rhs.low);

    if constexpr (new_carry) {
      const auto new_high = high().add(rhs.high().succ());
      return new_high.template inject_right<new_low>();
    } else {
      const auto new_high = high().add(rhs.high());
      return new_high.template inject_right<new_low>();
    }
  }

  ///////////////// Nat<...>::subtraction ////////////////

  template <uint64_t... Rs>
  consteval auto sub(const Nat<Rs...> rhs) const noexcept {
    if constexpr (low < rhs.low) {
      // need to borrow from the high part
      const auto new_high = high().sub(rhs.high().succ());
      return new_high
          .template inject_right<(~uint64_t(0) - rhs.low) + 1 + low>();
    } else {
      // no borrow needed
      const auto new_high = high().sub(rhs.high());
      return new_high.template inject_right<low - rhs.low>();
    }
  }

  //////////////// Nat<...>::comparison ////////////////

  template <uint64_t... Rs>
  consteval Cmp cmp(const Nat<Rs...> rhs,
                    const Cmp prev = Cmp::EQ) const noexcept {

    if constexpr (low == rhs.low) {
      return high().cmp(rhs.high(), prev);
    } else if constexpr (low > rhs.low) {
      return high().cmp(rhs.high(), Cmp::GT);
    } else {
      return high().cmp(rhs.high(), Cmp::LT);
    }
  }

  template <uint64_t... Rs>
  consteval Cmp cmp(const Neg<Rs...> rhs) const noexcept;

  template <typename T>
    requires std::is_integral_v<T>
  consteval Cmp cmp(const T rhs) const noexcept {
    if constexpr (std::is_signed_v<T>) {
      if (rhs < 0) {
        return Cmp::GT;
      } else {
        return cmp(std::make_unsigned_t<T>(rhs));
      }
    } else {
      if constexpr (high().is_zero()) {
        if (low == rhs) {
          return Cmp::EQ;
        } else if (low < rhs) {
          return Cmp::LT;
        } else {
          return Cmp::GT;
        }
      } else {
        return Cmp::GT;
      }
    }
  }

  template <typename Rhs>
  consteval bool operator==(const Rhs rhs) const noexcept {
    return cmp(rhs) == Cmp::EQ;
  }

  template <typename Rhs>
  consteval bool operator>(const Rhs rhs) const noexcept {
    return cmp(rhs) == Cmp::GT;
  }

  template <typename Rhs>
  consteval bool operator<(const Rhs rhs) const noexcept {
    return cmp(rhs) == Cmp::LT;
  }

  template <typename Rhs>
  consteval bool operator<=(const Rhs rhs) const noexcept {
    return cmp(rhs) != Cmp::GT;
  }

  template <typename Rhs>
  consteval bool operator>=(const Rhs rhs) const noexcept {
    return cmp(rhs) != Cmp::LT;
  }
};

/***********************/
/* special case: empty */
/***********************/

template <> struct Nat<> {
  constexpr static uint64_t low = 0;

  consteval static const Nat<> high() noexcept { return {}; }

  consteval static uint64_t get(const uint64_t) noexcept { return 0; }

  consteval uint64_t bit_size() const noexcept { return 0; }

  consteval bool is_zero() const noexcept { return true; }

  consteval const Nat<1> succ() const noexcept { return {}; }

  template <uint64_t new_low>
  consteval const Nat<new_low> inject_right() const noexcept {
    return {};
  }

  ///////////////// Shift left /////////////////////

  template <uint64_t... Rs>
  consteval auto operator<<(const Nat<Rs...>) const noexcept {
    return *this;
  }

  ///////////////// Nat<>::addition ////////////////

  template <uint64_t... Rs>
  consteval auto add(const Nat<Rs...> rhs) const noexcept {
    return rhs;
  }

  ///////////////// Nat<>::subtraction ////////////////

  template <uint64_t... Rs>
  consteval Nat<> sub(const Nat<Rs...> rhs) const noexcept {
    static_assert(rhs.is_zero(),
                  "Cannot subtract from zero unless rhs is also zero");
    return {};
  }

  ///////////////// Nat<>::comparison ////////////////

  template <uint64_t... Rs>
  consteval Cmp cmp(const Nat<Rs...> rhs,
                    const Cmp prev = Cmp::EQ) const noexcept {
    if (rhs.is_zero()) {
      return prev;
    } else {
      return Cmp::LT;
    }
  }

  consteval Cmp cmp(const uint64_t rhs) const noexcept {
    if (rhs == 0) {
      return Cmp::EQ;
    } else {
      return Cmp::LT;
    }
  }

  consteval Cmp cmp(const int64_t rhs) const noexcept {
    if (rhs < 0) {
      return Cmp::GT;
    } else {
      return cmp(static_cast<uint64_t>(rhs));
    }
  }

  template <uint64_t... Rs>
  consteval bool operator==(const Nat<Rs...> rhs) const noexcept {
    return cmp(rhs) == Cmp::EQ;
  }

  template <uint64_t... Rs>
  consteval bool operator<=(const Nat<Rs...> rhs) const noexcept {
    return cmp(rhs) != Cmp::GT;
  }

  template <uint64_t... Rs>
  consteval bool operator>(const Nat<Rs...> rhs) const noexcept {
    return cmp(rhs) == Cmp::GT;
  }
};

///////////////////////////////////////////////////////////////////////////

template <uint64_t... Vs> struct Neg {

  consteval static const Nat<Vs...> abs() noexcept { return {}; }

  consteval static uint64_t get(uint64_t i) noexcept { return abs().get(i); }

  consteval const Nat<Vs...> operator-() const noexcept { return {}; }

  consteval static bool is_zero() noexcept { return abs().is_zero(); }

  template <uint64_t... Vs2>
  consteval Cmp cmp(const Neg<Vs2...> rhs) const noexcept {
    return rhs.abs().cmp(abs());
  }

  template <uint64_t... Vs2>
  consteval Cmp cmp(const Nat<Vs2...> rhs) const noexcept {
    if constexpr (Neg<Vs...>{}.is_zero() && rhs.is_zero()) {
      return Cmp::EQ;
    } else {
      return Cmp::LT;
    }
  }

  template <typename T>
    requires std::is_integral_v<T>
  consteval Cmp cmp(const T rhs) const noexcept {
    if constexpr (std::is_signed_v<T>) {
      assert(false);
      return Cmp::EQ;
    } else {
      if (Neg<Vs...>{}.is_zero() && (rhs == 0)) {
        return Cmp::EQ;
      } else {
        return Cmp::LT;
      }
    }
  }

  template <typename Rhs>
  consteval bool operator==(const Rhs rhs) const noexcept {
    return cmp(rhs) == Cmp::EQ;
  }

  template <typename Rhs>
  consteval bool operator>=(const Rhs rhs) const noexcept {
    return cmp(rhs) != Cmp::LT;
  }

  template <typename Rhs>
  consteval bool operator<=(const Rhs rhs) const noexcept {
    return cmp(rhs) != Cmp::GT;
  }
};

template <uint64_t Low, uint64_t... High>
template <uint64_t... Rs>
consteval Cmp Nat<Low, High...>::cmp(const Neg<Rs...> rhs) const noexcept {
  if constexpr (Nat<Low, High...>{}.is_zero() && rhs.is_zero()) {
    return Cmp::EQ;
  } else
    return Cmp::GT;
}

template <uint64_t... Vs>
consteval const Neg<Vs...> operator-(const Nat<Vs...>) noexcept {
  return {};
}

template <uint64_t... Vs>
consteval auto operator+(const Nat<Vs...> lhs,
                         const decltype(lhs.low) rhs) noexcept {
  return lhs + Nat<rhs>{};
}

template <uint64_t... Vs, uint64_t... Rs>
consteval auto operator+(const Nat<Vs...> lhs, const Nat<Rs...> rhs) noexcept {
  return lhs.add(rhs);
}

template <uint64_t... Vs, uint64_t... Rs>
consteval auto operator-(const Nat<Vs...> lhs, const Nat<Rs...> rhs) noexcept {
  const auto c = lhs.cmp(rhs);
  if constexpr (c == Cmp::EQ) {
    return Nat<>{};
  } else if constexpr (c == Cmp::GT) {
    return lhs.sub(rhs);
  } else {
    return -(rhs.sub(lhs));
  }
}

template <uint64_t... Vs, uint64_t... Rs>
consteval auto operator-(const Nat<Vs...> lhs, const Neg<Rs...> rhs) noexcept {
  return lhs + rhs.abs();
}

template <uint64_t... Vs, uint64_t... Rs>
consteval auto operator-(const Neg<Vs...> lhs, const Nat<Rs...> rhs) noexcept {
  return rhs + lhs.abs();
}

template <uint64_t... Vs, uint64_t... Rs>
consteval auto operator-(const Neg<Vs...> lhs, const Neg<Rs...> rhs) noexcept {
  return rhs.abs() - lhs.abs();
}

template <uint64_t... Vs, uint64_t... Rs>
consteval auto operator+(const Nat<Vs...> lhs, const Neg<Rs...> rhs) noexcept {
  return lhs - rhs.abs();
}

template <uint64_t... Vs, uint64_t... Rs>
consteval auto operator+(const Neg<Vs...> lhs, const Nat<Rs...> rhs) noexcept {
  return rhs - lhs.abs();
}

template <uint64_t... Vs, uint64_t... Rs>
consteval auto operator+(const Neg<Vs...> lhs, const Neg<Rs...> rhs) noexcept {
  return -(lhs.abs() + rhs.abs());
}

} // namespace mango::ct

template <uint64_t... Vs>
std::ostream &operator<<(std::ostream &os, const mango::ct::Nat<Vs...>) {
  bool first = true;
  for (const auto &v : {Vs...}) {
    if (first) {
      first = false;
    } else {
      os << "_";
    }
    os << std::format("{:016x}", v);
  }
  return os;
}

template <uint64_t... Vs>
std::ostream &operator<<(std::ostream &os, const mango::ct::Neg<Vs...>) {
  os << "-" << mango::ct::Nat<Vs...>{};
  return os;
}
