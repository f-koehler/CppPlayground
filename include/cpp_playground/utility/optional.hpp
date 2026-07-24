#ifndef CPP_PLAYGROUND_UTILITY_OPTIONAL_HPP
#define CPP_PLAYGROUND_UTILITY_OPTIONAL_HPP

#include <cstddef>
#include <optional>
#include <type_traits>
#include <utility>

namespace CppPlayground {

template <typename T> class Optional {
public:
  using value_type = T;
  static constexpr std::size_t value_size = sizeof(T);

private:
  union Storage {
    std::byte dummy;
    T value;

    Storage() : dummy() {}
    ~Storage() {}
  };
  Storage m_storage;
  bool m_has_value = false;

public:
  Optional() = default;

  constexpr Optional(T &&value) noexcept(
      std::is_nothrow_move_constructible_v<T>) {
    new (&m_storage.value) T(std::move(value));
    m_has_value = true;
  }

  constexpr Optional(const T &value) noexcept(
      std::is_nothrow_copy_constructible_v<T>) {
    new (&m_storage.value) T(value);
    m_has_value =
        true; // only set after successful construction in case copy ctor throws
  }

  constexpr Optional(const Optional &other) noexcept(
      std::is_nothrow_copy_constructible_v<T>) {
    if (other.has_value()) {
      new (&m_storage.value) T(other.m_storage.value);
      m_has_value = true;
    }
  }

  constexpr Optional(Optional &&other) noexcept(
      std::is_nothrow_move_constructible_v<T>) {
    if (other.has_value()) {
      new (&m_storage.value) T(std::move(other.m_storage.value));
      m_has_value = true;
    }
  }

  template <typename... Args>
  constexpr Optional(
      [[maybe_unused]] std::in_place_t in_place,
      Args &&...args) noexcept(std::is_nothrow_constructible_v<T, Args &&...>) {
    new (&m_storage.value) T(std::forward<Args>(args)...);
    m_has_value = true;
  }

  constexpr ~Optional() {
    if (m_has_value) {
      m_storage.value.~T();
    }
  }

  constexpr Optional &operator=(const Optional &other) noexcept(
      std::is_nothrow_copy_constructible_v<T> &&
      std::is_nothrow_copy_assignable_v<T>) {
    if (this == &other) {
      return *this;
    }

    if (m_has_value && other.m_has_value) {
      if constexpr(std::is_nothrow_copy_assignable_v<T>) {
      m_storage.value = other.m_storage.value;
      } else {
          // TODO(fk): implement strong exception guarantee
          // check if nothrow swappable T -> swap (implement Optional::swap noexcept first)
          // consider adding
          // template<class... Args>
          // void construct(Args&&... args)
          // {
          //     std::construct_at(
          //         std::addressof(m_storage.value),
          //         std::forward<Args>(args)...);

          //     m_has_value = true;
          // }

          // void destroy() noexcept
          // {
          //     if (m_has_value) {
          //         std::destroy_at(std::addressof(m_storage.value));
          //         m_has_value = false;
          //     }
          // }
      }
    } else if (m_has_value) {
      m_storage.value.~T();
      m_has_value = false;
    } else if (other.m_has_value) {
      new (&m_storage.value) T(other.m_storage.value);
      m_has_value = true;
    }

    return *this;
  }

  constexpr Optional &operator=(Optional &&other) noexcept(
      std::is_nothrow_move_constructible_v<T> &&
      std::is_nothrow_move_assignable_v<T>) {
    if (this == &other) {
      return *this;
    }

    if (m_has_value && other.m_has_value) {
      // TODO(fk): implement strong exception guarantee
      m_storage.value = std::move(other.m_storage.value);
    } else if (m_has_value) {
      m_storage.value.~T();
      m_has_value = false;
    } else if (other.m_has_value) {
      new (&m_storage.value) T(std::move(other.m_storage));
      m_has_value = true;
    }

    return *this;
  }

  [[nodiscard]] constexpr bool has_value() const noexcept {
    return m_has_value;
  }

  constexpr T &value() {
    if (!has_value()) {
      throw std::bad_optional_access();
    }
    return m_storage.value;
  }
  constexpr const T &value() const {
    if (!has_value()) {
      throw std::bad_optional_access();
    }
    return m_storage.value;
  }
};

} // namespace CppPlayground

#endif
