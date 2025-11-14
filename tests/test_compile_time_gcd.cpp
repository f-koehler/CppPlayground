#include <catch2/catch_test_macros.hpp>
#include <cpp_playground/compile_time_gcd.hpp>

using namespace CppPlayground;

TEST_CASE("compile_time_gcd", "[CompileTime]") {
  REQUIRE(compile_time_gcd<0, 0> == 0);
  REQUIRE(compile_time_gcd<1, 0> == 1);
  REQUIRE(compile_time_gcd<0, 1> == 1);
  REQUIRE(compile_time_gcd<5, 0> == 5);
  REQUIRE(compile_time_gcd<5, 5> == 5);
  REQUIRE(compile_time_gcd<5, 3> == 1);
  REQUIRE(compile_time_gcd<0, 5> == 5);
  REQUIRE(compile_time_gcd<3, 5> == 1);
  REQUIRE(compile_time_gcd<10, 5> == 5);
  REQUIRE(compile_time_gcd<5, 10> == 5);
}

TEST_CASE("compile_time_gcd_signed", "[CompileTime]") {
  REQUIRE(compile_time_gcd_signed<0, 0> == 0);
  REQUIRE(compile_time_gcd_signed<1, 0> == 1);
  REQUIRE(compile_time_gcd_signed<0, 1> == 1);
  REQUIRE(compile_time_gcd_signed<-1, 0> == 1);
  REQUIRE(compile_time_gcd_signed<0, -1> == 1);
  REQUIRE(compile_time_gcd_signed<5, 0> == 5);
  REQUIRE(compile_time_gcd_signed<-5, 0> == 5);
  REQUIRE(compile_time_gcd_signed<5, 5> == 5);
  REQUIRE(compile_time_gcd_signed<-5, 5> == 5);
  REQUIRE(compile_time_gcd_signed<5, -5> == 5);
  REQUIRE(compile_time_gcd_signed<-5, -5> == 5);
  REQUIRE(compile_time_gcd_signed<5, 3> == 1);
  REQUIRE(compile_time_gcd_signed<-5, 3> == 1);
  REQUIRE(compile_time_gcd_signed<5, -3> == 1);
  REQUIRE(compile_time_gcd_signed<-5, -3> == 1);
  REQUIRE(compile_time_gcd_signed<0, 5> == 5);
  REQUIRE(compile_time_gcd_signed<0, -5> == 5);
  REQUIRE(compile_time_gcd_signed<3, 5> == 1);
  REQUIRE(compile_time_gcd_signed<-3, 5> == 1);
  REQUIRE(compile_time_gcd_signed<3, -5> == 1);
  REQUIRE(compile_time_gcd_signed<-3, -5> == 1);
  REQUIRE(compile_time_gcd_signed<10, 5> == 5);
  REQUIRE(compile_time_gcd_signed<10, -5> == 5);
  REQUIRE(compile_time_gcd_signed<-10, 5> == 5);
  REQUIRE(compile_time_gcd_signed<-10, -5> == 5);
  REQUIRE(compile_time_gcd_signed<5, 10> == 5);
  REQUIRE(compile_time_gcd_signed<-5, 10> == 5);
  REQUIRE(compile_time_gcd_signed<5, -10> == 5);
  REQUIRE(compile_time_gcd_signed<-5, -10> == 5);
}
