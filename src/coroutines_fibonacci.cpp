#include <cstdint>
#include <generator>
#include <limits>
#include <print>
#include <type_traits>
#include <utility>

template <typename T>
[[nodiscard]] constexpr T fibonacci_limit() noexcept
  requires(std::is_integral_v<T>)
{
  T current = 0;
  T next = 1;
  while (true) {
    if ((std::numeric_limits<T>::max() - next) < current) {
      return next;
    }
    next += std::exchange(current, next);
  }
}

template <typename T>
std::generator<T> fibonacci_numbers()
  requires(std::is_integral_v<T>)
{
  static constexpr auto limit = fibonacci_limit<T>();

  T current = 0;
  T next = 1;
  while (true) {
    co_yield current;
    if (current == limit) {
      co_return;
    }
    next += std::exchange(current, next);
  }
}

int main() {
  for (auto x : fibonacci_numbers<uint64_t>()) {
    std::println("{}", x);
  }
}
