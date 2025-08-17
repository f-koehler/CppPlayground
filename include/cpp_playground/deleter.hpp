#ifndef CPPPLAYGROUND_DELETER_HPP
#define CPPPLAYGROUND_DELETER_HPP

#include <concepts>
#include <type_traits>

namespace CppPlayground {
template <typename T>
concept IsDeleter =
    std::is_default_constructible_v<T> && requires(T t, void *vptr) {
      { t(vptr) } -> std::same_as<void>;
    };

template <typename T> struct DefaultDelete {
  virtual void operator()(void *ptr) { delete (T *)ptr; }
};

template <typename T> struct DefaultDeleteArray {
  virtual void operator()(void *ptr) { delete[] (T *)ptr; }
};

template <IsDeleter Deleter> class CountingDelete : Deleter {
public:
  using BaseDeleter = Deleter;

private:
  std::size_t m_count = 0;

public:
  virtual void operator()(void *ptr) {
    BaseDeleter::operator()(ptr);
    ++m_count;
  }

  constexpr std::size_t get_count() const noexcept { return m_count; }
};

} // namespace CppPlayground

#endif
