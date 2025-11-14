#include <CppPlayground/testing/thread_local_lifetime_tracker.hpp>

namespace CppPlayground::Testing {
void ThreadLocalLifetimeTracker::reset() {
  m_num_constructions = 0UL;
  m_num_destructions = 0UL;
  m_num_default_constructions = 0UL;
  m_num_copy_constructions = 0UL;
  m_num_move_constructions = 0UL;
  m_num_copy_assignments = 0UL;
  m_num_move_assignments = 0UL;
}

ThreadLocalLifetimeTracker::ThreadLocalLifetimeTracker() noexcept {
  ++m_num_default_constructions;
  ++m_num_constructions;
}
ThreadLocalLifetimeTracker::ThreadLocalLifetimeTracker(
    [[maybe_unused]] const ThreadLocalLifetimeTracker &other) noexcept {
  ++m_num_copy_constructions;
  ++m_num_constructions;
}
ThreadLocalLifetimeTracker::ThreadLocalLifetimeTracker(
    [[maybe_unused]] ThreadLocalLifetimeTracker &&other) noexcept {
  ++m_num_move_constructions;
  ++m_num_constructions;
}
ThreadLocalLifetimeTracker::~ThreadLocalLifetimeTracker() {
  ++m_num_destructions;
}

ThreadLocalLifetimeTracker &ThreadLocalLifetimeTracker::operator=(
    const ThreadLocalLifetimeTracker &other) noexcept {
  if (this == &other) {
    return *this;
  }
  ++m_num_copy_assignments;
  return *this;
}
ThreadLocalLifetimeTracker &ThreadLocalLifetimeTracker::operator=(
    [[maybe_unused]] ThreadLocalLifetimeTracker &&other) noexcept {
  if (this == &other) {
    return *this;
  }
  ++m_num_move_assignments;
  return *this;
}

} // namespace CppPlayground::Testing
