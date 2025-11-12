#include <catch2/catch_test_macros.hpp>
#include <cpp_playground/fixed_capacity_vector.hpp>
#include <cstdint>
#include <stdexcept>

using namespace CppPlayground;

TEST_CASE("FixedCapacityVector: Constructors") {
  constexpr uint64_t Capacity = 3UL;
  SECTION("Default Constructor") {
    FixedCapacityVector<uint64_t, Capacity> vector;
    REQUIRE(vector.size() == 0UL);
    REQUIRE(vector.capacity() == Capacity);
    REQUIRE(vector.is_empty());
    REQUIRE(!vector.is_full());
  }

  SECTION("Initializer List Constructor") {
    FixedCapacityVector<uint64_t, Capacity> vector = {1UL, 2UL, 3UL};
    REQUIRE(vector.size() == 3UL);
    REQUIRE(vector.capacity() == Capacity);
    REQUIRE(!vector.is_empty());
    REQUIRE(vector.is_full());
    REQUIRE(vector.front() == 1UL);
    REQUIRE(vector.back() == 3UL);
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
  }
}
