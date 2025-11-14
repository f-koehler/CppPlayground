#include <atomic>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cpp_playground/arc_pointer.hpp>
#include <cpp_playground/deleter.hpp>
#include <cstddef>
#include <string>

using CppPlayground::ArcPointer;
using CppPlayground::CountingDelete;
using CppPlayground::DefaultDelete;

TEMPLATE_TEST_CASE("ArcPointer: Type Traits", "[SmartPointer]", int, char,
                   std::string) {
  REQUIRE(std::is_same_v<TestType, typename ArcPointer<TestType>::ElementType>);
  REQUIRE(
      std::is_same_v<TestType, typename ArcPointer<TestType[]>::ElementType>);
}

template <typename T> struct SentinelType {
private:
  static std::atomic_uint64_t num_destructions;
  T m_value;

public:
  explicit SentinelType() = default;
  explicit SentinelType(const T &value) : m_value(value) {}
  explicit SentinelType(T &&value) noexcept : m_value(std::move(value)) {}
  SentinelType &operator=(const T &value) {
    m_value = value;
    return *this;
  }
  SentinelType &operator=(T &&value) {
    m_value = std::move(value);
    return *this;
  }
  virtual ~SentinelType() { ++num_destructions; }

  static std::size_t get_destruction_count() { return num_destructions; }
  static void reset_destruction_count() { num_destructions.store(0UL); }
};

template <typename T>
std::atomic_uint64_t SentinelType<T>::num_destructions =
    std::atomic_uint64_t{0};

TEST_CASE("ArcPointer: Control Block", "[SmartPointer]") {
  using T = SentinelType<std::string>;

  SECTION("Single Element") {
    using Deleter = CppPlayground::DefaultDelete<T>;
    using ControlBlock = CppPlayground::Detail::ControlBlock<Deleter>;

    auto *block = new ControlBlock(new T("hello"), Deleter{});
    T::reset_destruction_count();
    REQUIRE(block->m_num_shared.load() == 1);
    block->release_ref();
    REQUIRE(T::get_destruction_count() == 1);
  }

  SECTION("Array") {
    using Deleter = CppPlayground::DefaultDeleteArray<T>;
    using ControlBlock = CppPlayground::Detail::ControlBlock<Deleter>;

    auto *block = new ControlBlock(new T[10], Deleter{});
    T::reset_destruction_count();
    REQUIRE(block->m_num_shared.load() == 1);
    block->release_ref();
    REQUIRE(T::get_destruction_count() == 10);
  }
}
