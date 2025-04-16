#include "mango.h"

constexpr Nat<3> x{5};
constexpr Nat<64> y{77};
constexpr Nat<0> zero;

int main() {
  printf("hello\n");
  printf("%lu\n", sizeof(x));
  std::cout << x << std::endl;
  std::cout << zero + x << std::endl;
  std::cout << zero + x << std::endl;
  std::cout << x + y << " " << (x + y).WIDTH << std::endl;

  return 0;
}
