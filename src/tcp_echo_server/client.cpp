#include <CLI/Error.hpp>
#include <arpa/inet.h>
#include <cpp_playground/networking/socket.hpp>
#include <cstdint>
#include <cstdlib>
#include <netinet/in.h>
#include <span>
#include <sys/socket.h>
#include <unistd.h>

using namespace CppPlayground;

static constexpr uint16_t DefaultPort = 2804;

int main() {
  ::sockaddr_in server_address = {};
  server_address.sin_port = htons(DefaultPort);
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;

  const auto client_socket =
      Networking::Socket::create_socket(AF_INET, SOCK_STREAM, 0).unwrap();
  client_socket.connect(server_address).unwrap();

  // send message
  std::string message = "Hello world!";
  client_socket.write(message).unwrap();

  // read reply
  message.resize(1024);
  client_socket.read(message).unwrap();

  std::println("Received reply: {}", message);

  return EXIT_SUCCESS;
}
