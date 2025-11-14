#include <CppPlayground/testing/thread_local_lifetime_tracker.hpp>
#include <catch2/catch_test_macros.hpp>
#include <thread>
#include <vector>

using CppPlayground::Testing::ThreadLocalLifetimeTracker;
using CppPlayground::Testing::ThreadLocalLifetimeTrackerFixture;

TEST_CASE_METHOD(ThreadLocalLifetimeTrackerFixture,
                 "ThreadLocalLifetimeTracker: count default constructions",
                 "[Testing]") {
  SECTION("Single object") {
    {
      ThreadLocalLifetimeTracker obj;
      REQUIRE(ThreadLocalLifetimeTracker::num_default_constructions() == 1);
      REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 1);
      REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 0);
    }
    REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 1);
  }

  SECTION("Multiple objects") {
    {
      ThreadLocalLifetimeTracker obj1;
      ThreadLocalLifetimeTracker obj2;
      ThreadLocalLifetimeTracker obj3;
      REQUIRE(ThreadLocalLifetimeTracker::num_default_constructions() == 3);
      REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 3);
      REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 0);
    }
    REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 3);
  }
}

TEST_CASE_METHOD(ThreadLocalLifetimeTrackerFixture,
                 "ThreadLocalLifetimeTracker: count copy constructions",
                 "[Testing]") {
  ThreadLocalLifetimeTracker original;
  REQUIRE(ThreadLocalLifetimeTracker::num_default_constructions() == 1);

  SECTION("Copy constructing") {
    {
      ThreadLocalLifetimeTracker copy(original);
      REQUIRE(ThreadLocalLifetimeTracker::num_copy_constructions() == 1);
      REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 2);
      REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 0);
    }
    REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 1);
  }
}

TEST_CASE_METHOD(ThreadLocalLifetimeTrackerFixture,
                 "ThreadLocalLifetimeTracker: count move constructions",
                 "[testing][thread-local]") {
  ThreadLocalLifetimeTracker original;
  REQUIRE(ThreadLocalLifetimeTracker::num_default_constructions() == 1);

  SECTION("Move constructing") {
    {
      ThreadLocalLifetimeTracker moved(std::move(original));
      REQUIRE(ThreadLocalLifetimeTracker::num_move_constructions() == 1);
      REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 2);
      REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 0);
    }
    REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 1);
  }
}

TEST_CASE_METHOD(ThreadLocalLifetimeTrackerFixture,
                 "ThreadLocalLifetimeTracker: count copy assignments",
                 "[Testing]") {
  ThreadLocalLifetimeTracker obj1;
  ThreadLocalLifetimeTracker obj2;
  REQUIRE(ThreadLocalLifetimeTracker::num_default_constructions() == 2);

  SECTION("Copy assigning") {
    obj1 = obj2;
    REQUIRE(ThreadLocalLifetimeTracker::num_copy_assignments() == 1);
    REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 2);
  }
}

TEST_CASE_METHOD(ThreadLocalLifetimeTrackerFixture,
                 "ThreadLocalLifetimeTracker: count move assignments",
                 "[Testing]") {
  ThreadLocalLifetimeTracker obj1;
  ThreadLocalLifetimeTracker obj2;
  REQUIRE(ThreadLocalLifetimeTracker::num_default_constructions() == 2);

  SECTION("Move assigning") {
    obj1 = std::move(obj2);
    REQUIRE(ThreadLocalLifetimeTracker::num_move_assignments() == 1);
    REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 2);
  }
}

TEST_CASE_METHOD(ThreadLocalLifetimeTrackerFixture,
                 "ThreadLocalLifetimeTracker: count destructions",
                 "[Testing]") {
  SECTION("Destruction of a single object") {
    {
      ThreadLocalLifetimeTracker obj;
      REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 0);
    }
    REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 1);
  }

  SECTION("Destruction of multiple objects") {
    {
      ThreadLocalLifetimeTracker obj1;
      ThreadLocalLifetimeTracker obj2;
      ThreadLocalLifetimeTracker obj3;
      REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 0);
    }
    REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 3);
  }
}

TEST_CASE_METHOD(ThreadLocalLifetimeTrackerFixture,
                 "ThreadLocalLifetimeTracker: reset", "[Testing]") {
  ThreadLocalLifetimeTracker obj1;
  ThreadLocalLifetimeTracker obj2(obj1);
  ThreadLocalLifetimeTracker obj3(std::move(obj1));
  obj1 = obj2;
  obj2 = std::move(obj3);

  REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 3);
  REQUIRE(ThreadLocalLifetimeTracker::num_default_constructions() == 1);
  REQUIRE(ThreadLocalLifetimeTracker::num_copy_constructions() == 1);
  REQUIRE(ThreadLocalLifetimeTracker::num_move_constructions() == 1);
  REQUIRE(ThreadLocalLifetimeTracker::num_copy_assignments() == 1);
  REQUIRE(ThreadLocalLifetimeTracker::num_move_assignments() == 1);

  ThreadLocalLifetimeTracker::reset();

  REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 0);
  REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 0);
  REQUIRE(ThreadLocalLifetimeTracker::num_default_constructions() == 0);
  REQUIRE(ThreadLocalLifetimeTracker::num_copy_constructions() == 0);
  REQUIRE(ThreadLocalLifetimeTracker::num_move_constructions() == 0);
  REQUIRE(ThreadLocalLifetimeTracker::num_copy_assignments() == 0);
  REQUIRE(ThreadLocalLifetimeTracker::num_move_assignments() == 0);
}
