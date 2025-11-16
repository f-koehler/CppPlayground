#ifndef CPPPLAYGROUND_DELETER_HPP
#define CPPPLAYGROUND_DELETER_HPP

#include <concepts>
#include <utility>
#include <type_traits>

namespace CppPlayground {

namespace Detail {
template <typename Scalar> struct ScalarDeleter {
  using Pointer = Scalar *;
  constexpr void operator()(Pointer ptr) const noexcept { delete ptr; }
};
template <typename Scalar> struct ArrayDeleter {
  using Pointer = Scalar *;
  constexpr void operator()(Pointer ptr) const noexcept { delete[] ptr; }
};
} // namespace Detail

template <typename T>
using DefaultDeleter =
    std::conditional_t<std::is_array_v<T>, Detail::ArrayDeleter<std::remove_extent_t<T>>,
                       Detail::ScalarDeleter<T>>;

} // namespace CppPlayground

#endif
