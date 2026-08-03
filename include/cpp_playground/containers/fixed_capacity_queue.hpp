#ifndef CPP_PLAYGROUND_CONTAINERS_FIXED_CAPACITY_QUEUE_HPP
#define CPP_PLAYGROUND_CONTAINERS_FIXED_CAPACITY_QUEUE_HPP

#include <cassert>
#include <concepts>
#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <unistd.h>

namespace CppPlayground {

template <typename T> class FixedCapacityQueue {
private:
  union Slot {
    T value;
    std::byte dummy;

    Slot() : dummy{} {}
    ~Slot() {}
  };

  std::unique_ptr<Slot[]> m_data;
  std::size_t m_capacity;
  std::size_t m_head_index = 0;
  std::size_t m_size = 0;

  [[nodiscard]] constexpr std::size_t tail_index() const noexcept {
    std::size_t write_index = m_head_index + m_size;
    if (write_index >= m_capacity) {
      write_index -= m_capacity;
    }
    return write_index;
  }
  constexpr void advance_index(std::size_t &index) noexcept {
    // avoid modulo here
    ++index;
    if (index == m_capacity) {
      index = 0;
    }
    --m_size;
  }

  template <typename... Args>
  bool construct_element(Args &&...args) noexcept(
      std::is_nothrow_constructible_v<T, Args &&...>) {
    if (full()) {
      return false;
    }
    std::construct_at(&m_data[tail_index()].value, std::forward<Args>(args)...);
    ++m_size;
    return true;
  }

  void destruct_element() noexcept {
    // TODO(fk): switch to C++26 contract
    assert(!empty());

    std::destroy_at(&m_data[m_head_index].value);
    advance_index(m_head_index);
  }

public:
  explicit FixedCapacityQueue(std::size_t capacity)
      : m_data(new Slot[capacity]), m_capacity(capacity) {
    if (m_capacity == 0) {
      // TODO(fk): switch to C++26 and use a contract for this check
      throw std::invalid_argument("Cannot create size 0 FixedCapacityQueue");
    }
  }

  // TODO(fk): implement these special members
  FixedCapacityQueue(const FixedCapacityQueue &) = delete;
  FixedCapacityQueue(FixedCapacityQueue &&) = delete;
  FixedCapacityQueue &operator=(const FixedCapacityQueue &) = delete;
  FixedCapacityQueue &operator=(FixedCapacityQueue &&) = delete;

  ~FixedCapacityQueue() noexcept { clear(); }

  void clear() noexcept {
    while (!empty()) {
      destruct_element();
    }
  }

  template <typename... Args>
  bool emplace(Args &&...args) noexcept(
      std::is_nothrow_constructible_v<T, Args &&...>)
    requires(std::constructible_from<T, Args && ...>)
  {
    return construct_element(std::forward<Args>(args)...);
  }


  bool push(const T &value) noexcept(std::is_nothrow_copy_constructible_v<T>) {
    return construct_element(value);
  }
  bool push(T &&value) noexcept(std::is_nothrow_copy_constructible_v<T>) {
    return construct_element(std::move(value));
  }

  // TODO(fk): add front() method

  // TODO(fk): have bool pop() and std::optional<T> try_pop()
  std::optional<T>
  pop() noexcept(std::is_nothrow_constructible_v<std::optional<T>, T &&>) {
    if (empty()) {
      return {};
    }
    std::optional<T> result{std::move(m_data[m_head_index].value)};
    destruct_element();
    return result;
  }

  [[nodiscard]] std::size_t size() const { return m_size; }
  [[nodiscard]] std::size_t capacity() const { return m_capacity; }
  [[nodiscard]] bool empty() const { return m_size == 0; }
  [[nodiscard]] bool full() const { return m_size == m_capacity; }
};

} // namespace CppPlayground

#endif
