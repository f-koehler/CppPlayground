#include <arpa/inet.h>
#include <array>
#include <cerrno>
#include <cpp_playground/networking/socket_resource.hpp>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <print>
#include <sys/socket.h>
#include <unistd.h>

static constexpr std::size_t MaxErrnoMessageSize = 1024;
static constexpr uint16_t DefaultPort = 2804;
static constexpr int DefaultListenBacklog = 20;

int main() {
  std::array<char, MaxErrnoMessageSize> errno_message = {0};

  CppPlayground::Networking::SocketResource server_socket(
      socket(AF_INET, SOCK_STREAM, 0));
  if (server_socket.get() == -1) {
    ::strerror_r(errno, errno_message.data(), errno_message.size());
    std::print("Failed to create socket: {}\n", errno_message.data());
    return EXIT_FAILURE;
  }

  ::sockaddr_in server_address = {};
  server_address.sin_port = ::htons(DefaultPort);
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;

  if (::bind(server_socket.get(), (const ::sockaddr *)&server_address,
             sizeof(server_address)) != 0) {
    ::strerror_r(errno, errno_message.data(), errno_message.size());
    std::print("Failed to bind socket: {}\n", errno_message.data());
    return EXIT_FAILURE;
  }

  if (::listen(server_socket.get(), DefaultListenBacklog) != 0) {
    ::strerror_r(errno, errno_message.data(), errno_message.size());
    std::print("Failed to listen on socket: {}\n", errno_message.data());
    return EXIT_FAILURE;
  }

  ::sockaddr_in client_address = {};
  ::socklen_t client_addr_size = sizeof(client_address);
  int client_socket = ::accept(
      server_socket.get(), (::sockaddr *)&client_address, &client_addr_size);
  if (client_socket == -1) {
    ::strerror_r(errno, errno_message.data(), errno_message.size());
    std::print("Failed to accept connection: {}\n", errno_message.data());
    return EXIT_FAILURE;
  }
  std::print("Accepted connection from {}:{}\n", client_address.sin_addr.s_addr,
             client_address.sin_port);

  if (::close(client_socket) != 0) {
    ::strerror_r(errno, errno_message.data(), errno_message.size());
    std::print("Failed to close client socket: {}\n", errno_message.data());
  }
  if (::close(server_socket.get()) != 0) {
    ::strerror_r(errno, errno_message.data(), errno_message.size());
    std::print("Failed to close client socket: {}\n", errno_message.data());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
