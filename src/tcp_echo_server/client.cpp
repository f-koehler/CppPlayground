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
#include <CLI/Error.hpp>
#include <cpp_playground/error_handling/error_code.hpp>
#include <cpp_playground/error_handling/source_location.hpp>
#include <cpp_playground/error_handling/expected.hpp>
#include <cpp_playground/networking/socket_resource.hpp>

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

    client_socket.write(uint64_t{42}).unwrap();

    // client_socket.write_exactly(std::span<std::byte>((std::byte*)message.data(), message.size())).unwrap();


    return EXIT_SUCCESS;
}
