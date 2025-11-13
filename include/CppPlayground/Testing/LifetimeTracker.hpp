#ifndef CPP_PLAYGROUND_TESTING_LIFETIMETRACKER_HPP
#define CPP_PLAYGROUND_TESTING_LIFETIMETRACKER_HPP

#include <cstdint>

namespace CppPlayground::Testing {
/**
 * @brief A testing utility to track the lifetime events of objects.
 * @details This class provides static counters for constructions, destructions,
 * copies, and moves. The counters are thread-local, ensuring that tests
 * running in parallel do not interfere with each other. It is designed to be
 * used as a template parameter in data structures to verify their correctness
 * in managing object lifetimes.
 */
class LifetimeTracker {
private:
  thread_local inline static int64_t m_num_constructions = 0UL;
  thread_local inline static int64_t m_num_destructions = 0UL;
  thread_local inline static int64_t m_num_default_constructions = 0UL;
  thread_local inline static int64_t m_num_copy_constructions = 0UL;
  thread_local inline static int64_t m_num_move_constructions = 0UL;
  thread_local inline static int64_t m_num_copy_assignments = 0UL;
  thread_local inline static int64_t m_num_move_assignments = 0UL;

public:
  /**
   * @brief Resets all lifetime event counters to zero.
   */
  static void reset();

  /**
   * @brief Default constructor. Increments the default construction counter.
   */
  LifetimeTracker() noexcept;
  /**
   * @brief Copy constructor. Increments the copy construction counter.
   */
  LifetimeTracker(const LifetimeTracker &other) noexcept;
  /**
   * @brief Move constructor. Increments the move construction counter.
   */
  LifetimeTracker(LifetimeTracker &&other) noexcept;
  /**
   * @brief Destructor. Increments the destruction counter.
   */
  ~LifetimeTracker();
  /**
   * @brief Copy assignment operator. Increments the copy assignment counter.
   */
  LifetimeTracker &operator=(const LifetimeTracker &other) noexcept;
  /**
   * @brief Move assignment operator. Increments the move assignment counter.
   */
  LifetimeTracker &operator=(LifetimeTracker &&other) noexcept;

  /**
   * @brief Returns the total number of constructions (default, copy, and move).
   */
  static int64_t num_constructions() noexcept {
    return m_num_constructions;
  }
  /**
   * @brief Returns the total number of destructions.
   */
  static int64_t num_destructions() noexcept {
    return m_num_destructions;
  }
  /**
   * @brief Returns the number of default constructions.
   */
  static int64_t num_default_constructions() noexcept {
    return m_num_default_constructions;
  }
  /**
   * @brief Returns the number of copy constructions.
   */
  static int64_t num_copy_constructions() noexcept {
    return m_num_copy_constructions;
  }
  /**
   * @brief Returns the number of move constructions.
   */
  static int64_t num_move_constructions() noexcept {
    return m_num_move_constructions;
  }
  /**
   * @brief Returns the number of copy assignments.
   */
  static int64_t num_copy_assignments() noexcept {
    return m_num_copy_assignments;
  }
  /**
   * @brief Returns the number of move assignments.
   */
  static int64_t num_move_assignments() noexcept {
    return m_num_move_assignments;
  }
};
} // namespace CppPlayground::Testing

#endif
