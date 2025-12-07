#ifndef CPP_PLAYGROUND_RESULT_HPP
#define CPP_PLAYGROUND_RESULT_HPP

#include <expected>
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
}

#endif //CPP_PLAYGROUND_RESULT_HPP
