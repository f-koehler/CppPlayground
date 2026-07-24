#include <catch2/catch_test_macros.hpp>
#include <cpp_playground/utility/optional.hpp>
#include <optional>

TEST_CASE("Optional", "[utility]") {
  using CppPlayground::Optional;
  SECTION("Default construction") {
    Optional<int> opt;
    REQUIRE(!opt.has_value());
    REQUIRE_THROWS_AS(opt.value(), std::bad_optional_access);
  }
  SECTION("Default construction") {
    Optional<int> opt(1);
    REQUIRE(opt.has_value());
    REQUIRE(opt.value() == 1);
  }
}
