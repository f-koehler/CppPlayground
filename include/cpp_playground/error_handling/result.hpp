#ifndef CPP_PLAYGROUND_RESULT_HPP
#define CPP_PLAYGROUND_RESULT_HPP

#include <expected>
#include <exception>
#include <print>

namespace CppPlayground::ErrorHandling
{
    template <typename T, typename E>
    class Result : public std::expected<T, E>
    {
    public:
        using std::expected<T, E>::expected;

        constexpr decltype(auto) expect(this auto&& self)
        {
            if (!self)
            {
                std::println("{}", self.error());
                std::terminate();
            }
            if constexpr (!std::is_void_v<T>)
            {
                return std::forward<decltype(self)>(self).value();
            }
        }
    };

    template <typename T>
    struct OkValue
    {
        T value;

        template <typename E>
            requires std::constructible_from<Result<T, E>, const T&>
        constexpr operator Result<T, E>() const &
        {
            return Result<T, E>(value);
        }

        template <typename E>
            requires std::constructible_from<Result<T, E>, T&&>
        constexpr operator Result<T, E>() &&
        {
            return Result<T, E>(std::move(value));
        }
    };

    template <typename E>
    struct ErrValue
    {
        E error;

        template <typename T>
            requires std::constructible_from<Result<T, E>, std::unexpected<E>>
        constexpr operator Result<T, E>() const &
        {
            return std::unexpected<E>(error);
        }

        template <typename T>
            requires std::constructible_from<Result<T, E>, std::unexpected<E>>

        constexpr operator Result<T, E>() &&
        {
            return std::unexpected<E>(std::move(error));
        }
    };

    template <typename T>
    OkValue<T> Ok(T&& value)
    {
        return OkValue<std::decay_t<T>>{std::forward<T>(value)};
    }

    template <typename E>
    ErrValue<E> Err(E&& error)
    {
        return ErrValue<std::decay_t<E>>{std::forward<E>(error)};
    }
}

namespace CppPlayground
{

}

#endif //CPP_PLAYGROUND_RESULT_HPP
