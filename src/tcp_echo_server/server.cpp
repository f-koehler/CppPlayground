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
    const auto server_socket = ErrorHandling::expect(
        Networking::SocketResource::create_socket(
            AF_INET, SOCK_STREAM, 0));

    ::sockaddr_in server_address = {};
    server_address.sin_port = ::htons(DefaultPort);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (::bind(server_socket.get(), (const ::sockaddr*)&server_address,
               sizeof(server_address)) != 0)
    {
        std::println("{}: failed to bind socket: {}", CppPlayground::ErrorHandling::format_source_location(),
                     CppPlayground::ErrorHandling::get_error_message(errno));
        return EXIT_FAILURE;
    }

    if (::listen(server_socket.get(), DefaultListenBacklog) != 0)
    {
        std::println("{}: failed to listen on socket: {}", CppPlayground::ErrorHandling::format_source_location(),
                     CppPlayground::ErrorHandling::get_error_message(errno));
        return EXIT_FAILURE;
    }

    ::sockaddr_in client_address = {};
    ::socklen_t client_addr_size = sizeof(client_address);
    int client_socket = ::accept(
        server_socket.get(), (::sockaddr*)&client_address, &client_addr_size);
    if (client_socket == -1)
    {
        std::println("{}: failed to accept connection: {}", CppPlayground::ErrorHandling::format_source_location(),
                     CppPlayground::ErrorHandling::get_error_message(errno));
        return EXIT_FAILURE;
    }
    std::println("Accepted connection from {}:{}", client_address.sin_addr.s_addr,
                 client_address.sin_port);


    return EXIT_SUCCESS;
}
