#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <cpp_playground/containers/fixed_capacity_vector.hpp>
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
    "FixedCapacityVector: default constructor with lifetime tracker",
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
    "FixedCapacityVector: initializer list constructor with lifetime tracker",
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
                 "FixedCapacityVector: copy constructor with lifetime tracker",
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
                 "FixedCapacityVector: move constructor with lifetime tracker",
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
      REQUIRE(vector[0] == 1UL);
      REQUIRE(vector[1] == 2UL);
    }
    SECTION("const vector") {
      const FixedCapacityVector<uint64_t, 3UL> vector = {1UL, 2UL};
      REQUIRE(vector[0] == 1UL);
      REQUIRE(vector[1] == 2UL);
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

TEST_CASE("FixedCapacityVector: Modifiers (uint64_t)", "[containers]") {
  constexpr uint64_t Capacity = 3UL;
  FixedCapacityVector<uint64_t, Capacity> vector;

  SECTION("push_back l-value, r-value, and emplace_back") {
    uint64_t value = 42;
    vector.push_back(value);
    REQUIRE(vector.size() == 1);
    REQUIRE(vector.back() == 42);

    vector.push_back(1337);
    REQUIRE(vector.size() == 2);
    REQUIRE(vector.back() == 1337);

    REQUIRE(vector.emplace_back(9001) == 9001);
    REQUIRE(vector.size() == 3);
    REQUIRE(vector.back() == 9001);
    REQUIRE(vector.is_full());
  }

  SECTION("pop_back") {
    vector = {1, 2, 3};
    REQUIRE(!vector.is_empty());

    vector.pop_back();
    REQUIRE(vector.size() == 2);
    REQUIRE(vector.back() == 2);

    vector.pop_back();
    REQUIRE(vector.size() == 1);
    REQUIRE(vector.back() == 1);

    vector.pop_back();
    REQUIRE(vector.is_empty());
  }

  SECTION("Push/Emplace on full vector throws") {
    vector = {1, 2, 3};
    REQUIRE(vector.is_full());
    uint64_t value = 4;
    REQUIRE_THROWS_AS(vector.push_back(value), std::length_error);
    REQUIRE_THROWS_AS(vector.push_back(4), std::length_error);
    REQUIRE_THROWS_AS(vector.emplace_back(4), std::length_error);
  }

  vector.clear();
  SECTION("Pop on empty vector throws") {
    REQUIRE_THROWS_AS(vector.pop_back(), std::out_of_range);
  }
}

TEST_CASE_METHOD(ThreadLocalLifetimeTrackerFixture,
                 "FixedCapacityVector: push_back (l-value)", "[containers]") {
  constexpr uint64_t Capacity = 3UL;

  {
    FixedCapacityVector<ThreadLocalLifetimeTracker, Capacity> vector;
    ThreadLocalLifetimeTracker tracker;
    REQUIRE(ThreadLocalLifetimeTracker::num_default_constructions() == 1);
    vector.push_back(tracker);
    REQUIRE(vector.size() == 1);
    REQUIRE(ThreadLocalLifetimeTracker::num_copy_constructions() == 1);
    REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 2);
    REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 0);
  }
  REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 2);
}

TEST_CASE_METHOD(ThreadLocalLifetimeTrackerFixture,
                 "FixedCapacityVector: push_back (r-value)", "[containers]") {
  constexpr uint64_t Capacity = 3UL;

  {
    FixedCapacityVector<ThreadLocalLifetimeTracker, Capacity> vector;
    vector.push_back(ThreadLocalLifetimeTracker{});
    REQUIRE(ThreadLocalLifetimeTracker::num_default_constructions() == 1);
    REQUIRE(vector.size() == 1);
    REQUIRE(ThreadLocalLifetimeTracker::num_move_constructions() == 1);
    REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 2);
  }
  REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 2);
}

TEST_CASE_METHOD(ThreadLocalLifetimeTrackerFixture,
                 "FixedCapacityVector: emplace_back (r-value)",
                 "[containers]") {
  constexpr uint64_t Capacity = 3UL;

  {
    FixedCapacityVector<ThreadLocalLifetimeTracker, Capacity> vector;
    vector.emplace_back();
    REQUIRE(ThreadLocalLifetimeTracker::num_default_constructions() == 1);
    REQUIRE(vector.size() == 1);
    REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 1);
  }
  REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 1);
}

TEST_CASE_METHOD(ThreadLocalLifetimeTrackerFixture,
                 "FixedCapacityVector: pop_back",
                 "[containers]") {
  constexpr uint64_t Capacity = 3;
  FixedCapacityVector<ThreadLocalLifetimeTracker, Capacity> vector;
  vector.emplace_back();
  vector.emplace_back();
  vector.emplace_back();
  REQUIRE(vector.is_full());
  REQUIRE(ThreadLocalLifetimeTracker::num_default_constructions() == 3);
  REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 0);

  vector.pop_back();
  REQUIRE(vector.size() == 2);
  REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 1);

  vector.pop_back();
  REQUIRE(vector.size() == 1);
  REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 2);

  vector.pop_back();
  REQUIRE(vector.is_empty());
  REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 3);
}
