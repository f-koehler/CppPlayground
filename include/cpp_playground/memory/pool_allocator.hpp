#ifndef CPP_PLAYGROUND_CONCURRENCY_MPMC_BLOCKING_QUEUE_HPP
#define CPP_PLAYGROUND_CONCURRENCY_MPMC_BLOCKING_QUEUE_HPP

#include <cstddef>
#include <memory>
#include <new>

namespace CppPlayground {

template <typename T> class MemoryPool {
public:
  struct Slot {
    union {
      alignas(T) std::byte storage[sizeof(T)];
      Slot *next;
    };
  };

private:
  std::size_t m_capacity;
  std::unique_ptr<Slot[]> m_slots;
  Slot *m_next_free;

  Slot *slot_from_object(T *ptr) {
    return reinterpret_cast<Slot *>(reinterpret_cast<std::byte *>(ptr) -
                                    offsetof(Slot, storage));
  }

public:
  explicit MemoryPool(std::size_t capacity)
      : m_capacity(capacity), m_slots(std::make_unique<Slot[]>(capacity)),
        m_next_free(m_slots.get()) {
    for (std::size_t i = 0; i < m_capacity; ++i) {
      m_slots[i].next = (i + 1 < m_capacity) ? &m_slots[i + 1] : nullptr;
    }
    m_next_free = (m_capacity > 0) ? m_slots.get() : nullptr;
  }
  MemoryPool(const MemoryPool &) = delete;
  MemoryPool(MemoryPool &&) = delete;
  MemoryPool &operator=(const MemoryPool &) = delete;
  MemoryPool &operator=(MemoryPool &&) = delete;
  ~MemoryPool() = default;

  // no desctrutor: m_slots automatically deallocated through std::unique_ptr,
  // responsibility for deconstructing objects outside of allocator

  template <typename... Args> T *allocate(Args &&...args) {
    if (m_next_free == nullptr) {
      // no free slot, allocation fails
      throw std::bad_alloc();
    }

    // construct object with placement new
    T *object = new (&(m_next_free->storage)) T(std::forward<Args>(args)...);

    // only update m_next_free after successful construction -> ensure
    // consistent state
    m_next_free = m_next_free->next;

    return object;
  }

  void deallocate(T *ptr) {
    if (ptr == nullptr) {
      return; // deallocation of nullptr is no-op
    }

    ptr->~T();
    Slot *slot = slot_from_object(ptr);
    slot->next = m_next_free;
    m_next_free = slot;
  }
};

} // namespace CppPlayground

#endif
