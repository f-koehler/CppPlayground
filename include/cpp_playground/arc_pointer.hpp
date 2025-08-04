#ifndef CPPPLAYGROUND_ARC_POINTER_HPP
#define CPPPLAYGROUND_ARC_POINTER_HPP

#include <atomic>
#include <cstddef>
#include <type_traits>

namespace CppPlayground {
template <typename T> class ArcPointer {
public:
  using Type = T;

private:
  std::atomic_uint64_t m_reference_count;
  Type *m_data;

public:
  constexpr ArcPointer() noexcept;
  constexpr ArcPointer(std::nullptr_t) noexcept;
};
} // namespace CppPlayground

#endif
