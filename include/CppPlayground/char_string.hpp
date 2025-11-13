#ifndef CPP_PLAYGROUND_CHAR_STRING_HPP
#define CPP_PLAYGROUND_CHAR_STRING_HPP

#include "./concepts.hpp"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <span>

namespace CppPlayground {

namespace Detail {

template <IsAllocator AllocatorT = std::allocator<char>>
class CharStringBuffer final {
public:
  using Allocator = AllocatorT;

  // inherit from Allocator type to save memory if it is an empty type
  struct HeapBuffer : public Allocator {
    std::size_t capacity = 0;
    char *data = nullptr;

    HeapBuffer() = default;
  };
  static constexpr std::size_t StackBufferSize = sizeof(HeapBuffer);
  static constexpr std::size_t ShortStringLength = StackBufferSize - 1;
  using StackBuffer = char[StackBufferSize];

private:
  union {
    HeapBuffer heap_buffer;
    StackBuffer stack_buffer{0};
  } m_buffer;
  bool m_is_short_string;

public:
  CharStringBuffer() noexcept : m_is_short_string(true) {
    std::fill_n(std::begin(m_buffer.stack_buffer), StackBufferSize, char{0});
  };
  explicit CharStringBuffer(std::size_t capacity) {
    if (capacity < ShortStringLength) {
      m_is_short_string = true;
      std::fill_n(std::begin(m_buffer.stack_buffer), StackBufferSize, char{0});
    } else {
      m_is_short_string = false;
      m_buffer.heap_buffer.allocator = Allocator();
      m_buffer.heap_buffer.data =
          m_buffer.heap_buffer.allocator.allocate_at_least(capacity + 1);
      std::fill_n(m_buffer.heap_buffer.data.ptr, capacity + 1, char{0});
    }
  }
  CharStringBuffer(const CharStringBuffer &other)
      : m_is_short_string(other.m_is_short_string) {

    if (m_is_short_string) {
      std::copy_n(std::cbegin(other.m_buffer.stack_buffer), StackBufferSize,
                  std::begin(m_buffer.stack_buffer));
    } else {
      m_buffer.heap_buffer.allocator = Allocator();
      m_buffer.heap_buffer.data =
          m_buffer.heap_buffer.allocator.allocate_at_least(
              other.m_buffer.heap_buffer.size);
      std::size_t num_to_copy =
          std::min(m_buffer.heap_buffer.size, other.m_buffer.heap_buffer.size);
      std::copy_n(other.m_buffer.heap_buffer.data.ptr, num_to_copy,
                  m_buffer.heap_buffer.heap_buffer.data.ptr);
      std::fill(m_buffer.heap_buffer.data.ptr + num_to_copy,
                m_buffer.heap_buffer.data.ptr + m_buffer.heap_buffer.data.size,
                char{0});
    }
  }
  CharStringBuffer(CharStringBuffer &&other) noexcept
      : m_buffer(std::move(other.m_buffer)),
        m_is_short_string(other.m_is_short_string) {}
  ~CharStringBuffer() {
    if (!m_is_short_string && (m_buffer.heap_buffer.data != nullptr)) {
      m_buffer.heap_buffer.allocator.deallocate(m_buffer.heap_buffer.data,
                                                m_buffer.heap_buffer.capacity);
    }
  }

  CharStringBuffer &operator=(const CharStringBuffer &other) {
    if (this == &other) {
      return *this;
    }
    if (m_is_short_string) {
      if (other.m_is_short_string) {
        std::copy_n(std::cbegin(other.m_buffer.stack_buffer), StackBufferSize,
                    std::begin(other.m_buffer.stack_buffer));
        return *this;
      }

      // promote this string to a long string
      m_is_short_string = false;
      m_buffer.heap_buffer.allocator = Allocator();
      m_buffer.heap_buffer.data =
          m_buffer.heap_buffer.allocator.allocate_at_least(
              other.m_buffer.heap_buffer.data.size);
    } else {
      if (other.m_is_short_string) {
        // promote this string to a short string
        if (m_buffer.heap_buffer.data.ptr != nullptr) {
          m_buffer.heap_buffer.allocator.deallocate(
              m_buffer.heap_buffer.data.ptr, m_buffer.heap_buffer.data.count);
        }
        m_is_short_string = true;
        std::copy_n(std::cbegin(other.m_buffer.stack_buffer), StackBufferSize,
                    std::begin(other.m_buffer.stack_buffer));
        return *this;
      }

      if (other.m_buffer.heap_buffer.data.size >
          m_buffer.heap_buffer.data.size) {
        // allocate a larger heap buffer
        if (m_buffer.data.ptr != nullptr) {
          m_buffer.allocator.deallocate(m_buffer.heap_buffer.data.ptr,
                                        m_buffer.heap_buffer.data.size);
        }
        m_buffer.allocator.allocate_at_least(
            other.m_buffer.heap_buffer.data.size);
        std::size_t num_to_copy = std::min(m_buffer.heap_buffer.size,
                                           other.m_buffer.heap_buffer.size);
        std::copy_n(other.m_buffer.heap_buffer.data.ptr, num_to_copy,
                    m_buffer.heap_buffer.heap_buffer.data.ptr);
        std::fill(m_buffer.heap_buffer.data.ptr + num_to_copy,
                  m_buffer.heap_buffer.data.ptr +
                      m_buffer.heap_buffer.data.size,
                  char{0});
      }
    }

    return *this;
  }

  CharStringBuffer &operator=(CharStringBuffer &&other) noexcept {
    std::swap(m_is_short_string, other.m_is_short_string);
    std::swap(m_buffer, other.m_buffer);
    return *this;
  }

  constexpr char *get() {
    if (m_is_short_string) {
      return &m_buffer.stack_buffer;
    }
    return m_buffer.heap_buffer.data.ptr;
  }
  [[nodiscard]] constexpr const char *get() const {
    // TODO(fk): return "" if empty string
    if (m_is_short_string) {
      return &m_buffer.stack_buffer;
    }
    return m_buffer.heap_buffer.data.ptr;
  }

  [[nodiscard]] constexpr std::size_t get_capacity() const {
    if (m_is_short_string) {
      return ShortStringLength;
    }
    return m_buffer.heap_buffer.data.size;
  }
};

} // namespace Detail

template <IsAllocator Allocator> class CharString final {
  using Buffer = Detail::CharStringBuffer<Allocator>;

private:
  Buffer m_buffer;

public:
  CharString(const CharString &) = default;
  CharString(CharString &&) = default;
  CharString &operator=(const CharString &) = default;
  CharString &operator=(CharString &&) = default;
  ~CharString() = default;

  CharString(std::size_t length, char character) : m_buffer(length) {
    std::fill_n(m_buffer.get(), length, character);
  }
};

} // namespace CppPlayground

#endif
