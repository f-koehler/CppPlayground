#ifndef CPPPLAYGROUND_COMPILE_TIME_GCD_HPP
#define CPPPLAYGROUND_COMPILE_TIME_GCD_HPP

#include <cstdint>

namespace CppPlayground {

template <uint64_t a, uint64_t b>
static constexpr uint64_t compile_time_gcd = compile_time_gcd<b, a % b>;

template <uint64_t a> static constexpr uint64_t compile_time_gcd<a, 0> = a;

namespace Detail {
template <int64_t a, int64_t b> static consteval uint64_t handle_gcd_signs() {
  if constexpr (a < 0) {
    if constexpr (b < 0) {
      return compile_time_gcd<static_cast<uint64_t>(-a),
                              static_cast<uint64_t>(-b)>;
    }
    return compile_time_gcd<static_cast<uint64_t>(-a),
                            static_cast<uint64_t>(b)>;
  }
  if constexpr (b < 0) {
    return compile_time_gcd<static_cast<uint64_t>(a),
                            static_cast<uint64_t>(-b)>;
  }
  return compile_time_gcd<static_cast<uint64_t>(a), static_cast<uint64_t>(b)>;
}
} // namespace Detail

template <int64_t a, int64_t b>
static constexpr uint64_t compile_time_gcd_signed =
    Detail::handle_gcd_signs<a, b>();

} // namespace CppPlayground

#endif
