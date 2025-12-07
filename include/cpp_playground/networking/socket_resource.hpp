#ifndef CPP_PLAYGROUND_NETWORKING_SOCKET_RESOURCE_HPP
#define CPP_PLAYGROUND_NETWORKING_SOCKET_RESOURCE_HPP

#include <expected>
#include <cpp_playground/error_handling/error_code.hpp>
#include <cpp_playground/error_handling/source_location.hpp>
#include <print>
#include <format>
#include <sys/socket.h>
#include <unistd.h>

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
    };
} // namespace CppPlayground::Networking

#endif
