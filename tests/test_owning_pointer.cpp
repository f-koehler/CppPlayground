#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cpp_playground/deleter.hpp>
#include <cpp_playground/owning_pointer.hpp>
#include <cstddef>
#include <string>

using namespace CppPlayground;

TEMPLATE_TEST_CASE("OwningPointer: Constructors", "[SmartPointer]", int, char,
                   std::string) {
  using T = TestType;

  SECTION("Default Constructor") {
    OwningPointer<T> const pointer;
    REQUIRE(pointer.get() == nullptr);
  }

  SECTION("Construct from nullptr_t") {
    OwningPointer<T> const pointer{std::nullptr_t{}};
    REQUIRE(pointer.get() == nullptr);
  }

  SECTION("From Raw Pointer") {
    // NOLINTBEGIN(cppcoreguidelines-owning-memory)
    T *raw_pointer = new T{};
    // NOLINTEND(cppcoreguidelines-owning-memory)
    OwningPointer<T> const pointer(raw_pointer);
    REQUIRE(pointer.get() == raw_pointer);
  }

  // SECTION("From Raw Pointer (Check Dtor)") {
  //
  //   // std::size_t count = 0;
  //   //
  //   // {
  //   //   OwningPointer<T> pointer(raw_pointer, [&count](void *ptr) {
  //   //     delete (T *)ptr;
  //   //     ++count;
  //   //   });
  //   //   REQUIRE(pointer.get() == raw_pointer);
  //   // }
  //   // REQUIRE(count == 1);
  // }
}

TEMPLATE_TEST_CASE("OwningPointer: Assignment Operators", "[SmartPointer]", int,
                   char, std::string) {
  using T = TestType;

  // NOLINTBEGIN(cppcoreguidelines-owning-memory)
  T *raw_pointer = new T{};
  // NOLINTEND(cppcoreguidelines-owning-memory)
  {
    OwningPointer<T, CountingDelete<DefaultDelete<T>>> pointer{raw_pointer};
    pointer = nullptr;
    REQUIRE(pointer.getDeleter().get_count() == 1);
  }
}
