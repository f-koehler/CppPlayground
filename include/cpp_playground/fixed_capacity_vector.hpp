#ifndef CPPPLAYGROUND_FIXED_CAPACITY_VECTOR_HPP
#define CPPPLAYGROUND_FIXED_CAPACITY_VECTOR_HPP

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <type_traits>

namespace CppPlayground {
/**
 * @brief A vector with a fixed capacity, allocated on the stack.
 * @details This vector provides a std::vector-like interface but with a
 * compile-time fixed capacity. The memory is allocated on the stack, avoiding
 * dynamic memory allocation. This is useful for real-time and
 * performance-critical applications where heap allocation is undesirable.
 * @tparam T The type of elements to be stored.
 * @tparam Capacity_ The maximum number of elements the vector can hold.
 */
template <typename T, std::size_t Capacity_> class FixedCapacityVector {
public:
  /// The type used for size and capacity.
  using SizeType = std::size_t;
  /// The type of the elements stored in the vector.
  using ValueType = T;
  /// A reference to an element.
  using ReferenceType = T &;
  /// A const reference to an element.
  using ConstReferenceType = const T &;

  /// A mutable random-access iterator.
  using Iterator = T *;
  /// A constant random-access iterator.
  using ConstIterator = const T *;
  /// A mutable reverse random-access iterator.
  using ReverseIterator = std::reverse_iterator<Iterator>;
  /// A constant reverse random-access iterator.
  using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

  /// The maximum number of elements the vector can hold.
  static constexpr SizeType Capacity = Capacity_;
  /// The size of a single element in bytes.
  static constexpr SizeType ElementSize = sizeof(ValueType);

private:
  alignas(ValueType) unsigned char m_buffer[Capacity * ElementSize] = {0U};
  T *m_data = (T *)m_buffer;
  SizeType m_size = 0UL;

public:
  /**
   * @brief Default constructor. Constructs an empty vector.
   */
  constexpr FixedCapacityVector() noexcept = default;

  /**
   * @brief Copy constructor.
   * @details Copies elements from another FixedCapacityVector with the same
   * capacity.
   * @param other The vector to copy from.
   */
  constexpr FixedCapacityVector(const FixedCapacityVector &other) noexcept(
      std::is_nothrow_copy_constructible_v<ValueType>);

  /**
   * @brief Copy constructor.
   * @details Copies elements from another FixedCapacityVector. The other vector
   * can have a different capacity.
   * @tparam OtherCapacity The capacity of the other vector.
   * @param other The vector to copy from.
   * @throw std::length_error if the other vector's size exceeds this vector's
   * capacity.
   */
  template <SizeType OtherCapacity>
  constexpr FixedCapacityVector(
      const FixedCapacityVector<T, OtherCapacity> &other);

  /**
   * @brief Move constructor.
   * @details Moves elements from another FixedCapacityVector.
   * @param other The vector to move from.
   */
  constexpr FixedCapacityVector(FixedCapacityVector &&other) noexcept(
      std::is_nothrow_move_constructible_v<ValueType>);

  /**
   * @brief Move constructor.
   * @details Moves elements from another FixedCapacityVector. The other vector
   * can have a different capacity.
   * @tparam OtherCapacity The capacity of the other vector.
   * @param other The vector to move from.
   * @throw std::length_error if the other vector's size exceeds this vector's
   * capacity.
   */
  template <SizeType OtherCapacity>
  constexpr FixedCapacityVector(FixedCapacityVector<T, OtherCapacity> &&other);

  /**
   * @brief Initializer list constructor.
   * @details Constructs the vector with the elements from the provided
   * initializer list.
   * @param init The initializer list to copy elements from.
   * @throw std::length_error if the size of the initializer list exceeds the
   * vector's capacity.
   */
  constexpr FixedCapacityVector(std::initializer_list<T> init);

  /**
   * @brief Destructor. Destroys all elements in the vector.
   */
  constexpr ~FixedCapacityVector();

  /**
   * @brief Copy assignment operator.
   * @tparam OtherCapacity The capacity of the other vector.
   * @param other The vector to copy from.
   * @return A reference to this vector.
   * @throw std::length_error if the other vector's size exceeds this vector's
   * capacity.
   */
  template <SizeType OtherCapacity>
  constexpr FixedCapacityVector &
  operator=(const FixedCapacityVector<ValueType, OtherCapacity>
                &other) noexcept(std::is_nothrow_copy_constructible_v<T>);

  /**
   * @brief Move assignment operator.
   * @param other The vector to move from.
   * @return A reference to this vector.
   */
  constexpr FixedCapacityVector &
  operator=(FixedCapacityVector &&other) noexcept(
      std::is_nothrow_move_constructible_v<ValueType>);

  /**
   * @brief Returns the number of elements in the vector.
   * @return The current number of elements.
   */
  [[nodiscard]] constexpr SizeType size() const noexcept;

  /**
   * @brief Returns the maximum number of elements the vector can hold.
   * @return The capacity of the vector.
   */
  [[nodiscard]] consteval SizeType capacity() const noexcept {
    return Capacity;
  }

  /**
   * @brief Checks if the vector is empty.
   * @return true if the vector is empty, false otherwise.
   */
  [[nodiscard]] constexpr bool is_empty() const noexcept;

  /**
   * @brief Checks if the vector is full.
   * @return true if the vector has reached its capacity, false otherwise.
   */
  [[nodiscard]] constexpr bool is_full() const noexcept;

  /**
   * @brief Accesses the first element.
   * @return A const reference to the first element.
   * @throw std::out_of_range if the vector is empty.
   */
  [[nodiscard]] constexpr ConstReferenceType front() const;

  /**
   * @brief Accesses the last element.
   * @return A const reference to the last element.
   * @throw std::out_of_range if the vector is empty.
   */
  [[nodiscard]] constexpr ConstReferenceType back() const;

  /**
   * @brief Accesses the first element.
   * @return A reference to the first element.
   * @throw std::out_of_range if the vector is empty.
   */
  [[nodiscard]] constexpr ReferenceType front();

  /**
   * @brief Accesses the last element.
   * @return A reference to the last element.
   * @throw std::out_of_range if the vector is empty.
   */
  [[nodiscard]] constexpr ReferenceType back();

  /**
   * @brief Accesses the element at a specific index with bounds checking.
   * @param index The index of the element to access.
   * @return A reference to the element at the specified index.
   * @throw std::out_of_range if the index is out of bounds.
   */
  [[nodiscard]] constexpr ReferenceType at(SizeType index);

  /**
   * @brief Accesses the element at a specific index with bounds checking.
   * @param index The index of the element to access.
   * @return A const reference to the element at the specified index.
   * @throw std::out_of_range if the index is out of bounds.
   */
  [[nodiscard]] constexpr ConstReferenceType at(SizeType index) const;

  /**
   * @brief Accesses the element at a specific index without bounds checking.
   * @param index The index of the element to access.
   * @return A reference to the element at the specified index.
   */
  [[nodiscard]] constexpr ReferenceType operator[](SizeType index) noexcept;
  /**
   * @brief Accesses the element at a specific index without bounds checking.
   * @param index The index of the element to access.
   * @return A const reference to the element at the specified index.
   */
  [[nodiscard]] constexpr ConstReferenceType
  operator[](SizeType index) const noexcept;

  /**
   * @brief Returns a pointer to the underlying array serving as element
   * storage.
   * @return A pointer to the underlying array.
   */
  [[nodiscard]] constexpr T *data() noexcept;

  /**
   * @brief Returns a const pointer to the underlying array serving as element
   * storage.
   * @return A const pointer to the underlying array.
   */
  [[nodiscard]] constexpr const T *data() const noexcept;

  /**
   * @brief Clears the vector, destroying all elements.
   */
  constexpr void clear() noexcept;

  /**
   * @brief Adds an element to the end of the vector by copying.
   * @param value The value to add.
   * @throw std::length_error if the vector is full.
   */
  constexpr void push_back(const T &value);

  /**
   * @brief Adds an element to the end of the vector by moving.
   * @param value The value to add.
   * @throw std::length_error if the vector is full.
   */
  constexpr void push_back(T &&value);

  /**
   * @brief Constructs an element in-place at the end of the vector.
   * @tparam Args The types of the arguments to forward to the constructor of
   * the element.
   * @param args The arguments to forward.
   * @return A reference to the newly constructed element.
   * @throw std::length_error if the vector is full.
   */
  template <typename... Args>
  constexpr ReferenceType emplace_back(Args &&...args);

  /**
   * @brief Removes the last element from the vector.
   * @throw std::out_of_range if the vector is empty.
   */
  constexpr void pop_back();

  /**
   * @brief Returns an iterator to the beginning of the vector.
   * @return An iterator to the first element.
   */
  [[nodiscard]] constexpr Iterator begin() noexcept;
  /**
   * @brief Returns a const iterator to the beginning of the vector.
   * @return A const iterator to the first element.
   */
  [[nodiscard]] constexpr ConstIterator begin() const noexcept;
  /**
   * @brief Returns a const iterator to the beginning of the vector.
   * @return A const iterator to the first element.
   */
  [[nodiscard]] constexpr ConstIterator cbegin() const noexcept;
  /**
   * @brief Returns a reverse iterator to the beginning of the reversed vector.
   * @return A reverse iterator to the first element of the reversed vector.
   */
  [[nodiscard]] constexpr ReverseIterator rbegin() noexcept;
  /**
   * @brief Returns a const reverse iterator to the beginning of the reversed
   * vector.
   * @return A const reverse iterator to the first element of the reversed
   * vector.
   */
  [[nodiscard]] constexpr ConstReverseIterator rbegin() const noexcept;
  /**
   * @brief Returns a const reverse iterator to the beginning of the reversed
   * vector.
   * @return A const reverse iterator to the first element of the reversed
   * vector.
   */
  [[nodiscard]] constexpr ConstReverseIterator crbegin() const noexcept;
  /**
   * @brief Returns an iterator to the end of the vector.
   * @return An iterator to the element following the last element.
   */
  [[nodiscard]] constexpr Iterator end() noexcept;
  /**
   * @brief Returns a const iterator to the end of the vector.
   * @return A const iterator to the element following the last element.
   */
  [[nodiscard]] constexpr ConstIterator end() const noexcept;
  /**
   * @brief Returns a const iterator to the end of the vector.
   * @return A const iterator to the element following the last element.
   */
  [[nodiscard]] constexpr ConstIterator cend() const noexcept;
  /**
   * @brief Returns a reverse iterator to the end of the reversed vector.
   * @return A reverse iterator to the element following the last element of the
   * reversed vector.
   */
  [[nodiscard]] constexpr ReverseIterator rend() noexcept;
  /**
   * @brief Returns a const reverse iterator to the end of the reversed vector.
   * @return A const reverse iterator to the element following the last element
   * of the reversed vector.
   */
  [[nodiscard]] constexpr ConstReverseIterator rend() const noexcept;
  /**
   * @brief Returns a const reverse iterator to the end of the reversed vector.
   * @return A const reverse iterator to the element following the last element
   * of the reversed vector.
   */
  [[nodiscard]] constexpr ConstReverseIterator crend() const noexcept;
};

template <typename T, std::size_t C>
constexpr FixedCapacityVector<T, C>::FixedCapacityVector(
    const FixedCapacityVector<T, C>
        &other) noexcept(std::is_nothrow_copy_constructible_v<ValueType>) {
  m_size = other.m_size;
  for (SizeType i = 0; i < other.m_size; ++i) {
    new (&m_data[i]) T(other.m_data[i]);
  }
}

template <typename T, std::size_t C>
template <std::size_t OtherCapacity>
constexpr FixedCapacityVector<T, C>::FixedCapacityVector(
    const FixedCapacityVector<T, OtherCapacity> &other) {

  if (other.size() > C) {
    throw std::length_error("FixedCapacityVector: Attempt to copy from a "
                            "vector with more elements than capacity");
  }
  for (SizeType i = 0; i < other.size(); ++i) {
    // use placement new operator to copy construct objects at the right place
    new (&m_data[i]) T(other.data()[i]);
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
constexpr FixedCapacityVector<T, C>::FixedCapacityVector(
    std::initializer_list<T> init) {
  if (init.size() > C) {
    throw std::length_error("FixedCapacityVector: Attempt to initialize with "
                            "more elements than capacity");
  }
  for (const auto &item : init) {
    new (&m_data[m_size++]) T(item);
  }
}

template <typename T, std::size_t C>
template <std::size_t OtherCapacity>
constexpr FixedCapacityVector<T, C>::FixedCapacityVector(
    FixedCapacityVector<T, OtherCapacity> &&other) {
  if (other.size() > C) {
    throw std::length_error("FixedCapacityVector: Attempt to move from a "
                            "vector with more elements than capacity");
  }
  for (SizeType i = 0; i < other.size(); ++i) {
    new (&m_data[i]) T(std::move(other.data()[i]));
    ++m_size;
    other.data()[i].~T();
  }
  other.clear();
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
[[nodiscard]] constexpr typename FixedCapacityVector<T, C>::ReferenceType
FixedCapacityVector<T, C>::operator[](SizeType index) noexcept {
  return m_data[index];
}

template <typename T, std::size_t C>
[[nodiscard]] constexpr typename FixedCapacityVector<T, C>::ConstReferenceType
FixedCapacityVector<T, C>::operator[](SizeType index) const noexcept {
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
[[nodiscard]] constexpr T *FixedCapacityVector<T, C>::data() noexcept {
  return m_data;
}

template <typename T, std::size_t C>
[[nodiscard]] constexpr const T *
FixedCapacityVector<T, C>::data() const noexcept {
  return m_data;
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
template <typename... Args>
constexpr typename FixedCapacityVector<T, C>::ReferenceType
FixedCapacityVector<T, C>::emplace_back(Args &&...args) {
  if (is_full()) {
    throw std::length_error("FixedCapacityVector: Attempt to emplace back "
                            "element into a full vector");
  }
  new (&m_data[m_size++]) T(std::forward<Args>(args)...);
}

template <typename T, std::size_t C>
constexpr void FixedCapacityVector<T, C>::pop_back() {
  if (is_empty()) {
    throw std::out_of_range("FixedCapacityVector: Attempt to pop back "
                            "element of empty vector");
  }
  m_data[--m_size].~T();
}

template <typename T, std::size_t C>
[[nodiscard]] constexpr typename FixedCapacityVector<T, C>::Iterator
FixedCapacityVector<T, C>::begin() noexcept {
  return m_data;
}

template <typename T, std::size_t C>
[[nodiscard]] constexpr typename FixedCapacityVector<T, C>::ConstIterator
FixedCapacityVector<T, C>::begin() const noexcept {
  return m_data;
}

template <typename T, std::size_t C>
[[nodiscard]] constexpr typename FixedCapacityVector<T, C>::ConstIterator
FixedCapacityVector<T, C>::cbegin() const noexcept {
  return m_data;
}

template <typename T, std::size_t C>
[[nodiscard]] constexpr typename FixedCapacityVector<T, C>::ReverseIterator
FixedCapacityVector<T, C>::rbegin() noexcept {
  return std::make_reverse_iterator(end());
}

template <typename T, std::size_t C>
[[nodiscard]] constexpr typename FixedCapacityVector<T, C>::ConstReverseIterator
FixedCapacityVector<T, C>::rbegin() const noexcept {
  return std::make_reverse_iterator(end());
}

template <typename T, std::size_t C>
[[nodiscard]] constexpr typename FixedCapacityVector<T, C>::ConstReverseIterator
FixedCapacityVector<T, C>::crbegin() const noexcept {
  return std::make_reverse_iterator(cend());
}

template <typename T, std::size_t C>
[[nodiscard]] constexpr typename FixedCapacityVector<T, C>::Iterator
FixedCapacityVector<T, C>::end() noexcept {
  return m_data + m_size;
}

template <typename T, std::size_t C>
[[nodiscard]] constexpr typename FixedCapacityVector<T, C>::ConstIterator
FixedCapacityVector<T, C>::end() const noexcept {
  return m_data + m_size;
}

template <typename T, std::size_t C>
[[nodiscard]] constexpr typename FixedCapacityVector<T, C>::ConstIterator
FixedCapacityVector<T, C>::cend() const noexcept {
  return m_data + m_size;
}

template <typename T, std::size_t C>
[[nodiscard]] constexpr typename FixedCapacityVector<T, C>::ReverseIterator
FixedCapacityVector<T, C>::rend() noexcept {
  return std::make_reverse_iterator(begin());
}

template <typename T, std::size_t C>
[[nodiscard]] constexpr typename FixedCapacityVector<T, C>::ConstReverseIterator
FixedCapacityVector<T, C>::rend() const noexcept {
  return std::make_reverse_iterator(begin());
}

template <typename T, std::size_t C>
[[nodiscard]] constexpr typename FixedCapacityVector<T, C>::ConstReverseIterator
FixedCapacityVector<T, C>::crend() const noexcept {
  return std::make_reverse_iterator(cbegin());
}

} // namespace CppPlayground

#endif
