#include <arpa/inet.h>
#include <cstdint>
#include <cstdlib>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <CLI/Error.hpp>
#include <cpp_playground/networking/socket_resource.hpp>
#include <span>

using namespace CppPlayground;

static constexpr uint16_t DefaultPort = 2804;

int main()
{
    ::sockaddr_in server_address = {};
    server_address.sin_port = htons(DefaultPort);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;

    const auto client_socket = Networking::SocketResource::create_socket(AF_INET, SOCK_STREAM, 0).unwrap();
    client_socket.connect(server_address).unwrap();

    // send message
    std::string message = "Hello world!";
    client_socket.write(message.size()).unwrap();
    client_socket.write_exactly(message).unwrap();

    // read reply
    const std::size_t message_size = client_socket.read<std::size_t>().unwrap();
    std::println("Receiving {} byte message", message_size);
    message.resize(message_size);
    std::span<std::byte> message_span((std::byte*) message.data(), message_size);
    client_socket.read_exactly(message_span).unwrap();
    std::println("Received message: {}", message);

    return EXIT_SUCCESS;
}
