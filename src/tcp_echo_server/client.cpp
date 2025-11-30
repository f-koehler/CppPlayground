#include <arpa/inet.h>
#include <array>
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <print>
#include <sys/socket.h>
#include <unistd.h>

static constexpr std::size_t MaxErrnoMessageSize = 1024;
static constexpr uint16_t DefaultPort = 2804;

int main() {
  std::array<char, MaxErrnoMessageSize> errno_message = {0};

  int client_socket = ::socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket == -1) {
    ::strerror_r(errno, errno_message.data(), errno_message.size());
    std::print("Failed to create socket: {}\n", errno_message.data());
    return EXIT_FAILURE;
  }

  ::sockaddr_in server_address = {};
  server_address.sin_port = ::htons(DefaultPort);
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;

  if(::connect(client_socket, (const ::sockaddr*)&server_address, sizeof(server_address)) != 0) {
      ::strerror_r(errno, errno_message.data(), errno_message.size());
      std::print("Failed to connect to server: {}\n", errno_message.data());
      return EXIT_FAILURE;
  }

  if (::close(client_socket) != 0) {
    ::strerror_r(errno, errno_message.data(), errno_message.size());
    std::print("Failed to close socket: {}\n", errno_message.data());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
