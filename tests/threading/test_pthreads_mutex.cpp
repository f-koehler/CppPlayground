#include <catch2/catch_test_macros.hpp>
#include <cpp_playground/threading/pthreads_mutex.hpp>
#include <mutex>
#include <system_error>

TEST_CASE("PThreadsMutex", "[threading]") {
  using CppPlayground::PThreadsMutex;

  SECTION("Lock and unlock") {
    PThreadsMutex mutex;
    REQUIRE_NOTHROW(mutex.lock());
    REQUIRE(!mutex.try_lock());
    REQUIRE_NOTHROW(mutex.unlock());
  }

  SECTION("try_lock() twice") {
    PThreadsMutex mutex;
    REQUIRE(mutex.try_lock());
    REQUIRE(!mutex.try_lock());
    REQUIRE_NOTHROW(mutex.unlock());
  }

  SECTION("Deadlock prevention") {
    PThreadsMutex mutex;
    REQUIRE_NOTHROW(mutex.lock());
    REQUIRE_THROWS_AS(mutex.lock(), std::system_error);
  }

  SECTION("STL Compatibility") {
    PThreadsMutex mutex;
    {
      std::unique_lock lock(mutex);
      REQUIRE(!mutex.try_lock());
    }
    REQUIRE(mutex.try_lock());
    REQUIRE_NOTHROW(mutex.unlock());
  }
}
