#ifndef CPP_PLAYGROUND_NETWORKING_SOCKET_RESOURCE_HPP
#define CPP_PLAYGROUND_NETWORKING_SOCKET_RESOURCE_HPP

#include <expected>
#include <cpp_playground/error_handling/error_code.hpp>
#include <cpp_playground/error_handling/source_location.hpp>
#include <print>
#include <format>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

template <>
struct std::formatter<::sockaddr_in>
{
    constexpr auto parse([[maybe_unused]] std::format_parse_context& ctx)
    {
        // TODO: not implemented yet
        return ctx.begin();
    }

    auto format(const ::sockaddr_in& addr, std::format_context& ctx) const noexcept
    {
        thread_local static std::array<char, INET_ADDRSTRLEN> ip_address = {0};
        if (::inet_ntop(AF_INET, &addr.sin_addr, ip_address.data(), ip_address.size()) == nullptr)
        {
            return std::format_to(ctx.out(), "<invalid_ipv4>");
        }
        uint16_t port = ntohs(addr.sin_port);
        return std::format_to(ctx.out(), "{}:{}", ip_address.data(), port);
    }
};

namespace CppPlayground::Networking
{

    class SocketResource
    {
    private:
        int m_socket_fd;

        explicit SocketResource(const int socket_fd) : m_socket_fd(socket_fd)
        {
        }

    public:
        static auto manage_socket(const int socket_fd) -> std::expected<SocketResource, std::string>
        {
            return SocketResource(socket_fd);
        }

        static auto create_socket(const int domain, const int type,
                                  const int protocol) -> std::expected<SocketResource, std::string>
        {
            const int socket_fd = ::socket(domain, type, protocol);
            if (socket_fd == -1)
            {
                return std::unexpected(std::format("{}: failed to create socket: {}",
                                                   ErrorHandling::format_source_location(),
                                                   ErrorHandling::get_error_message(errno)));
            }
            return SocketResource(socket_fd);
        }

        SocketResource(const SocketResource&) = delete;

        SocketResource(SocketResource&& other) noexcept : m_socket_fd(other.m_socket_fd)
        {
            other.m_socket_fd = -1;
        }

        ~SocketResource()
        {
            if (m_socket_fd != -1)
            {
                if (::close(m_socket_fd) != 0)
                {
                    std::println("{}: error when closing socket: {}",
                                 ErrorHandling::format_source_location(),
                                 ErrorHandling::get_error_message(errno));
                }
            }
        }

        SocketResource& operator=(const SocketResource&) = delete;

        SocketResource& operator=(SocketResource&& other) noexcept
        {
            if (this == &other)
            {
                return *this;
            }
            if (m_socket_fd != -1)
            {
                if (::close(m_socket_fd) != 0)
                {
                    std::println("{}: error when closing socket: {}",
                                 ErrorHandling::format_source_location(),
                                 ErrorHandling::get_error_message(errno));
                }
            }
            m_socket_fd = other.m_socket_fd;
            other.m_socket_fd = -1;
            return *this;
        }

        [[nodiscard]] int get() const { return m_socket_fd; }

        template <typename Address>
        [[nodiscard]] auto bind(const Address& address) const -> std::expected<void, std::string>
        {
            if (::bind(m_socket_fd, (const ::sockaddr*)&address, sizeof(Address)) != 0)
            {
                return std::unexpected(std::format("{}: failed to bind socket: {}",
                                                   ErrorHandling::format_source_location(),
                                                   ErrorHandling::get_error_message(errno)));
            }
            return {};
        }

        template <typename Address>
        [[nodiscard]] auto connect(const Address& address) const -> std::expected<void, std::string>
        {
            if (::connect(m_socket_fd, (const ::sockaddr*)&address, sizeof(Address)) != 0)
            {
                return std::unexpected(std::format("{}: failed to connect socket: {}",
                                                   ErrorHandling::format_source_location(),
                                                   ErrorHandling::get_error_message(errno)));
            }
            return {};
        }

        [[nodiscard]] auto listen(const int backlog_size) const -> std::expected<void, std::string>
        {
            if (::listen(m_socket_fd, backlog_size) != 0)
            {
                return std::unexpected(std::format("{}: failed to listen socket: {}",
                                                   ErrorHandling::format_source_location(),
                                                   ErrorHandling::get_error_message(errno)));
            }
            return {};
        }

        template <typename Address>
        [[nodiscard]] auto accept(Address& address,
                                  ::socklen_t& address_length) const -> std::expected<SocketResource, std::string>
        {
            int client_socket = ::accept(m_socket_fd, (::sockaddr*)&address, &address_length);
            if (client_socket == -1)
            {
                return std::unexpected(std::format("{}: failed to accept connection: {}",
                                                   ErrorHandling::format_source_location(),
                                                   ErrorHandling::get_error_message(errno)));
            }
            return SocketResource(client_socket);
        }
    };
} // namespace CppPlayground::Networking

#endif
