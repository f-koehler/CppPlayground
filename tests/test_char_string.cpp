#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cpp_playground/char_string.hpp>
#include <memory>
#include <memory_resource>

using CppPlayground::CharString;
using CppPlayground::Detail::CharStringBuffer;

TEMPLATE_TEST_CASE("CharStringBuffer", "[Detail]", std::allocator<char>,
                   std::pmr::polymorphic_allocator<char>) {
  using Allocator = TestType;
  using Buffer = CharStringBuffer<Allocator>;

  SECTION("Constants") {
    REQUIRE(sizeof(typename Buffer::HeapBuffer) >=
            (sizeof(std::size_t) + sizeof(char *)));
    REQUIRE(Buffer::StackBufferSize == sizeof(typename Buffer::HeapBuffer));
    REQUIRE(Buffer::ShortStringLength ==
            sizeof(typename Buffer::HeapBuffer) - 1);
  }

  SECTION("Default Construction") {
    REQUIRE(Buffer{}.get_capacity() == Buffer::ShortStringLength);
  }
}
