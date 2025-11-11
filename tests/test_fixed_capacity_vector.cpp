#include <catch2/catch_test_macros.hpp>
#include <cpp_playground/fixed_capacity_vector.hpp>
#include <cstdint>

using namespace CppPlayground;

TEST_CASE("FixedCapacityVector: Constructors") {
  SECTION("Default Constructor") {
    FixedCapacityVector<uint64_t, 128UL> vector;
    REQUIRE(vector.size() == 0UL);
    REQUIRE(vector.capacity() == 128UL);
    REQUIRE(vector.is_empty());
    REQUIRE(!vector.is_full());
  }

  SECTION("Initializer List Constructor") {
    FixedCapacityVector<uint64_t, 128UL> vector = {1UL, 2UL, 3UL};
    REQUIRE(vector.size() == 3UL);
    REQUIRE(vector.capacity() == 128UL);
    REQUIRE(!vector.is_empty());
    REQUIRE(!vector.is_full());
    REQUIRE(vector.front() == 1UL);
    REQUIRE(vector.back() == 3UL);
  }
}
