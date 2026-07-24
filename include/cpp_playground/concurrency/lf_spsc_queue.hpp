#ifndef CPP_PLAYGROUND_CONCURRENCY_LC_SPSC_QUEUE_HPP
#define CPP_PLAYGROUND_CONCURRENCY_LC_SPSC_QUEUE_HPP

#include <atomic>
#include <cstddef>
#include <new>
#include <optional>
#include <vector>

namespace CppPlayground {

template <typename T> class LFSPSCQueue {
private:
  struct Slot {
    union {
      std::byte dummy;
      T value;
    };
    Slot() noexcept : dummy() {}
    ~Slot() {}
  };
  std::vector<Slot> m_ring_buffer;
  std::atomic_size_t m_read_index{0};
  std::atomic_size_t m_write_index{0};

public:
  explicit LFSPSCQueue(std::size_t capacity)
      : m_ring_buffer(capacity + 1)
      // +1 to distinguish full and empty state:
      // empty: read == write
      // full: (write + 1) % capacity == read

  {}

  [[nodiscard]] bool try_push(T value) {
    auto write =
        m_write_index.load(std::memory_order_relaxed); // only one producer
    auto next = (write + 1) % m_ring_buffer.size();
    if (next == m_read_index.load(
                    std::memory_order::acquire)) // make sure in-transit read
                                                 // are committed to memory
    {
      return false;
    }
    new (m_ring_buffer[write].value) T(std::move(value));
    m_write_index.store(next, std::memory_order::release);
    return true;
  }
  [[nodiscard]] std::optional<T> try_pop() {
    auto read =
        m_read_index.load(std::memory_order_relaxed); // only one consumer
    if (read == m_write_index.load(
                    std::memory_order_acquire)) // make sure in-transit write is
                                                // committed to memory
    {
      return std::nullopt;
    }
    T *ptr = std::launder(m_ring_buffer[read].value);
    std::optional<T> result(std::move(*ptr));
    ptr->~T();
    auto next = (read + 1) % m_ring_buffer.size();
    m_read_index.store(next, std::memory_order_release);
    return result;
  }
};

} // namespace CppPlayground

#endif
