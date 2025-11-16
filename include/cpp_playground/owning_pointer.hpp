#ifndef CPPPLAYGROUND_OWNING_POINTER_HPP
#define CPPPLAYGROUND_OWNING_POINTER_HPP

#include "./deleter.hpp"
#include <type_traits>
#include <utility>

namespace CppPlayground {

/**
 * @brief A simple smart pointer that provides unique ownership of a dynamically
 * allocated object.
 * @details The OwningPointer automatically manages the lifetime of the object
 * it points to, deallocating it when the OwningPointer goes out of scope. It is
 * non-copyable but movable, ensuring that only one OwningPointer can own the
 * object at any time. This is a simplified implementation similar to
 * std::unique_ptr.
 *
 * @tparam T The type of the managed object.
 * @tparam D The type of the deleter used to deallocate the object.
 */
template <typename T, typename D = DefaultDeleter<T>> class OwningPointer {
public:
  /** @brief The type of the managed object. */
  using ElementType = std::remove_extent_t<T>;
  /** @brief The type of the deleter. */
  using DeleterType = std::remove_reference_t<D>;

private:
  [[no_unique_address]] DeleterType m_deleter;
  ElementType *m_ptr;

public:
  /**
   * @brief Constructs a new OwningPointer that does not own an object.
   */
  constexpr OwningPointer() noexcept;
  /**
   * @brief Constructs a new OwningPointer that does not own an object.
   */
  constexpr OwningPointer(std::nullptr_t) noexcept;
  /**
   * @brief Constructs a new OwningPointer that takes ownership of a raw
   * pointer.
   * @param ptr A pointer to the object to manage.
   */
  constexpr explicit OwningPointer(ElementType *ptr) noexcept;
  /**
   * @brief Constructs a new OwningPointer that takes ownership of a raw
   * pointer and uses a provided deleter.
   * @param ptr A pointer to the object to manage.
   * @param deleter A reference to the deleter to use.
   */
  constexpr explicit OwningPointer(ElementType *ptr,
                                   const DeleterType &deleter) noexcept;
  /**
   * @brief Constructs a new OwningPointer that takes ownership of a raw
   * pointer and uses a provided deleter.
   * @param ptr A pointer to the object to manage.
   * @param deleter An rvalue reference to the deleter to use.
   */
  constexpr explicit OwningPointer(ElementType *ptr,
                                   DeleterType &&deleter) noexcept;

  OwningPointer(const OwningPointer &) = delete;
  template <typename OtherDeleter>
  OwningPointer(const OwningPointer<T, OtherDeleter> &) = delete;

  /**
   * @brief Move constructs a new OwningPointer from another.
   * @details After the move, `other` will no longer own the object.
   * @param other The OwningPointer to move from.
   */
  constexpr OwningPointer(OwningPointer &&other) noexcept;

  /**
   * @brief Destroys the OwningPointer and deallocates the managed object, if
   * any.
   */
  ~OwningPointer() noexcept(
      noexcept(std::declval<DeleterType &>()(std::declval<ElementType *>())));

  OwningPointer &operator=(const OwningPointer &) = delete;

  /**
   * @brief Move assigns an OwningPointer from another.
   * @details Deallocates the currently owned object, if any, and takes
   * ownership of the object from `other`. After the move, `other` will no
   * longer own the object.
   * @param other The OwningPointer to move from.
   * @return A reference to this OwningPointer.
   */
  OwningPointer &operator=(OwningPointer &&other);

  /**
   * @brief Assigns nullptr to the OwningPointer.
   * @details Deallocates the currently owned object, if any.
   * @return A reference to this OwningPointer.
   */
  OwningPointer &operator=(std::nullptr_t);

  /**
   * @brief Gets the raw pointer to the managed object.
   * @return The raw pointer.
   */
  constexpr ElementType *get() const noexcept;
  /**
   * @brief Gets a reference to the deleter.
   * @return A reference to the deleter.
   */
  constexpr DeleterType &deleter() noexcept;
  /**
   * @brief Gets a const reference to the deleter.
   * @return A const reference to the deleter.
   */
  constexpr const DeleterType &deleter() const noexcept;
  /**
   * @brief Deallocates the managed object, if any, and sets the internal
   * pointer to nullptr.
   */
  constexpr void release();

  /**
   * @brief Dereferences the pointer to the managed object.
   * @return A reference to the managed object.
   */
  constexpr ElementType &operator*() const noexcept;
  /**
   * @brief Dereferences the pointer to the managed object.
   * @return The raw pointer to the managed object.
   */
  constexpr ElementType *operator->() const noexcept;
};

template <typename T, typename D>
constexpr OwningPointer<T, D>::OwningPointer() noexcept
    : m_deleter{}, m_ptr{nullptr} {}

template <typename T, typename D>
constexpr OwningPointer<T, D>::OwningPointer(std::nullptr_t) noexcept
    : m_deleter{}, m_ptr{nullptr} {}

template <typename T, typename D>
constexpr OwningPointer<T, D>::OwningPointer(ElementType *ptr) noexcept
    : m_deleter{}, m_ptr{ptr} {}

template <typename T, typename D>
constexpr OwningPointer<T, D>::OwningPointer(
    ElementType *ptr, const DeleterType &deleter) noexcept
    : m_deleter(deleter), m_ptr{ptr} {}

template <typename T, typename D>
constexpr OwningPointer<T, D>::OwningPointer(ElementType *ptr,
                                             DeleterType &&deleter) noexcept
    : m_deleter(std::move(deleter)), m_ptr{ptr} {}

template <typename T, typename D>
constexpr OwningPointer<T, D>::OwningPointer(OwningPointer &&other) noexcept
    : m_deleter(std::move(other.m_deleter)), m_ptr{other.m_ptr} {
  other.m_ptr = nullptr;
}

template <typename T, typename D>
OwningPointer<T, D>::~OwningPointer() noexcept(
    noexcept(std::declval<DeleterType &>()(std::declval<ElementType *>()))) {
  if (m_ptr != nullptr) {
    m_deleter(m_ptr);
  }
}

template <typename T, typename D>
OwningPointer<T, D> &
OwningPointer<T, D>::operator=(OwningPointer &&other) {
  if (this == &other) {
    return *this;
  }
  if (m_ptr != nullptr) {
    m_deleter(m_ptr);
    m_ptr = nullptr;
  }
  m_deleter = std::move(other.m_deleter);
  m_ptr = other.m_ptr;
  other.m_ptr = nullptr;
  return *this;
}

template <typename T, typename D>
OwningPointer<T, D> &OwningPointer<T, D>::operator=(std::nullptr_t) {
  if (m_ptr != nullptr) {
    m_deleter(m_ptr);
    m_ptr = nullptr;
  }
  return *this;
}

template <typename T, typename D>
constexpr typename OwningPointer<T, D>::ElementType *
OwningPointer<T, D>::get() const noexcept {
  return m_ptr;
}

template <typename T, typename D>
constexpr typename OwningPointer<T, D>::DeleterType &
OwningPointer<T, D>::deleter() noexcept {
  return m_deleter;
}

template <typename T, typename D>
constexpr const typename OwningPointer<T, D>::DeleterType &
OwningPointer<T, D>::deleter() const noexcept {
  return m_deleter;
}

template <typename T, typename D>
constexpr void OwningPointer<T, D>::release() {
  if (m_ptr == nullptr) {
    return;
  }
  m_deleter(m_ptr);
  m_ptr = nullptr;
}

template <typename T, typename D>
constexpr typename OwningPointer<T, D>::ElementType &
OwningPointer<T, D>::operator*() const noexcept {
  return *m_ptr;
}

template <typename T, typename D>
constexpr typename OwningPointer<T, D>::ElementType *
OwningPointer<T, D>::operator->() const noexcept {
  return m_ptr;
}

} // namespace CppPlayground

#endif
