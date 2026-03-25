#ifndef CPPPLAYGROUND_FIXED_CAPACITY_VECTOR_HPP
#define CPPPLAYGROUND_FIXED_CAPACITY_VECTOR_HPP

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>
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

  /// A mutable random-access iterator.
  using Iterator = T *;
  /// A constant random-access iterator.
  using ConstIterator = const T *;
  /// A mutable reverse random-access iterator.
  using ReverseIterator = std::reverse_iterator<Iterator>;
  /// A constant reverse random-access iterator.
  using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

  /// Alias for ValueType for STL compatibility
  using value_type = ValueType;

  /// Alias for SizeType for STL compatibility
  using size_type = SizeType;

  /// Difference type for STL compatibility
  using difference_type = std::ptrdiff_t;

  /// Reference type for STL compatibility
  using reference = ValueType &;

  /// Const reference type for STL compatibility
  using const_reference = const ValueType &;

  /// Pointer type for STL compatibility
  using pointer = ValueType *;

  /// Const pointer type for STL compatibility
  using const_pointer = const ValueType *;

  /// Iterator type for STL compatibility
  using iterator = Iterator;

  /// Const iterator type for STL compatibility
  using const_iterator = ConstIterator;

  /// Reverse iterator type for STL compatibility
  using reverse_iterator = ReverseIterator;

  /// Const reverse iterator type for STL compatibility
  using const_reverse_iterator = ConstReverseIterator;

  /// The maximum number of elements the vector can hold.
  static constexpr SizeType Capacity = Capacity_;
  /// The size of a single element in bytes.
  static constexpr SizeType ElementSize = sizeof(ValueType);

  static_assert(Capacity > 0, "Capacity must be larger than 0");

  template <typename U, std::size_t OtherCap> friend class FixedCapacityVector;

private:
  SizeType m_size = 0UL;

  // NOLINTBEGIN(*-avoid-c-arrays)
  alignas(ValueType) std::byte m_buffer[Capacity * ElementSize];
  // NOLINTEND(*-avoid-c-arrays)

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
      std::is_nothrow_copy_constructible_v<ValueType>) {
    for (SizeType i = 0; i < other.m_size; ++i) {
      new (std::addressof(data()[i])) T(other.data()[i]);
      ++m_size;
    }
  }

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
    requires(OtherCapacity != Capacity)
  constexpr FixedCapacityVector(
      const FixedCapacityVector<T, OtherCapacity>
          &other) noexcept(OtherCapacity <= Capacity &&
                           std::is_nothrow_copy_constructible_v<ValueType>) {
    if (other.size() > Capacity) {
      throw std::length_error("FixedCapacityVector: Attempt to copy from a "
                              "vector with more elements than capacity");
    }
    for (SizeType i = 0; i < other.size(); ++i) {
      // use placement new operator to copy construct objects at the right place
      new (std::addressof(data()[i])) T(other.data()[i]);
      ++m_size;
    }
  }

  /**
   * @brief Move constructor.
   * @details Moves elements from another FixedCapacityVector.
   * @param other The vector to move from.
   */
  constexpr FixedCapacityVector(FixedCapacityVector &&other) noexcept(
      std::is_nothrow_move_constructible_v<ValueType> &&
      std::is_nothrow_destructible_v<ValueType>)
      : m_size(other.m_size) {
    for (SizeType i = 0; i < other.m_size; ++i) {
      new (std::addressof(data()[i])) T(std::move(other.data()[i]));
      std::destroy_at(std::addressof(other.data()[i]));
    }
    other.m_size = 0;
  }

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
    requires(OtherCapacity != Capacity)
  constexpr FixedCapacityVector(
      FixedCapacityVector<T, OtherCapacity>
          &&other) noexcept(OtherCapacity <= Capacity &&
                            std::is_nothrow_move_constructible_v<ValueType>) {
    if (other.size() > Capacity) {
      throw std::length_error("FixedCapacityVector: Attempt to move from a "
                              "vector with more elements than capacity");
    }
    for (SizeType i = 0; i < other.size(); ++i) {
      new (std::addressof(data()[i])) T(std::move(other.data()[i]));
      ++m_size;
      std::destroy_at(std::addressof(other.data()[i]));
    }
    other.m_size = 0;
  }

  /**
   * @brief Initializer list constructor.
   * @details Constructs the vector with the elements from the provided
   * initializer list.
   * @param init The initializer list to copy elements from.
   * @throw std::length_error if the size of the initializer list exceeds the
   * vector's capacity.
   */
  constexpr FixedCapacityVector(std::initializer_list<T> init) {
    if (init.size() > Capacity) {
      throw std::length_error("FixedCapacityVector: Attempt to initialize with "
                              "more elements than capacity");
    }
    for (const auto &item : init) {
      new (std::addressof(data()[m_size++])) T(item);
    }
  }

  /**
   * @brief Destructor. Destroys all elements in the vector.
   */
  constexpr ~FixedCapacityVector() { clear(); }

  /**
   * @brief Copy assignment operator.
   * @param other The vector to copy from.
   * @return A reference to this vector.
   */
  constexpr FixedCapacityVector &
  operator=(const FixedCapacityVector &other) noexcept(
      std::is_nothrow_copy_constructible_v<ValueType>) {
    if (this == std::addressof(other)) {
      return *this;
    }
    std::destroy(data(), data() + m_size);
    m_size = 0;
    for (SizeType i = 0; i < other.m_size; ++i) {
      new (std::addressof(data()[i])) T(other.data()[i]);
      ++m_size;
    }
    return *this;
  }

  /**
   * @brief Copy assignment operator.
   * @details Copies elements from another FixedCapacityVector with a different
   * capacity.
   * @tparam OtherCapacity The capacity of the other vector.
   * @param other The vector to copy from.
   * @return A reference to this vector.
   * @throw std::length_error if the other vector's size exceeds this vector's
   * capacity.
   */
  template <SizeType OtherCapacity>
    requires(OtherCapacity != Capacity)
  constexpr FixedCapacityVector &operator=(
      const FixedCapacityVector<ValueType, OtherCapacity>
          &other) noexcept(OtherCapacity <= Capacity &&
                           std::is_nothrow_copy_constructible_v<ValueType>) {
    if constexpr (OtherCapacity > Capacity) {
      if (other.m_size > Capacity) {
        throw std::length_error("FixedCapacityVector: Attempt to copy from a "
                                "vector with more elements than capacity");
      }
    }
    std::destroy(data(), data() + m_size);
    m_size = 0;
    for (SizeType i = 0; i < other.m_size; ++i) {
      new (std::addressof(data()[i])) T(other.data()[i]);
      ++m_size;
    }
    return *this;
  }

  /**
   * @brief Move assignment operator.
   * @param other The vector to move from.
   * @return A reference to this vector.
   */
  constexpr FixedCapacityVector &
  operator=(FixedCapacityVector &&other) noexcept(
      std::is_nothrow_move_constructible_v<ValueType>) {
    if (this == std::addressof(other)) {
      return *this;
    }
    clear();
    for (SizeType i = 0; i < other.m_size; ++i) {
      new (std::addressof(data()[i])) T(std::move(other.data()[i]));
      ++m_size;
      std::destroy_at(std::addressof(other.data()[i]));
    }
    other.m_size = 0;
    return *this;
  }

  template <SizeType OtherCapacity>
    requires(OtherCapacity != Capacity)
  constexpr FixedCapacityVector &
  operator=(FixedCapacityVector<T, OtherCapacity> &&other) noexcept(
      (OtherCapacity < Capacity) &&
      std::is_nothrow_move_assignable_v<ValueType>) {
    if constexpr (OtherCapacity > Capacity) {
      if (other.m_size > Capacity) {
        throw std::length_error(
            "FixedCapacityVector: Attempt to move-assign from a "
            "vector with more elements than capacity");
      }
    }
    clear();
    for (SizeType i = 0; i < other.m_size; ++i) {
      new (std::addressof(data()[i])) T(std::move(other.data()[i]));
      ++m_size;
      std::destroy_at(std::addressof(other.data()[i]));
    }
    other.m_size = 0;
    return *this;
  }

  /**
   * @brief Resizes the vector to the specified size.
   * @param new_size The new size of the vector.
   * @throw std::length_error if the new size exceeds the vector's capacity.
   */
  constexpr void resize(const SizeType new_size) {
    if (m_size == new_size) {
      return;
    }
    while (m_size > new_size) {
      pop_back();
    }
    while (m_size < new_size) {
      emplace_back();
    }
  }

  /**
   * @brief Returns the number of elements in the vector.
   * @return The current number of elements.
   */
  [[nodiscard]] constexpr SizeType size() const noexcept { return m_size; }

  /**
   * @brief Returns the maximum number of elements the vector can hold.
   * @return The capacity of the vector.
   */
  [[nodiscard]] constexpr SizeType capacity() const noexcept {
    return Capacity;
  }

  /**
   * @brief Checks if the vector is empty.
   * @return true if the vector is empty, false otherwise.
   */
  [[nodiscard]] constexpr bool empty() const noexcept { return m_size == 0UL; }

  /**
   * @brief Checks if the vector is full.
   * @return true if the vector has reached its capacity, false otherwise.
   */
  [[nodiscard]] constexpr bool full() const noexcept {
    return m_size == Capacity;
  }

  /**
   * @brief Accesses the first element.
   * @return A reference to the first element.
   * @throw std::out_of_range if the vector is empty.
   */
  template <typename Self>
  [[nodiscard]] constexpr auto &&front(this Self &&self) {
    if (self.empty()) {
      throw std::out_of_range("FixedCapacityVector: Attempt to access front "
                              "element of empty vector");
    }
    return *std::forward<Self>(self).data();
  }

  /**
   * @brief Accesses the last element.
   * @return A reference to the last element.
   * @throw std::out_of_range if the vector is empty.
   */
  template <typename Self>
  [[nodiscard]] constexpr auto &&back(this Self &&self) {
    if (self.empty()) {
      throw std::out_of_range("FixedCapacityVector: Attempt to access back "
                              "element of empty vector");
    }
    return std::forward<Self>(self).data()[self.m_size - 1];
  }

  /**
   * @brief Accesses the element at a specific index with bounds checking.
   * @param index The index of the element to access.
   * @return A reference to the element at the specified index.
   * @throw std::out_of_range if the index is out of bounds.
   */
  template <typename Self>
  [[nodiscard]] constexpr auto &&at(this Self &&self, SizeType index) {
    if (index >= self.m_size) {
      throw std::out_of_range("FixedCapacityVector: Out of range access");
    }
    return std::forward<Self>(self).data()[index];
  }

  /**
   * @brief Accesses the element at a specific index without bounds checking.
   * @param index The index of the element to access.
   * @return A reference to the element at the specified index.
   */
  template <typename Self>
  [[nodiscard]] constexpr auto &&operator[](this Self &&self,
                                            SizeType index) noexcept {
    return std::forward<Self>(self).data()[index];
  }

  /**
   * @brief Returns a pointer to the underlying array serving as element
   * storage.
   * @return A pointer to the underlying array.
   * @todo Make constexpr when we can use std::start_lifetime_as
   */
  [[nodiscard]] T *data() noexcept {
    // NOLINTNEXTLINE(*-pro-type-reinterpret-cast)
    return reinterpret_cast<T *>(m_buffer);
  }

  /**
   * @brief Returns a const pointer to the underlying array serving as element
   * storage.
   * @return A const pointer to the underlying array.
   * @todo Make constexpr when we can use std::start_lifetime_as
   */
  [[nodiscard]] const T *data() const noexcept {
    // NOLINTNEXTLINE(*-pro-type-reinterpret-cast)
    return reinterpret_cast<const T *>(m_buffer);
  }

  /**
   * @brief Clears the vector, destroying all elements.
   */
  constexpr void clear() noexcept {
    if (empty()) {
      return;
    }

    // destruct all constructed objects
    std::destroy(data(), data() + m_size);
    m_size = 0;
  }

  /**
   * @brief Adds an element to the end of the vector by copying.
   * @param value The value to add.
   * @throw std::length_error if the vector is full.
   */
  constexpr void push_back(const T &value) {
    if (full()) {
      throw std::length_error("FixedCapacityVector: Attempt to push back "
                              "element into a full vector");
    }
    new (std::addressof(data()[m_size++])) T(value);
  }

  /**
   * @brief Adds an element to the end of the vector by moving.
   * @param value The value to add.
   * @throw std::length_error if the vector is full.
   */
  constexpr void push_back(T &&value) {
    if (full()) {
      throw std::length_error("FixedCapacityVector: Attempt to push back "
                              "element into a full vector");
    }
    new (std::addressof(data()[m_size++])) T(std::move(value));
  }

  /**
   * @brief Constructs an element in-place at the end of the vector.
   * @tparam Args The types of the arguments to forward to the constructor of
   * the element.
   * @param args The arguments to forward.
   * @return A reference to the newly constructed element.
   * @throw std::length_error if the vector is full.
   */
  template <typename... Args> constexpr T &emplace_back(Args &&...args) {
    if (full()) {
      throw std::length_error("FixedCapacityVector: Attempt to emplace back "
                              "element into a full vector");
    }
    new (std::addressof(data()[m_size++])) T(std::forward<Args>(args)...);
    return data()[m_size - 1];
  }

  /**
   * @brief Removes the last element from the vector.
   * @throw std::out_of_range if the vector is empty.
   */
  constexpr void pop_back() {
    if (empty()) {
      throw std::out_of_range("FixedCapacityVector: Attempt to pop back "
                              "element of empty vector");
    }
    std::destroy_at(std::addressof(data()[--m_size]));
  }

  /**
   * @brief Returns an iterator to the beginning of the vector.
   * @return An iterator to the first element.
   */
  template <typename Self> [[nodiscard]] auto begin(this Self &&self) noexcept {
    return std::forward<Self>(self).data();
  }

  /**
   * @brief Returns a const iterator to the beginning of the vector.
   * @return A const iterator to the first element.
   */
  [[nodiscard]] auto cbegin() const noexcept { return data(); }

  /**
   * @brief Returns an iterator to the end of the vector.
   * @return An iterator to the element following the last element.
   */
  template <typename Self> [[nodiscard]] auto end(this Self &&self) noexcept {
    return std::forward<Self>(self).data() + self.m_size;
  }

  /**
   * @brief Returns a const iterator to the end of the vector.
   * @return A const iterator to the element following the last element.
   */
  [[nodiscard]] auto cend() const noexcept { return data() + m_size; }
  /**
   * @brief Returns a reverse iterator to the beginning of the reversed vector.
   * @return A reverse iterator to the first element of the reversed vector.
   */
  template <typename Self>
  [[nodiscard]] constexpr auto rbegin(this Self &&self) noexcept {
    return std::make_reverse_iterator(std::forward<Self>(self).end());
  }

  /**
   * @brief Returns a const reverse iterator to the beginning of the reversed
   * vector.
   * @return A const reverse iterator to the first element of the reversed
   * vector.
   */
  [[nodiscard]] constexpr auto crbegin() const noexcept { return rbegin(); }
  /**
   * @brief Returns a reverse iterator to the end of the reversed vector.
   * @return A reverse iterator to the element following the last element of the
   * reversed vector.
   */
  template <typename Self>
  [[nodiscard]] constexpr auto rend(this Self &&self) noexcept {
    return std::make_reverse_iterator(std::forward<Self>(self).begin());
  }

  /**
   * @brief Returns a const reverse iterator to the end of the reversed vector.
   * @return A const reverse iterator to the element following the last element
   * of the reversed vector.
   */
  [[nodiscard]] constexpr auto crend() const noexcept { return rend(); }
};
} // namespace CppPlayground

#endif
