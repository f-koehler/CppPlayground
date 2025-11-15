#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <cpp_playground/fixed_capacity_vector.hpp>
#include <cpp_playground/testing/thread_local_lifetime_tracker.hpp>
#include <cstdint>
#include <iterator>
#include <ranges>
#include <stdexcept>

using namespace CppPlayground;
using namespace CppPlayground::Testing;

TEST_CASE("FixedCapacityVector: Constructors", "[containers]") {
  constexpr uint64_t Capacity = 3UL;
  SECTION("Default Constructor") {
    FixedCapacityVector<uint64_t, Capacity> vector;
    REQUIRE(vector.size() == 0UL);
    REQUIRE(vector.capacity() == Capacity);
    REQUIRE(vector.is_empty());
    REQUIRE(!vector.is_full());
  }

  SECTION("Initializer List Constructor") {
    SECTION("Nominal") {
      FixedCapacityVector<uint64_t, Capacity> vector = {1UL, 2UL, 3UL};
      REQUIRE(vector.size() == 3UL);
      REQUIRE(vector.capacity() == Capacity);
      REQUIRE(!vector.is_empty());
      REQUIRE(vector.is_full());
      REQUIRE(vector.front() == 1UL);
      REQUIRE(vector.back() == 3UL);
    }

    SECTION("Input too large") {
      REQUIRE_THROWS_AS(
          (FixedCapacityVector<uint64_t, Capacity>{1UL, 2UL, 3UL, 4UL}),
          std::length_error);
    }

    SECTION("Empty list") {
      FixedCapacityVector<uint64_t, Capacity> vector = {};
      REQUIRE(vector.size() == 0UL);
      REQUIRE(vector.capacity() == Capacity);
      REQUIRE(vector.is_empty());
      REQUIRE(!vector.is_full());
    }
  }

  SECTION("Copy Constructor") {
    FixedCapacityVector<uint64_t, Capacity> vector = {1UL, 2UL, 3UL};
    FixedCapacityVector<uint64_t, Capacity> copy(vector);
    REQUIRE(copy.size() == 3UL);
    REQUIRE(copy.capacity() == Capacity);
    REQUIRE(!copy.is_empty());
    REQUIRE(copy.is_full());
    REQUIRE(copy.front() == 1UL);
    REQUIRE(copy.back() == 3UL);
  }

  SECTION("Copy Constructor (different capacity)") {
    constexpr uint64_t OtherCapacity = Capacity + 1;
    SECTION("Input too large") {
      FixedCapacityVector<uint64_t, OtherCapacity> vector = {1UL, 2UL, 3UL,
                                                             4UL};
      REQUIRE_THROWS_AS((FixedCapacityVector<uint64_t, Capacity>(vector)),
                        std::length_error);
    }

    SECTION("Input fits") {
      FixedCapacityVector<uint64_t, OtherCapacity> vector = {1UL, 2UL, 3UL};
      FixedCapacityVector<uint64_t, Capacity> copy(vector);
      REQUIRE(copy.size() == 3UL);
      REQUIRE(copy.capacity() == Capacity);
      REQUIRE(!copy.is_empty());
      REQUIRE(copy.is_full());
      REQUIRE(copy.front() == 1UL);
      REQUIRE(copy.back() == 3UL);
    }
  }

  SECTION("Move constructor") {
    FixedCapacityVector<uint64_t, Capacity> vector = {1UL, 2UL, 3UL};
    FixedCapacityVector<uint64_t, Capacity> moved(std::move(vector));
    REQUIRE(moved.size() == 3UL);
    REQUIRE(moved.capacity() == Capacity);
    REQUIRE(!moved.is_empty());
    REQUIRE(moved.is_full());
    REQUIRE(moved.front() == 1UL);
    REQUIRE(moved.back() == 3UL);

    REQUIRE(vector.is_empty());
  }

  SECTION("Move Constructor (different capacity)") {
    constexpr uint64_t OtherCapacity = Capacity + 1;
    SECTION("Input too large") {
      FixedCapacityVector<uint64_t, OtherCapacity> vector = {1UL, 2UL, 3UL,
                                                             4UL};
      REQUIRE_THROWS_AS(
          (FixedCapacityVector<uint64_t, Capacity>(std::move(vector))),
          std::length_error);
    }

    SECTION("Input fits") {
      FixedCapacityVector<uint64_t, OtherCapacity> vector = {1UL, 2UL, 3UL};
      FixedCapacityVector<uint64_t, Capacity> moved(std::move(vector));
      REQUIRE(moved.size() == 3UL);
      REQUIRE(moved.capacity() == Capacity);
      REQUIRE(!moved.is_empty());
      REQUIRE(moved.is_full());
      REQUIRE(moved.front() == 1UL);
      REQUIRE(moved.back() == 3UL);

      REQUIRE(vector.is_empty());
    }
  }
}

TEST_CASE_METHOD(
    ThreadLocalLifetimeTrackerFixture,
    "FixedCapacityVector: default contructor with lifetime tracker",
    "[containers]") {
  constexpr uint64_t Capacity = 3UL;
  {
    const FixedCapacityVector<ThreadLocalLifetimeTracker, Capacity> vector;
    REQUIRE(vector.size() == 0UL);
    REQUIRE(vector.capacity() == Capacity);
    REQUIRE(vector.is_empty());
    REQUIRE(!vector.is_full());
    REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 0);
  }
  REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 0);
}

TEST_CASE_METHOD(
    ThreadLocalLifetimeTrackerFixture,
    "FixedCapacityVector: initializer list contructor with lifetime tracker",
    "[containers]") {
  constexpr uint64_t Capacity = 3UL;
  {
    const FixedCapacityVector<ThreadLocalLifetimeTracker, Capacity> vector = {
        ThreadLocalLifetimeTracker{}, ThreadLocalLifetimeTracker{},
        ThreadLocalLifetimeTracker{}};
    REQUIRE(vector.size() == 3UL);
    REQUIRE(vector.capacity() == Capacity);
    REQUIRE(!vector.is_empty());
    REQUIRE(vector.is_full());
    REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 6);
    REQUIRE(ThreadLocalLifetimeTracker::num_default_constructions() == 3);
    REQUIRE(ThreadLocalLifetimeTracker::num_copy_constructions() == 3);
  }
  REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 6);
}

TEST_CASE_METHOD(ThreadLocalLifetimeTrackerFixture,
                 "FixedCapacityVector: copy contructor with lifetime tracker",
                 "[containers]") {
  constexpr uint64_t Capacity = 3UL;
  const FixedCapacityVector<ThreadLocalLifetimeTracker, Capacity> vector = {
      ThreadLocalLifetimeTracker{}, ThreadLocalLifetimeTracker{},
      ThreadLocalLifetimeTracker{}};
  {
    ThreadLocalLifetimeTracker::reset();
    const FixedCapacityVector<ThreadLocalLifetimeTracker, Capacity> copy(
        vector);
    REQUIRE(copy.size() == 3UL);
    REQUIRE(copy.capacity() == Capacity);
    REQUIRE(!copy.is_empty());
    REQUIRE(copy.is_full());
    REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 3);
    REQUIRE(ThreadLocalLifetimeTracker::num_copy_constructions() == 3);
  }
  REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 3);
}

TEST_CASE_METHOD(ThreadLocalLifetimeTrackerFixture,
                 "FixedCapacityVector: move contructor with lifetime tracker",
                 "[containers]") {
  constexpr uint64_t Capacity = 3UL;
  FixedCapacityVector<ThreadLocalLifetimeTracker, Capacity> vector = {
      ThreadLocalLifetimeTracker{}, ThreadLocalLifetimeTracker{},
      ThreadLocalLifetimeTracker{}};
  {
    ThreadLocalLifetimeTracker::reset();
    const FixedCapacityVector<ThreadLocalLifetimeTracker, Capacity> moved(
        std::move(vector));
    REQUIRE(moved.size() == 3UL);
    REQUIRE(moved.capacity() == Capacity);
    REQUIRE(!moved.is_empty());
    REQUIRE(moved.is_full());
    REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 3);
    REQUIRE(ThreadLocalLifetimeTracker::num_move_constructions() == 3);
  }
  REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 6);
}

TEST_CASE("FixedCapacityVector: Element Accessors", "[containers]") {
  SECTION("at()") {
    SECTION("mutable vector") {
      FixedCapacityVector<uint64_t, 3UL> vector = {1UL, 2UL};
      REQUIRE(vector.at(0) == 1UL);
      REQUIRE(vector.at(1) == 2UL);
      REQUIRE_THROWS_AS(vector.at(2), std::out_of_range);
      REQUIRE_THROWS_AS(vector.at(3), std::out_of_range);
      REQUIRE_THROWS_AS(vector.at(4), std::out_of_range);
    }
    SECTION("const vector") {
      const FixedCapacityVector<uint64_t, 3UL> vector = {1UL, 2UL};
      REQUIRE(vector.at(0) == 1UL);
      REQUIRE(vector.at(1) == 2UL);
      REQUIRE_THROWS_AS(vector.at(2), std::out_of_range);
      REQUIRE_THROWS_AS(vector.at(3), std::out_of_range);
      REQUIRE_THROWS_AS(vector.at(4), std::out_of_range);
    }
  }
  SECTION("at()") {
    SECTION("mutable vector") {
      FixedCapacityVector<uint64_t, 3UL> vector = {1UL, 2UL};
      REQUIRE(vector.at(0) == 1UL);
      REQUIRE(vector.at(1) == 2UL);
    }
    SECTION("const vector") {
      const FixedCapacityVector<uint64_t, 3UL> vector = {1UL, 2UL};
      REQUIRE(vector.at(0) == 1UL);
      REQUIRE(vector.at(1) == 2UL);
    }
  }
}

TEST_CASE("FixedCapacityVector: Iterators", "[containers][iterators]") {
  SECTION("Forward Iteration") {
    SECTION("Mutable Vector") {
      FixedCapacityVector<uint64_t, 3UL> vector = {1UL, 2UL, 3UL};
      auto it = vector.begin();
      REQUIRE(*it == 1UL);
      ++it;
      REQUIRE(*it == 2UL);
      ++it;
      REQUIRE(*it == 3UL);
      ++it;
      REQUIRE(it == vector.end());
    }
    SECTION("Mutable Vector (const iter)") {
      FixedCapacityVector<uint64_t, 3UL> vector = {1UL, 2UL, 3UL};
      auto it = vector.cbegin();
      REQUIRE(*it == 1UL);
      ++it;
      REQUIRE(*it == 2UL);
      ++it;
      REQUIRE(*it == 3UL);
      ++it;
      REQUIRE(it == vector.cend());
    }
    SECTION("Mutable Vector") {
      const FixedCapacityVector<uint64_t, 3UL> vector = {1UL, 2UL, 3UL};
      auto it = vector.begin();
      REQUIRE(*it == 1UL);
      ++it;
      REQUIRE(*it == 2UL);
      ++it;
      REQUIRE(*it == 3UL);
      ++it;
      REQUIRE(it == vector.end());
    }
  }
  SECTION("Reverse Iteration") {
    SECTION("Mutable Vector") {
      FixedCapacityVector<uint64_t, 3UL> vector = {1UL, 2UL, 3UL};
      auto it = vector.rbegin();
      REQUIRE(*it == 3UL);
      ++it;
      REQUIRE(*it == 2UL);
      ++it;
      REQUIRE(*it == 1UL);
      ++it;
      REQUIRE(it == vector.rend());
    }
    SECTION("Mutable Vector (const iter)") {
      FixedCapacityVector<uint64_t, 3UL> vector = {1UL, 2UL, 3UL};
      auto it = vector.crbegin();
      REQUIRE(*it == 3UL);
      ++it;
      REQUIRE(*it == 2UL);
      ++it;
      REQUIRE(*it == 1UL);
      ++it;
      REQUIRE(it == vector.crend());
    }
    SECTION("Mutable Vector") {
      const FixedCapacityVector<uint64_t, 3UL> vector = {1UL, 2UL, 3UL};
      auto it = vector.rbegin();
      REQUIRE(*it == 3UL);
      ++it;
      REQUIRE(*it == 2UL);
      ++it;
      REQUIRE(*it == 1UL);
      ++it;
      REQUIRE(it == vector.rend());
    }
  }
}

TEST_CASE("FixedCapacityVector: Modification Through Iterators",
          "[containers][iterators]") {
  FixedCapacityVector<uint64_t, 3UL> vector = {1UL, 2UL, 3UL};
  std::transform(vector.begin(), vector.end(), vector.begin(),
                 [](uint64_t value) { return value * 2; });
  REQUIRE(vector.at(0) == 2UL);
  REQUIRE(vector.at(1) == 4UL);
  REQUIRE(vector.at(2) == 6UL);

  FixedCapacityVector<uint64_t, 3UL> copy;
  std::copy(vector.cbegin(), vector.cend(), std::back_inserter(copy));
  REQUIRE(copy.at(0) == 2UL);
  REQUIRE(copy.at(1) == 4UL);
  REQUIRE(copy.at(2) == 6UL);
  REQUIRE(copy.size() == 3UL);

  std::ranges::reverse(copy);
  REQUIRE(copy.at(0) == 6UL);
  REQUIRE(copy.at(1) == 4UL);
  REQUIRE(copy.at(2) == 2UL);

  std::ranges::sort(copy);
  REQUIRE(copy.at(0) == 2UL);
  REQUIRE(copy.at(1) == 4UL);
  REQUIRE(copy.at(2) == 6UL);
}
