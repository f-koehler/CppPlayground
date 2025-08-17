#ifndef CPPPLAYGROUND_ARC_POINTER_HPP
#define CPPPLAYGROUND_ARC_POINTER_HPP

#include <atomic>
#include <cpp_playground/deleter.hpp>
#include <cstddef>
#include <memory>
#include <type_traits>

namespace CppPlayground {

namespace Detail {

struct ControlBlockBase {
  void *m_ptr;
  // store deleter with erased type
  void (*m_deleter)(void *);
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

template <typename Deleter> struct ControlBlock : public ControlBlockBase {
  // store deleter with EBO
  [[no_unique_address]] Deleter m_deleter;

  explicit ControlBlock(void *ptr, const Deleter &deleter)
      : ControlBlockBase(ptr), m_deleter(deleter) {}
  explicit ControlBlock(void *ptr, Deleter &&deleter)
      : ControlBlockBase(ptr), m_deleter(deleter) {}

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
  ArcPointer(ArcPointer &&other)
      : m_control_block(other.m_control_block), m_ptr(other.m_ptr) {
    // no benefit of moving our small pointer types, other will be deconstructed
    // -> decrements ref count
  }

  // copy assignment
  ArcPointer &operator=(const ArcPointer &other) {
    // do nothing if pointing to the same control block
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
  ArcPointer &operator=(ArcPointer &&other) {
    if (m_control_block == other.m_control_block) {
      // pointing to the same control block, other will be deconstructed ->
      // decrements ref count
      return *this;
    }

    m_control_block = other.m_control_block;
    m_ptr = other.m_ptr;
  }

  virtual ~ArcPointer() {
    if (m_control_block != nullptr) {
      m_control_block->release_ref();
    }
  }

  ElementType *get() const noexcept { return m_ptr; }
};
} // namespace CppPlayground

#endif
