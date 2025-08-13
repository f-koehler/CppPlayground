#ifndef CPPPLAYGROUND_ARC_POINTER_HPP
#define CPPPLAYGROUND_ARC_POINTER_HPP

#include <atomic>
#include <cstddef>
#include <memory>
#include <type_traits>

namespace CppPlayground {

namespace Detail {
struct ControlBlock {
  void *m_ptr;
  // store allocator and deleter with erase type
  void *(*m_allocator)(std::size_t n);
  void (*m_deleter)(void *);
  std::atomic_uint64_t m_num_shared;
  std::atomic_uint64_t m_num_weak;

  ControlBlock(void *ptr, void *(*allocator)(std::size_t n),
               void (*deleter)(void *))
      : m_ptr(ptr), m_allocator(allocator), m_deleter(deleter), m_num_shared{1},
        m_num_weak{0} {}

  void add_ref() { ++m_num_shared; }
  void add_weak_ref() { ++m_num_weak; }
  void release_ref() {
    if ((--m_num_shared) == 0) {
      m_deleter(m_ptr);
      delete this;
    }
  }
};
}; // namespace Detail

template <typename T> class ArcPointer {
public:
  using ElementType = std::remove_extent_t<T>;

private:
  Detail::ControlBlock *m_control_block;
  ElementType *m_ptr;

public:
  constexpr ArcPointer() noexcept
      : m_control_block(new Detail::ControlBlock(
            nullptr,
            [](const std::size_t n) {
              return (void *)std::allocator<ElementType>{}.allocate(n);
            },
            [](void *ptr) { delete (ElementType *)ptr; })) {}
  constexpr ArcPointer(std::nullptr_t) noexcept;
};
} // namespace CppPlayground

#endif
