#include <arpa/inet.h>
#include <atomic>
#include <cpp_playground/networking/socket.hpp>
#include <csignal>
#include <cstdlib>
#include <netinet/in.h>
#include <print>
#include <sys/socket.h>
#include <thread>
#include <unordered_map>

#include "cpp_playground/networking/epoll.hpp"

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
  server_socket.set_non_blocking(true).unwrap();
  server_socket.bind(server_address).unwrap();
  server_socket.listen(DefaultListenBacklog).unwrap();

  auto epoll = Networking::EpollInstance::create_epoll_instance().unwrap();
  epoll.add_socket(server_socket.get()).unwrap();
  std::vector<Networking::EpollInstance::Event> events(10);

  ::sockaddr_in client_address = {};
  ::socklen_t client_address_length = sizeof(client_address);

  std::unordered_map<int, Networking::Socket> client_sockets;

  std::string buffer(1024, '\0');
  const std::string reply("This is the reply from the server!");

  while (!stop_flag.load()) {
    events.resize(10);
    epoll.wait(events).unwrap();
    std::println("{} epoll events", events.size());

    for (const auto &event : events) {
      if (event.data.fd == server_socket.get()) {
        auto client_socket =
            server_socket.accept(client_address, client_address_length)
                .unwrap();
        std::println("Accepted connection from {}", client_address);
        client_socket.set_non_blocking(true).unwrap();
        epoll.add_socket(client_socket.get()).unwrap();
        client_sockets.emplace(client_socket.get(), std::move(client_socket));
        continue;
      }
      if (!client_sockets.contains(event.data.fd)) {
        std::println("Invalid socket: {}", event.data.fd);
        continue;
      }
      std::println("socket: {}", event.data.fd);
      auto &socket = client_sockets.at(event.data.fd);
      socket.read(buffer).unwrap();
      std::println("Received message: {}", buffer);
      socket.write_exactly(reply).unwrap();
    }

    events.clear();
  }

  return EXIT_SUCCESS;
}
