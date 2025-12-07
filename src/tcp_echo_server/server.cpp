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
#include <cpp_playground/error_handling/expected.hpp>

using namespace CppPlayground;
static constexpr uint16_t DefaultPort = 2804;
static constexpr int DefaultListenBacklog = 20;

int main()
{
    ::sockaddr_in server_address = {};
    server_address.sin_port = ::htons(DefaultPort);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;

    const auto server_socket = ErrorHandling::expect(
        Networking::SocketResource::create_socket(
            AF_INET, SOCK_STREAM, 0));
    ErrorHandling::expect(server_socket.bind(server_address));
    ErrorHandling::expect(server_socket.listen(DefaultListenBacklog));

    ::sockaddr_in client_address = {};
    ::socklen_t client_address_length = sizeof(client_address);
    const auto client_socket = ErrorHandling::expect(server_socket.accept(client_address, client_address_length));
    std::println("Accepted connection from {}:{}", client_address.sin_addr.s_addr,
                 client_address.sin_port);


    return EXIT_SUCCESS;
}
