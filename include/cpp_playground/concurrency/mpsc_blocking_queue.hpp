#ifndef CPP_PLAYGROUND_CONCURRENCY_MPMC_BLOCKING_QUEUE_HPP
#define CPP_PLAYGROUND_CONCURRENCY_MPMC_BLOCKING_QUEUE_HPP

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <queue>
#include <utility>

namespace CppPlayground {

template <typename T> class BoundedBlockingQueue {
private:
  std::queue<T> m_queue;
  std::mutex m_mutex;
  std::condition_variable m_cv_push;
  std::condition_variable m_cv_pop;
  std::size_t m_capacity;


public:
  explicit BoundedBlockingQueue(std::size_t capacity) : m_capacity(capacity) {}

  template <typename V>
  void push(V&& value) {
    std::unique_lock lock(m_mutex);
    m_cv_push.wait(lock, [this] { return m_queue.size() < m_capacity; });
    m_queue.push(std::forward(value));
    lock.unlock(); // unlock mutex before notifying other threads, prevents
                    // unneccessary wake-ups due to the mutex still being locked
                    // when they could start work, would be safe to omit this
                    // line just less efficient
    m_cv_pop.notify_one();
  }

  template <typename... Args>
  void emplace(Args&&... args) {
      std::unique_lock lock(m_mutex);
      m_cv_push.wait(lock, [this] { return m_queue.size() < m_capacity; });
      m_queue.emplace(std::forward<Args>(args)...);
      lock.unlock(); // unlock mutex before notifying other threads, prevents
                      // unneccessary wake-ups due to the mutex still being locked
                      // when they could start work, would be safe to omit this
                      // line just less efficient
      m_cv_pop.notify_one();
  }

  T pop() {
    std::unique_lock lock(m_mutex);
    m_cv_pop.wait(lock, [this] { return !m_queue.empty(); });
    T value = std::move(m_queue.front());
    m_queue.pop();
    lock.unlock(); // same consideration as above
    m_cv_push.notify_one();
    return value;
  }
};

} // namespace CppPlayground

#endif
