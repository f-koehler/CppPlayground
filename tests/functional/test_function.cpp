#include <catch2/catch_test_macros.hpp>
#include <cpp_playground/functional/function.hpp>
#include <functional>

using CppPlayground::Function;

struct DoubleFunctor {
  constexpr int operator()(int x) const noexcept { return x * 2; }
};

constexpr int double_function(int x) noexcept { return x * 2; }

TEST_CASE("Function", "[functional]") {

  SECTION("Construction") {
    SECTION("Default Construction") {
      Function<int(int)> func;
      REQUIRE_THROWS_AS(func(42), std::bad_function_call);
    }
    SECTION("Move Construct From Lambda") {
      Function<int(int)> func([](int x) { return x * 2; });
      REQUIRE(func(21) == 42);
    }

    SECTION("Copy Construct From Lambda") {
      const auto lambda = [](int x) { return x * 2; };
      Function<int(int)> func(lambda);
      REQUIRE(func(21) == 42);
    }

    SECTION("Move Construct From Functor") {
      Function<int(int)> func(DoubleFunctor{});
      REQUIRE(func(21) == 42);
    }

    SECTION("Copy Construct From Functor") {
      const DoubleFunctor functor;
      Function<int(int)> func(functor);
      REQUIRE(func(21) == 42);
    }

    SECTION("Construct From Function") {
      Function<int(int)> func(double_function);
      REQUIRE(func(21) == 42);
    }
  }

  SECTION("Assignment") {
    SECTION("Move Assign Lambda") {
      Function<int(int)> func;
      func = [](int x) { return x * 2; };
      REQUIRE(func(21) == 42);
    }

    SECTION("Copy Assign Lambda") {
      Function<int(int)> func;
      const auto lambda = [](int x) { return x * 2; };
      func = lambda;
      REQUIRE(func(21) == 42);
    }

    SECTION("Move Assign Functor") {
      Function<int(int)> func;
      func = DoubleFunctor{};
      REQUIRE(func(21) == 42);
    }

    SECTION("Copy Assign Functor") {
      Function<int(int)> func;
      const DoubleFunctor functor;
      func = functor;
      REQUIRE(func(21) == 42);
    }

    SECTION("Assign Function") {
      Function<int(int)> func;
      func = double_function;
      REQUIRE(func(21) == 42);
    }
  }
}
