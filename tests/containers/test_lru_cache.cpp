#include <catch2/catch_test_macros.hpp>
#include <cpp_playground/containers/lru_cache.hpp>
#include <string>

TEST_CASE("LRUCache", "[containers]") {
  using CppPlayground::LRUCache;

  SECTION("Grow and evict") {
    LRUCache<int, std::string> cache(4);
    REQUIRE(cache.insert(0, "0") == "0");
    REQUIRE(*cache.get(0) == "0");
    REQUIRE(cache.size() == 1);

    REQUIRE(cache.insert(1, "1") == "1");
    REQUIRE(*cache.get(1) == "1");
    REQUIRE(cache.size() == 2);

    REQUIRE(cache.insert(2, "2") == "2");
    REQUIRE(*cache.get(2) == "2");
    REQUIRE(cache.size() == 3);

    REQUIRE(cache.insert(3, "3") == "3");
    REQUIRE(*cache.get(3) == "3");
    REQUIRE(cache.size() == 4);

    // element 0 will be evicted
    REQUIRE(cache.insert(4, "4") == "4");
    REQUIRE(*cache.get(4) == "4");
    REQUIRE(!cache.get(0).has_value());
    REQUIRE(cache.size() == 4);

    // test overwriting
    REQUIRE(cache.insert(4, "3") == "3");
    REQUIRE(*cache.get(4) == "3");
    REQUIRE(cache.size() == 4);
  }

  SECTION("Test promotion by reading") {
      LRUCache<int, std::string> cache(4);
      REQUIRE(cache.insert(0, "0") == "0");
      REQUIRE(*cache.get(0) == "0");
      REQUIRE(cache.size() == 1);

      REQUIRE(cache.insert(1, "1") == "1");
      REQUIRE(*cache.get(1) == "1");
      REQUIRE(cache.size() == 2);

      REQUIRE(cache.insert(2, "2") == "2");
      REQUIRE(*cache.get(2) == "2");
      REQUIRE(cache.size() == 3);

      REQUIRE(cache.insert(3, "3") == "3");
      REQUIRE(*cache.get(3) == "3");
      REQUIRE(cache.size() == 4);

      // promote element 0
      REQUIRE(cache.get(0) == "0");

      // element 1 will be evicted
      REQUIRE(cache.insert(4, "4") == "4");
      REQUIRE(*cache.get(4) == "4");
      REQUIRE(*cache.get(0) == "0");
      REQUIRE(!cache.get(1).has_value());
      REQUIRE(cache.size() == 4);
  }

  SECTION("Test promotion by overwriting") {
      LRUCache<int, std::string> cache(4);
      REQUIRE(cache.insert(0, "0") == "0");
      REQUIRE(*cache.get(0) == "0");
      REQUIRE(cache.size() == 1);

      REQUIRE(cache.insert(1, "1") == "1");
      REQUIRE(*cache.get(1) == "1");
      REQUIRE(cache.size() == 2);

      REQUIRE(cache.insert(2, "2") == "2");
      REQUIRE(*cache.get(2) == "2");
      REQUIRE(cache.size() == 3);

      REQUIRE(cache.insert(3, "3") == "3");
      REQUIRE(*cache.get(3) == "3");
      REQUIRE(cache.size() == 4);

      // promote element 0
      REQUIRE(cache.insert(0, "9") == "9");

      // element 1 will be evicted
      REQUIRE(cache.insert(4, "4") == "4");
      REQUIRE(*cache.get(4) == "4");
      REQUIRE(*cache.get(0) == "9");
      REQUIRE(!cache.get(1).has_value());
      REQUIRE(cache.size() == 4);
  }
}
