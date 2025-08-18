#ifndef CPPPLAYGROUND_ARC_POINTER_HPP
#define CPPPLAYGROUND_ARC_POINTER_HPP

#include "./deleter.hpp"
#include <atomic>
#include <cstddef>
#include <memory>
#include <type_traits>

namespace CppPlayground {

template <typename T> class ArcPointer;

namespace Detail {

struct ControlBlockBase {
  void *m_ptr;
  std::atomic_uint64_t m_num_shared;

  explicit ControlBlockBase(void *ptr) : m_ptr(ptr), m_num_shared{1} {}
  virtual ~ControlBlockBase() = default;

  virtual void destroy_object() = 0;
  void add_ref() { ++m_num_shared; }
  void release_ref() {
    if ((--m_num_shared) == 0) {
      if (m_ptr != nullptr) {
        destroy_object();
      }
      delete this;
    }
  }
};

template <IsDeleter Deleter> struct ControlBlock : public ControlBlockBase {
  // store deleter with EBO
  [[no_unique_address]] Deleter m_deleter;

  explicit ControlBlock(void *ptr, const Deleter &deleter)
      : ControlBlockBase(ptr), m_deleter(deleter) {}
  explicit ControlBlock(void *ptr, Deleter &&deleter)
      : ControlBlockBase(ptr), m_deleter(std::move(deleter)) {}

  void destroy_object() override { m_deleter(m_ptr); }
};

} // namespace Detail

template <typename T> class ArcPointer {
public:
  using ElementType = std::remove_extent_t<T>;

private:
  Detail::ControlBlockBase *m_control_block;
  ElementType *m_ptr;

public:
  // default constructor
  template <typename Deleter = DefaultDelete<T>>
  explicit constexpr ArcPointer() noexcept
      : m_control_block(
            new Detail::ControlBlock<Deleter>(nullptr, DefaultDelete<T>{})),
        m_ptr(nullptr) {}

  template <typename Deleter = DefaultDelete<T>>
  explicit constexpr ArcPointer(ElementType *ptr,
                                Deleter deleter = Deleter{}) noexcept
      : m_control_block(
            new Detail::ControlBlock<Deleter>((void *)ptr, deleter)),
        m_ptr(ptr) {}

  // copy constructor
  ArcPointer(const ArcPointer &other)
      : m_control_block(other.m_control_block), m_ptr(other.m_ptr) {
    m_control_block->add_ref();
  }

  // move constructor
  ArcPointer(ArcPointer &&other) noexcept
      : m_control_block(other.m_control_block), m_ptr(other.m_ptr) {
    other.m_control_block = nullptr;
    other.m_ptr = nullptr;
  }

  // copy assignment
  ArcPointer &operator=(const ArcPointer &other) {
    if (this == &other) {
      return *this;
    }
    if (other.m_control_block == m_control_block) {
      return *this;
    }

    other.m_control_block->add_ref();
    m_control_block->release_ref();
    m_control_block = other.m_control_block;
    m_ptr = other.m_ptr;
    return *this;
  }

  // move assignment
  ArcPointer &operator=(ArcPointer &&other) noexcept {
    m_control_block->release_ref();
    m_control_block = other.m_control_block;
    m_ptr = other.m_ptr;
    other.m_control_block = nullptr;
    other.m_control_block = nullptr;
  }

  virtual ~ArcPointer() {
    if (m_control_block != nullptr) {
      m_control_block->release_ref();
    }
  }

  [[nodiscard]] ElementType *get() const noexcept { return m_ptr; }
};
} // namespace CppPlayground

#endif
