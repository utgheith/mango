#pragma once

namespace mango::ct {

// general case //
template <uint64_t... Vs> struct Nat;

/***************************/
/* special case: not empty */
/***************************/

template <uint64_t... Tail, uint64_t Head> struct Nat<Head, Tail...> {

  constexpr static uint64_t low = Head;

  constexpr const Nat<Tail...> high() const noexcept { return {}; }

  constexpr bool is_zero() const noexcept {
    return Head == 0 && Nat<Tail...>{}.is_zero();
  }

  template <uint64_t new_low>
  constexpr const Nat<new_low, Head, Tail...> inject_right() const noexcept {
    return Nat<new_low, Head, Tail...>{};
  }

  constexpr const auto succ() const noexcept {
    if constexpr (low == ~uint64_t{0}) {
      return Nat<Tail...>{}.succ().template inject_right<0>();
    } else {
      return Nat<low + 1, Tail...>{};
    }
  }

  template <uint64_t... Rs>
  constexpr const auto
  add_with_carry(const Nat<Rs...> &rhs,
                 const bool carry_in = false) const noexcept {
    const auto new_low = low + rhs.low + carry_in ? 1 : 0;
    const auto new_carry = new_low < low || (new_low < rhs.low);

    return high()
        .add_with_carry(rhs.high(), new_carry)
        .template inject_right<new_low>();
  }

  template <uint64_t... Rs>
  constexpr const auto operator+(const Nat<Rs...> &rhs) const noexcept {
    return add_with_carry(rhs, false);
  }

  template <uint64_t... Rs>
  constexpr Cmp cmp(const Nat<Rs...> &rhs,
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
  constexpr bool operator==(const Nat<Rs...> &rhs) const noexcept {
    return cmp(rhs) == Cmp::EQ;
  }

  template <uint64_t... Rs>
  constexpr bool operator>(const Nat<Rs...> &rhs) const noexcept {
    return cmp(rhs) == Cmp::GT;
  }

  template <uint64_t... Rs>
  constexpr bool operator<(const Nat<Rs...> &rhs) const noexcept {
    return cmp(rhs) == Cmp::LT;
  }
};

/***********************/
/* special case: empty */
/***********************/

template <> struct Nat<> {
  constexpr static uint64_t low = 0;

  constexpr const Nat<> high() const noexcept { return {}; }

  constexpr bool is_zero() const noexcept { return true; }

  constexpr const Nat<1> succ() const noexcept { return {}; }

  template <uint64_t... Rs>
  constexpr Cmp cmp(const Nat<Rs...> &rhs,
                    const Cmp prev = Cmp::EQ) const noexcept {
    if (rhs.is_zero()) {
      return prev;
    } else {
      return Cmp::LT;
    }
  }

  template <uint64_t... Rs>
  constexpr bool operator==(const Nat<Rs...> &rhs) const noexcept {
    return cmp(rhs) == Cmp::EQ;
  }

  template <uint64_t... Rs>
  constexpr bool operator>(const Nat<Rs...> &rhs) const noexcept {
    return cmp(rhs) == Cmp::GT;
  }
};

} // namespace mango::ct

template <uint64_t... Vs>
std::ostream &operator<<(std::ostream &os, const mango::ct::Nat<Vs...> &) {
  os << "ct::Nat<";
  bool first = true;
  for (const auto &v : {Vs...}) {
    if (first) {
      first = false;
    } else {
      os << ", ";
    }
    os << v;
  }
  os << ">";
  return os;
}
