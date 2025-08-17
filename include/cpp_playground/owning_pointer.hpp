#ifndef CPPPLAYGROUND_OWNING_POINTER_HPP
#define CPPPLAYGROUND_OWNING_POINTER_HPP

#include <cpp_playground/deleter.hpp>
#include <type_traits>

namespace CppPlayground {

template <typename T, IsDeleter DeleterT = DefaultDelete<T>>
class OwningPointer {
public:
  using ElementType = std::remove_extent_t<T>;
  using Deleter = DeleterT;

private:
  [[no_unique_address]] Deleter m_deleter;
  ElementType *m_ptr;

public:
  constexpr OwningPointer() noexcept : m_deleter{}, m_ptr{nullptr} {}
  constexpr OwningPointer(std::nullptr_t) noexcept
      : m_deleter{}, m_ptr{nullptr} {}
  constexpr explicit OwningPointer(ElementType *ptr) noexcept
      : m_deleter{}, m_ptr{ptr} {}

  template <typename OtherDeleter>
  OwningPointer(const OwningPointer<T, OtherDeleter> &) = delete;

  template <typename OtherDeleter>
  constexpr OwningPointer(OwningPointer<T, OtherDeleter> &&other)
      : m_deleter{}, m_ptr{other.m_ptr} {
    other.m_ptr = nullptr;
  }

  virtual ~OwningPointer() {
    if (m_ptr != nullptr) {
      m_deleter((void *)m_ptr);
    }
  }

  template <typename OtherDeleter>
  OwningPointer &operator=(const OwningPointer<T, OtherDeleter> &) = delete;

  template <typename OtherDeleter>
  OwningPointer &operator=(OwningPointer<T, OtherDeleter> &&other) {
    if (m_ptr != nullptr) {
      m_deleter((void *)m_ptr);
      m_ptr = nullptr;
    }
    m_ptr = other.m_ptr;
    std::swap(m_ptr, other.m_ptr);
    return *this;
  }

  OwningPointer &operator=(std::nullptr_t) {
    if (m_ptr != nullptr) {
      m_deleter((void *)m_ptr);
      m_ptr = nullptr;
    }
    return *this;
  }

  constexpr ElementType *get() const noexcept { return m_ptr; }
  constexpr Deleter &getDeleter() noexcept { return m_deleter; }
  constexpr const Deleter &getDeleter() const noexcept { return m_deleter; }
};

} // namespace CppPlayground

#endif
