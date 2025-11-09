#ifndef CPPPLAYGROUND_FIXED_CAPACITY_VECTOR_HPP
#define CPPPLAYGROUND_FIXED_CAPACITY_VECTOR_HPP

#include <cstddef>
#include <stdexcept>
#include <type_traits>

namespace CppPlayground {
template <typename T, std::size_t Capacity_> class FixedCapacityVector {
public:
  using SizeType = std::size_t;
  using ValueType = T;
  using ReferenceType = T &;
  using ConstReferenceType = const T &;

  static constexpr SizeType Capacity = Capacity_;
  static constexpr SizeType ElementSize = sizeof(ValueType);

private:
  alignas(ValueType) unsigned char m_buffer[Capacity * ElementSize] = {0U};
  T *m_data = reinterpret_cast<T *>(m_buffer);
  SizeType m_size = 0UL;

public:
  constexpr FixedCapacityVector() noexcept = default;

  template <SizeType OtherCapacity>
  constexpr FixedCapacityVector(
      const FixedCapacityVector<T, OtherCapacity>
          &other) noexcept(std::is_nothrow_copy_constructible_v<ValueType>);

  constexpr FixedCapacityVector(FixedCapacityVector &&other) noexcept(
      std::is_nothrow_move_constructible_v<ValueType>);

  constexpr ~FixedCapacityVector();

  template <SizeType OtherCapacity>
  constexpr FixedCapacityVector &
  operator=(const FixedCapacityVector<ValueType, OtherCapacity>
                &other) noexcept(std::is_nothrow_copy_constructible_v<T>);
  constexpr FixedCapacityVector &
  operator=(FixedCapacityVector &&other) noexcept(
      std::is_nothrow_move_constructible_v<ValueType>);

  [[nodiscard]] constexpr SizeType size() const noexcept;
  [[nodiscard]] constexpr bool is_empty() const noexcept;
  [[nodiscard]] constexpr bool is_full() const noexcept;
  [[nodiscard]] constexpr ConstReferenceType front() const;
  [[nodiscard]] constexpr ConstReferenceType back() const;
  [[nodiscard]] constexpr ReferenceType front();
  [[nodiscard]] constexpr ReferenceType back();
  [[nodiscard]] constexpr ReferenceType at(SizeType index);
  [[nodiscard]] constexpr ConstReferenceType at(SizeType index) const;
  constexpr void clear() noexcept;
  constexpr void push_back(const T &value);
  constexpr void push_back(T &&value);
  constexpr void pop_back();
};

template <typename T, std::size_t C>
template <std::size_t OtherCapacity>
constexpr FixedCapacityVector<T, C>::FixedCapacityVector(
    const FixedCapacityVector<T, OtherCapacity>
        &other) noexcept(std::is_nothrow_copy_constructible_v<T>) {
  if (other.m_size > C) {
    throw std::length_error("FixedCapacityVector: Attempt to copy from a "
                            "vector with more elements than capacity");
  }
  for (SizeType i = 0; i < other.m_size; ++i) {
    // use placement new operator to copy construct objects at the right place
    new (&m_data[i]) T(other.m_data[i]);
    ++m_size;
  }
}

template <typename T, std::size_t C>
constexpr FixedCapacityVector<T, C>::FixedCapacityVector(
    FixedCapacityVector<T, C>
        &&other) noexcept(std::is_nothrow_move_constructible_v<T>) {
  for (SizeType i = 0; i < other.m_size; ++i) {
    new (&m_data[i]) T(std::move(other.m_data[i]));
    ++m_size;
    other.m_data[i].~T();
  }
  other.m_size = 0;
}

template <typename T, std::size_t C>
constexpr FixedCapacityVector<T, C>::~FixedCapacityVector() {
  clear();
}

template <typename T, std::size_t C>
template <std::size_t OtherCapacity>
constexpr FixedCapacityVector<T, C> &FixedCapacityVector<T, C>::operator=(
    const FixedCapacityVector<T, OtherCapacity>
        &other) noexcept(std::is_nothrow_copy_constructible_v<T>) {
  if (this == &other) {
    return *this;
  }
  if (other.m_size > C) {
    throw std::length_error("FixedCapacityVector: Attempt to copy from a "
                            "vector with more elements than capacity");
  }
  for (SizeType i = 0; i < m_size; ++i) {
    m_data[i].~T();
  }
  m_size = 0;
  for (SizeType i = 0; i < other.m_size; ++i) {
    new (&m_data[i]) T(other.m_data[i]);
    ++m_size;
  }
  return *this;
}

template <typename T, std::size_t C>
constexpr FixedCapacityVector<T, C> &FixedCapacityVector<T, C>::operator=(
    FixedCapacityVector<T, C>
        &&other) noexcept(std::is_nothrow_move_constructible_v<T>) {
  if (this == &other) {
    return *this;
  }
  clear();
  for (SizeType i = 0; i < other.m_size; ++i) {
    new (&m_data[i]) T(std::move(other.m_data[i]));
    ++m_size;
    other.m_data[i].~T();
  }
  other.m_size = 0;
  return *this;
}

template <typename T, std::size_t C>
[[nodiscard]] constexpr typename FixedCapacityVector<T, C>::SizeType
FixedCapacityVector<T, C>::size() const noexcept {
  return m_size;
}

template <typename T, std::size_t C>
[[nodiscard]] constexpr bool
FixedCapacityVector<T, C>::is_empty() const noexcept {
  return m_size == 0UL;
}

template <typename T, std::size_t C>
[[nodiscard]] constexpr bool
FixedCapacityVector<T, C>::is_full() const noexcept {
  return m_size == C;
}

template <typename T, std::size_t C>
[[nodiscard]] constexpr typename FixedCapacityVector<T, C>::ConstReferenceType
FixedCapacityVector<T, C>::front() const {
  if (is_empty()) {
    throw std::out_of_range("FixedCapacityVector: Attempt to access front "
                            "element of empty vector");
  }
  return *m_data;
}

template <typename T, std::size_t C>
[[nodiscard]] constexpr typename FixedCapacityVector<T, C>::ConstReferenceType
FixedCapacityVector<T, C>::back() const {
  if (is_empty()) {
    throw std::out_of_range("FixedCapacityVector: Attempt to access back "
                            "element of empty vector");
  }
  return m_data[m_size - 1];
}

template <typename T, std::size_t C>
[[nodiscard]] constexpr typename FixedCapacityVector<T, C>::ReferenceType
FixedCapacityVector<T, C>::front() {
  if (is_empty()) {
    throw std::out_of_range("FixedCapacityVector: Attempt to access front "
                            "element of empty vector");
  }
  return *m_data;
}

template <typename T, std::size_t C>
[[nodiscard]] constexpr typename FixedCapacityVector<T, C>::ReferenceType
FixedCapacityVector<T, C>::back() {
  if (is_empty()) {
    throw std::out_of_range("FixedCapacityVector: Attempt to access back "
                            "element of empty vector");
  }
  return m_data[m_size - 1];
}

template <typename T, std::size_t C>
[[nodiscard]] constexpr typename FixedCapacityVector<T, C>::ReferenceType
FixedCapacityVector<T, C>::at(SizeType index) {
  if (index >= m_size) {
    throw std::out_of_range("FixedCapacityVector: Out of range access");
  }
  return m_data[index];
}

template <typename T, std::size_t C>
[[nodiscard]] constexpr typename FixedCapacityVector<T, C>::ConstReferenceType
FixedCapacityVector<T, C>::at(SizeType index) const {
  if (index >= m_size) {
    throw std::out_of_range("FixedCapacityVector: Out of range access");
  }
  return m_data[index];
}

template <typename T, std::size_t C>
constexpr void FixedCapacityVector<T, C>::clear() noexcept {
  if (is_empty()) {
    return;
  }

  if constexpr (!std::is_trivially_destructible_v<T>) {
    // destruct all constructed objects
    for (SizeType i = 0; i < m_size; ++i) {
      m_data[i].~T();
    }
  }
  m_size = 0;
}

template <typename T, std::size_t C>
constexpr void FixedCapacityVector<T, C>::push_back(const T &value) {
  if (is_full()) {
    throw std::length_error("FixedCapacityVector: Attempt to push back "
                            "element into a full vector");
  }
  new (&m_data[m_size++]) T(value);
}

template <typename T, std::size_t C>
constexpr void FixedCapacityVector<T, C>::push_back(T &&value) {
  if (is_full()) {
    throw std::length_error("FixedCapacityVector: Attempt to push back "
                            "element into a full vector");
  }
  new (&m_data[m_size++]) T(std::move(value));
}

template <typename T, std::size_t C>
constexpr void FixedCapacityVector<T, C>::pop_back() {
  if (is_empty()) {
    throw std::out_of_range("FixedCapacityVector: Attempt to pop back "
                            "element of empty vector");
  }
  m_data[--m_size].~T();
}

} // namespace CppPlayground

#endif
