#ifndef CPP_PLAYGROUND_EXPECTED_HPP
#define CPP_PLAYGROUND_EXPECTED_HPP

#include <expected>
#include <exception>
#include <ostream>

namespace CppPlayground::ErrorHandling
{
    template <typename T, typename E>
    T expect(std::expected<T, E>&& expected)
    {
        if (!expected)
        {
            std::println("{}", expected.error());
            std::terminate();
        }
        if constexpr (!std::is_same_v<void,T>)
        {
            return std::move(expected.value());
        }
    }
}

#endif //CPP_PLAYGROUND_EXPECTED_HPP