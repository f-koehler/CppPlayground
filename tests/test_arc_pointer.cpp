#include <atomic>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cpp_playground/arc_pointer.hpp>
#include <string>

using CppPlayground::ArcPointer;

TEMPLATE_TEST_CASE("ArcPointer: Constructors", "[SmartPointer]", int, char,
                   std::string) {
  using T = TestType;

  SECTION("Default Constructor") {
    ArcPointer<T> pointer;
    REQUIRE(pointer.get() == nullptr);
  }

  SECTION("From Raw Pointer") {
    T *raw_pointer = new T{};
    ArcPointer<T> pointer(raw_pointer);
    REQUIRE(pointer.get() == raw_pointer);
  }

  SECTION("From Raw Pointer (Check Dtor)") {
    // struct {
    //   std::size_t count = 0;
    //
    //   void operator()(void *ptr) {
    //     delete (T *)ptr;
    //     ++count;
    //   }
    // } deleter;

    T *raw_pointer = new T{};
    std::size_t count = 0;

    {
      ArcPointer<T> pointer(raw_pointer, [&count](void *ptr) {
        delete (T *)ptr;
        ++count;
      });
      REQUIRE(pointer.get() == raw_pointer);
    }
    REQUIRE(count == 1);
  }
}
