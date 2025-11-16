#include <catch2/catch_test_macros.hpp>

#include <cpp_playground/owning_pointer.hpp>
#include <cpp_playground/testing/thread_local_lifetime_tracker.hpp>
#include <cstddef>

using CppPlayground::OwningPointer;
using CppPlayground::Testing::ThreadLocalLifetimeTracker;
using CppPlayground::Testing::ThreadLocalLifetimeTrackerFixture;

TEST_CASE_METHOD(ThreadLocalLifetimeTrackerFixture,
                 "OwningPointer: default constructor", "[memory]") {
  {
    OwningPointer<ThreadLocalLifetimeTracker> ptr;
    REQUIRE(ptr.get() == nullptr);
  }
  REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 0);
  REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 0);
}

TEST_CASE_METHOD(ThreadLocalLifetimeTrackerFixture,
                 "OwningPointer: construct from nullptr", "[memory]") {
  {
    OwningPointer<ThreadLocalLifetimeTracker> ptr(std::nullptr_t{});
    REQUIRE(ptr.get() == nullptr);
  }
  REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 0);
  REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 0);
}

TEST_CASE_METHOD(ThreadLocalLifetimeTrackerFixture,
                 "OwningPointer: construct from raw pointer", "[memory]") {
  {
    ThreadLocalLifetimeTracker *tracker = new ThreadLocalLifetimeTracker();
    OwningPointer<ThreadLocalLifetimeTracker> ptr(tracker);
    REQUIRE(ptr.get() == tracker);
  }
  REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 1);
  REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 1);
}

TEST_CASE_METHOD(ThreadLocalLifetimeTrackerFixture,
                 "OwningPointer: construct from raw pointer, custom deleter",
                 "[memory]") {
  auto deleter = [](ThreadLocalLifetimeTracker *tracker) { delete tracker; };
  {
    ThreadLocalLifetimeTracker *tracker = new ThreadLocalLifetimeTracker();
    OwningPointer<ThreadLocalLifetimeTracker, decltype(deleter)> ptr(tracker,
                                                                     deleter);
    REQUIRE(ptr.get() == tracker);
  }
  REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 1);
  REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 1);
}

TEST_CASE_METHOD(ThreadLocalLifetimeTrackerFixture,
                 "OwningPointer: move construct", "[memory]") {
  {

    ThreadLocalLifetimeTracker *tracker = new ThreadLocalLifetimeTracker();
    OwningPointer<ThreadLocalLifetimeTracker> ptr(tracker);
    OwningPointer<ThreadLocalLifetimeTracker> ptr2(std::move(ptr));
    REQUIRE(ptr2.get() == tracker);
    REQUIRE(ptr.get() == nullptr);
  }
  REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 1);
  REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 1);
}

TEST_CASE_METHOD(ThreadLocalLifetimeTrackerFixture,
                 "OwningPointer: move assign from another pointer",
                 "[memory]") {
  {

    ThreadLocalLifetimeTracker *tracker = new ThreadLocalLifetimeTracker();
    OwningPointer<ThreadLocalLifetimeTracker> ptr(tracker);
    OwningPointer<ThreadLocalLifetimeTracker> ptr2;
    ptr2 = std::move(ptr);
    REQUIRE(ptr2.get() == tracker);
    REQUIRE(ptr.get() == nullptr);
  }
  REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 1);
  REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 1);
}

TEST_CASE_METHOD(ThreadLocalLifetimeTrackerFixture,
                 "OwningPointer: assigning nullptr", "[memory]") {
  {

    OwningPointer<ThreadLocalLifetimeTracker> ptr(
        new ThreadLocalLifetimeTracker());
    ptr = nullptr;
    REQUIRE(ptr.get() == nullptr);
  }
  REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 1);
  REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 1);
}

TEST_CASE("OwningPointer: dereference", "[memory]") {
  struct Element {
    int value;
  };
  OwningPointer<Element> ptr(new Element{42});
  REQUIRE((*ptr).value == 42);
  REQUIRE(ptr->value == 42);
  REQUIRE((*ptr.get()).value == 42);
  REQUIRE(ptr.get()->value == 42);
}

TEST_CASE_METHOD(ThreadLocalLifetimeTrackerFixture, "OwningPointer: release") {
  {
    OwningPointer<ThreadLocalLifetimeTracker> ptr(
        new ThreadLocalLifetimeTracker());
    REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 1);
    ptr.release();
    REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 1);
  }
  REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 1);
}

// using CppPlayground::OwningPointer;
// using CppPlayground::Testing::ThreadLocalLifetimeTracker;
// using CppPlayground::Testing::ThreadLocalLifetimeTrackerFixture;

// TEST_CASE("OwningPointer: lifetime management",
//           "[memory]") {
//   ThreadLocalLifetimeTrackerFixture fixture;

//   SECTION("Custom deleter is used") {
//     bool deleter_called = false;
//     auto custom_deleter =
//         [&deleter_called](ThreadLocalLifetimeTracker *p) {
//           delete p;
//           deleter_called = true;
//         };

//     {
//       OwningPointer<ThreadLocalLifetimeTracker, decltype(custom_deleter)>
//       ptr(
//           new ThreadLocalLifetimeTracker(), custom_deleter);
//       REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 1);
//       REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 0);
//       REQUIRE_FALSE(deleter_called);
//     }

//     REQUIRE(ThreadLocalLifetimeTracker::num_constructions() == 1);
//     REQUIRE(ThreadLocalLifetimeTracker::num_destructions() == 1);
//     REQUIRE(deleter_called);
//   }
// }
