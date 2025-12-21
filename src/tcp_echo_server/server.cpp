#include <arpa/inet.h>
#include <atomic>
#include <cpp_playground/networking/socket.hpp>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <print>
#include <sys/socket.h>

using namespace CppPlayground;
static constexpr uint16_t DefaultPort = 2804;
static constexpr int DefaultListenBacklog = 20;
std::atomic<bool> stop_flag{false};

void handle_signals([[maybe_unused]] int signal_id) { stop_flag.store(true); }

int main() {
  if ((std::signal(SIGINT, handle_signals) == SIG_ERR) ||
      (std::signal(SIGTERM, handle_signals) == SIG_ERR)) {
    std::println("{}: failed to setup signal handlers",
                 ErrorHandling::format_source_location());
    return EXIT_FAILURE;
  }

  ::sockaddr_in server_address = {};
  server_address.sin_port = htons(DefaultPort);
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;

  const auto server_socket =
      Networking::Socket::create_socket(AF_INET, SOCK_STREAM, 0).unwrap();
  server_socket.bind(server_address).unwrap();
  server_socket.listen(DefaultListenBacklog).unwrap();

  ::sockaddr_in client_address = {};
  ::socklen_t client_address_length = sizeof(client_address);

  std::string buffer(1024, '\0');
  const std::string reply("This is the reply from the server!");

  while (!stop_flag.load()) {
    buffer.resize(1024);

    const auto client_socket =
        server_socket.accept(client_address, client_address_length).unwrap();
    std::println("Accepted connection from {}", client_address);

    // read message
    client_socket.read(buffer).unwrap();
    std::println("Received message: {}", buffer);

    // send reply
    client_socket.write_exactly(reply).unwrap();
  }

  return EXIT_SUCCESS;
}
