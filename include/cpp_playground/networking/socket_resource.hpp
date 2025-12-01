#ifndef CPP_PLAYGROUND_NETWORKING_SOCKET_RESOURCE_HPP
#define CPP_PLAYGROUND_NETWORKING_SOCKET_RESOURCE_HPP

#include <cpp_playground/error_handling/error_code.hpp>
#include <cpp_playground/error_handling/source_location.hpp>
#include <print>
#include <sys/socket.h>
#include <unistd.h>

namespace CppPlayground::Networking {

class SocketResource {
private:
  int m_socket_fd;

public:
  explicit SocketResource(int fd) : m_socket_fd(fd) {}

  SocketResource(const SocketResource &) = delete;
  SocketResource(SocketResource &&other) noexcept;
  ~SocketResource() {
    if (::close(m_socket_fd) != 0) {
      std::println("{}: error when closing socket: {}",
                   ErrorHandling::format_source_location(),
                   ErrorHandling::get_error_message(errno));
    }
  }

  SocketResource &operator=(const SocketResource &) = delete;
  SocketResource &operator=(SocketResource &&) noexcept;

  [[nodiscard]] int get() const { return m_socket_fd; }
};

} // namespace CppPlayground::Networking

#endif
