#ifndef CPP_PLAYGROUND_FUNCTIONAL_FUNCTION_HPP
#define CPP_PLAYGROUND_FUNCTIONAL_FUNCTION_HPP

#include <functional>
#include <memory>
#include <utility>

namespace CppPlayground {

template <typename Signature> class Function;

template <typename R, typename... Args> class Function<R(Args...)> {
public:
  Function() = default;
  Function(Function &&) noexcept = default;
  Function(const Function &) = delete;
  Function &operator=(Function &&) noexcept = default;
  Function &operator=(const Function &) = delete;

  template <typename U>
  explicit Function(U &&function)
      : m_callable(std::make_unique<Callable<std::decay_t<U>>>(
            std::forward<U>(function))) {}

  template <typename U> Function &operator=(U &&function) {
    m_callable =
        std::make_unique<Callable<std::decay_t<U>>>(std::forward<U>(function));
    return *this;
  }

  R operator()(Args... args) {
    if (m_callable == nullptr) {
      throw std::bad_function_call();
    }
    return m_callable->invoke(std::forward<Args>(args)...);
  }

private:
  // abstract type-erased interface for callable
  struct ICallable {
    virtual ~ICallable() = default;
    virtual R invoke(Args &&...args) = 0;
  };

  template <typename F> struct Callable : ICallable {
    F m_function;

    template <typename U>
    explicit Callable(U &&function) : m_function(std::forward<U>(function)) {}

    R invoke(Args &&...args) override {
      return std::invoke(m_function, std::forward<Args>(args)...);
    }
  };

  std::unique_ptr<ICallable> m_callable;
};

} // namespace CppPlayground

#endif
