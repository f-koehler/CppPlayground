#ifndef CPP_PLAYGROUND_ERROR_HANDLING_ERROR_CODE_HPP
#define CPP_PLAYGROUND_ERROR_HANDLING_ERROR_CODE_HPP

#include <array>
#include <cerrno>
#include <cstring>

namespace CppPlayground::ErrorHandling {

static constexpr std::size_t ErrnoMessageSize = 256;

inline const char *get_error_message(int error_code) {
  thread_local static std::array<char, ErrnoMessageSize> error_buffer;
  ::strerror_r(error_code, error_buffer.data(), error_buffer.size());
  return error_buffer.data();
}

} // namespace CppPlayground::ErrorHandling

#endif
