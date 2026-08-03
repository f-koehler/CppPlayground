#include <catch2/catch_test_macros.hpp>
#include <cpp_playground/containers/fixed_capacity_queue.hpp>
#include <optional>

TEST_CASE("FixedCapacityQueue", "[containers]") {
  using CppPlayground::FixedCapacityQueue;
  static constexpr std::size_t capacity = 5;

  SECTION("Properties of empty queue") {
    FixedCapacityQueue<int> queue(capacity);
    REQUIRE(queue.size() == 0);
    REQUIRE(queue.capacity() == capacity);
    REQUIRE(queue.empty());
    REQUIRE(!queue.full());
  }

  SECTION("Pushing elements") {
    FixedCapacityQueue<int> queue(capacity);

    for (std::size_t i = 0; i < capacity; ++i) {
      REQUIRE(queue.push(i));
      REQUIRE(queue.size() == i + 1);
      REQUIRE(!queue.empty());
    }
    REQUIRE(queue.full());
    REQUIRE(!queue.push(5));
  }

  SECTION("Emplacing elements") {
    struct S {
      double a;
      int b;
    };
    FixedCapacityQueue<S> queue(capacity);

    for (std::size_t i = 0; i < capacity; ++i) {
      REQUIRE(queue.emplace(3.14 * i, i));
      REQUIRE(queue.size() == i + 1);
      REQUIRE(!queue.empty());
    }
    REQUIRE(queue.full());
    REQUIRE(!queue.emplace(42.24, 5));
  }

  SECTION("Popping elements") {
    FixedCapacityQueue<int> queue(capacity);
    REQUIRE(queue.pop() == std::nullopt);

    for (std::size_t i = 0; i < capacity; ++i) {
      queue.push(i);
    }

    for (std::size_t i = 0; i < capacity; ++i) {
      REQUIRE(queue.pop() == i);
      REQUIRE(queue.size() == capacity - (i + 1));
    }
    REQUIRE(queue.empty());
    REQUIRE(queue.size() == 0);
    REQUIRE(queue.pop() == std::nullopt);
  }
}
