#include <CppPlayground/Testing/LifetimeTracker.hpp>

namespace CppPlayground::Testing {
void LifetimeTracker::reset() {
  m_num_constructions = 0UL;
  m_num_destructions = 0UL;
  m_num_default_constructions = 0UL;
  m_num_copy_constructions = 0UL;
  m_num_move_constructions = 0UL;
  m_num_copy_assignments = 0UL;
  m_num_move_assignments = 0UL;
}

LifetimeTracker::LifetimeTracker() noexcept {
  ++m_num_default_constructions;
  ++m_num_constructions;
}
LifetimeTracker::LifetimeTracker(
    [[maybe_unused]] const LifetimeTracker &other) noexcept {
  ++m_num_copy_constructions;
  ++m_num_constructions;
}
LifetimeTracker::LifetimeTracker(
    [[maybe_unused]] LifetimeTracker &&other) noexcept {
  ++m_num_move_constructions;
  ++m_num_constructions;
}
LifetimeTracker::~LifetimeTracker() { ++m_num_destructions; }

LifetimeTracker &
LifetimeTracker::operator=(const LifetimeTracker &other) noexcept {
  if (this == &other) {
    return *this;
  }
  ++m_num_copy_assignments;
  return *this;
}
LifetimeTracker &
LifetimeTracker::operator=([[maybe_unused]] LifetimeTracker &&other) noexcept {
  if (this == &other) {
    return *this;
  }
  ++m_num_move_assignments;
  return *this;
}

} // namespace CppPlayground::Testing
