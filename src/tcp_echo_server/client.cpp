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
#include <cpp_playground/error_handling/expected.hpp>
#include <cpp_playground/networking/socket_resource.hpp>

using namespace CppPlayground;

static constexpr uint16_t DefaultPort = 2804;

int main()
{
    const auto client_socket = ErrorHandling::expect(
        Networking::SocketResource::create_socket(AF_INET, SOCK_STREAM, 0));

    ::sockaddr_in server_address = {};
    server_address.sin_port = ::htons(DefaultPort);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (::connect(client_socket.get(), (const ::sockaddr*)&server_address, sizeof(server_address)) != 0)
    {
        std::println("{}: failed to connect to server: {}", CppPlayground::ErrorHandling::format_source_location(),
                     CppPlayground::ErrorHandling::get_error_message(errno));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
