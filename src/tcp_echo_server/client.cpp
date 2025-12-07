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
#include <cpp_playground/error_handling/error_code.hpp>
#include <cpp_playground/error_handling/source_location.hpp>

static constexpr uint16_t DefaultPort = 2804;

int main()
{
    int client_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        std::println("{}: failed to create socket: {}\n", CppPlayground::ErrorHandling::format_source_location(),
                   CppPlayground::ErrorHandling::get_error_message(errno));
        return EXIT_FAILURE;
    }

    ::sockaddr_in server_address = {};
    server_address.sin_port = ::htons(DefaultPort);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (::connect(client_socket, (const ::sockaddr*)&server_address, sizeof(server_address)) != 0)
    {
        std::println("{}: failed to connect to server: {}", CppPlayground::ErrorHandling::format_source_location(),
                   CppPlayground::ErrorHandling::get_error_message(errno));
        return EXIT_FAILURE;
    }

    if (::close(client_socket) != 0)
    {
        std::println("{}: failed to close socket: {}\n", CppPlayground::ErrorHandling::format_source_location(),
                   CppPlayground::ErrorHandling::get_error_message(errno));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
