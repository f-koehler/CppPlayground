#ifndef CPP_PLAYGROUND_TESTING_LIFETIMETRACKER_HPP
#define CPP_PLAYGROUND_TESTING_LIFETIMETRACKER_HPP

#include <cstdint>

namespace CppPlayground::Testing {
/**
 * @brief A testing utility to track the lifetime events of objects.
 * @details This class provides static counters for constructions, destructions,
 * copies, and moves. The counters are thread-local, ensuring that tests
 * running in parallel do not interfere with each other.
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
  constexpr static void reset();

  /**
   * @brief Default constructor. Increments the default construction counter.
   */
  constexpr LifetimeTracker() noexcept;
  /**
   * @brief Copy constructor. Increments the copy construction counter.
   */
  constexpr LifetimeTracker(const LifetimeTracker &other) noexcept;
  /**
   * @brief Move constructor. Increments the move construction counter.
   */
  constexpr LifetimeTracker(LifetimeTracker &&other) noexcept;
  /**
   * @brief Destructor. Increments the destruction counter.
   */
  constexpr ~LifetimeTracker();
  /**
   * @brief Copy assignment operator. Increments the copy assignment counter.
   */
  constexpr LifetimeTracker &operator=(const LifetimeTracker &other) noexcept;
  /**
   * @brief Move assignment operator. Increments the move assignment counter.
   */
  constexpr LifetimeTracker &operator=(LifetimeTracker &&other) noexcept;

  /**
   * @brief Returns the total number of constructions (default, copy, and move).
   */
  constexpr static int64_t num_constructions() noexcept {
    return m_num_constructions;
  }
  /**
   * @brief Returns the total number of destructions.
   */
  constexpr static int64_t num_destructions() noexcept {
    return m_num_destructions;
  }
  /**
   * @brief Returns the number of default constructions.
   */
  constexpr static int64_t num_default_constructions() noexcept {
    return m_num_default_constructions;
  }
  /**
   * @brief Returns the number of copy constructions.
   */
  constexpr static int64_t num_copy_constructions() noexcept {
    return m_num_copy_constructions;
  }
  /**
   * @brief Returns the number of move constructions.
   */
  constexpr static int64_t num_move_constructions() noexcept {
    return m_num_move_constructions;
  }
  /**
   * @brief Returns the number of copy assignments.
   */
  constexpr static int64_t num_copy_assignments() noexcept {
    return m_num_copy_assignments;
  }
  /**
   * @brief Returns the number of move assignments.
   */
  constexpr static int64_t num_move_assignments() noexcept {
    return m_num_move_assignments;
  }
};
} // namespace CppPlayground::Testing

#endif
