#ifndef CPP_PLAYGROUND_ERROR_HANDLING_SOURCE_LOCATION_HPP
#define CPP_PLAYGROUND_ERROR_HANDLING_SOURCE_LOCATION_HPP

#include <cassert>
#include <format>
#include <source_location>

namespace CppPlayground::ErrorHandling {
inline auto format_source_location(
    const std::source_location &location = std::source_location::current()) {
  const char *function_name = location.function_name();
  assert(function_name != nullptr);
  if (function_name[0] == '\0') {
    return std::format("{}({}:{})", location.file_name(), location.line(),
                       location.column());
  }
  return std::format("{}({}:{}) `{}`", location.file_name(), location.line(),
                     location.column(), function_name);
}
} // namespace CppPlayground::ErrorHandling

#endif
