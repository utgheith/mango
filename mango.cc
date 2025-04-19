#include "mango/mango.h"

#include <iostream>

#include "mango/int.h"

using namespace mango;

constexpr Nat<3> x{5};
constexpr Nat<64> y{77};
constexpr Nat<0> zero;
constexpr Nat<1000> big{x};

template <uint16_t N>
void show(const char* msg, Nat<N> const& v) {
  std::cout << "[" << msg << "] ";
  std::cout << "Nat<" << N << ">{" << v << "}";
  std::cout << std::endl;
}

template <uint16_t N>
void show(const char* msg, Int<N> const& v) {
  std::cout << "[" << msg << "] ";
  std::cout << "Int<" << N << ">{" << v << "}";
  std::cout << std::endl;
}

void show(const char* msg, const bool v) {
  std::cout << "[" << msg << "] " << v << std::endl;
}

#define SHOW(x) show(#x, x)

int main() {
  printf("hello\n");
  SHOW(x);
  SHOW(y);
  SHOW(zero);
  SHOW(big);
  SHOW(Nat<500>{x});
  SHOW(big.succ());
  SHOW(x + y);
  SHOW(~Nat<100>{});
  SHOW(Int(x));
  SHOW(-Int(x));
  SHOW(x == y);
  SHOW(x == big);

  // std::cout << zero + x << std::endl;
  // std::cout << zero + x << std::endl;
  // std::cout << x + y << " " << (x + y).WIDTH << std::endl;

  return 0;
}
