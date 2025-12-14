#ifndef CPP_PLAYGROUND_NETWORKING_SOCKET_RESOURCE_HPP
#define CPP_PLAYGROUND_NETWORKING_SOCKET_RESOURCE_HPP

#include <expected>
#include <cpp_playground/error_handling/error_code.hpp>
#include <cpp_playground/error_handling/source_location.hpp>
#include <cpp_playground/error_handling/result.hpp>
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
        static auto manage_socket(const int socket_fd) -> ErrorHandling::Result<SocketResource, std::string>
        {
            return SocketResource(socket_fd);
        }

        static auto create_socket(const int domain, const int type,
                                  const int protocol) -> ErrorHandling::Result<SocketResource, std::string>
        {
            const int socket_fd = ::socket(domain, type, protocol);
            if (socket_fd == -1)
            {
                return std::unexpected(std::format("{}: failed to create socket: {}",
                                                   ErrorHandling::format_source_location(),
                                                   ErrorHandling::get_error_message(errno)));
            }
            return ErrorHandling::Ok(SocketResource(socket_fd));
            // return SocketResource(socket_fd);
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
        [[nodiscard]] auto bind(const Address& address) const -> ErrorHandling::Result<void, std::string>
        {
            if (::bind(m_socket_fd, (const ::sockaddr*)&address, sizeof(Address)) != 0)
            {
                return ErrorHandling::Err(std::format("{}: failed to bind socket: {}",
                                                      ErrorHandling::format_source_location(),
                                                      ErrorHandling::get_error_message(errno)));
            }
            return {};
        }

        template <typename Address>
        [[nodiscard]] auto connect(const Address& address) const -> ErrorHandling::Result<void, std::string>
        {
            if (::connect(m_socket_fd, (const ::sockaddr*)&address, sizeof(Address)) != 0)
            {
                return ErrorHandling::Err(std::format("{}: failed to connect socket: {}",
                                                      ErrorHandling::format_source_location(),
                                                      ErrorHandling::get_error_message(errno)));
            }
            return {};
        }

        [[nodiscard]] auto listen(const int backlog_size) const -> ErrorHandling::Result<void, std::string>
        {
            if (::listen(m_socket_fd, backlog_size) != 0)
            {
                return ErrorHandling::Err(std::format("{}: failed to listen socket: {}",
                                                      ErrorHandling::format_source_location(),
                                                      ErrorHandling::get_error_message(errno)));
            }
            return {};
        }

        template <typename Address>
        [[nodiscard]] auto accept(Address& address,
                                  ::socklen_t& address_length) const -> ErrorHandling::Result<
            SocketResource, std::string>
        {
            int client_socket = ::accept(m_socket_fd, (::sockaddr*)&address, &address_length);
            if (client_socket == -1)
            {
                return ErrorHandling::Err(std::format("{}: failed to accept connection: {}",
                                                      ErrorHandling::format_source_location(),
                                                      ErrorHandling::get_error_message(errno)));
            }
            return ErrorHandling::Ok(SocketResource(client_socket));
        }

        template<typename T>
        [[nodiscard]] auto write(
            const T& value) const -> ErrorHandling::Result<void, std::string>
        {
            constexpr auto size = sizeof(T);
            const auto bytes_written = ::write(m_socket_fd, &value, sizeof(T));
            if (bytes_written == -1)
            {
                return ErrorHandling::Err(std::format("{}: failed to write to socket: {}",
                                                      ErrorHandling::format_source_location(),
                                                      ErrorHandling::get_error_message(errno)));
            }
            if (bytes_written != size)
            {
                return ErrorHandling::Err(std::format("{}: only wrote {} of {} bytes from socket: {}",
                                                      ErrorHandling::format_source_location(), bytes_written, size,
                                                      ErrorHandling::get_error_message(errno)));
            }
            return {};
        }

        [[nodiscard]] auto write(
            const std::span<std::byte>& data) const -> ErrorHandling::Result<std::size_t, std::string>
        {
            const auto bytes_written = ::write(m_socket_fd, data.data(), data.size());
            if (bytes_written == -1)
            {
                return ErrorHandling::Err(std::format("{}: failed to write to socket: {}",
                                                      ErrorHandling::format_source_location(),
                                                      ErrorHandling::get_error_message(errno)));
            }
            return {static_cast<std::size_t>(bytes_written)};
        }

        [[nodiscard]] auto write_exactly(
            const std::span<std::byte>& data) const -> ErrorHandling::Result<void, std::string>
        {
            auto result = write(data);
            if (!result)
            {
                return ErrorHandling::Err(std::move(result.error()));
            }
            if (*result != data.size())
            {
                return ErrorHandling::Err(std::format("{}: only wrote {} of {} bytes to socket: {}",
                                                      ErrorHandling::format_source_location(), *result, data.size(),
                                                      ErrorHandling::get_error_message(errno)));
            }
            return {};
        }


        template <typename T>
        [[nodiscard]] auto read() const -> ErrorHandling::Result<T, std::string>
        {
            constexpr auto size = sizeof(T);
            T value;
            const auto bytes_read = ::read(m_socket_fd, &value, size);
            if (bytes_read == -1)
            {
                return ErrorHandling::Err(std::format("{}: failed to read from socket: {}",
                                                      ErrorHandling::format_source_location(),
                                                      ErrorHandling::get_error_message(errno)));
            }
            if (bytes_read != size)
            {
                return ErrorHandling::Err(std::format("{}: only read {} of {} bytes from socket: {}",
                                                      ErrorHandling::format_source_location(), bytes_read, size,
                                                      ErrorHandling::get_error_message(errno)));
            }
            return ErrorHandling::Ok(std::move(value));
        }

        [[nodiscard]] auto read(std::span<std::byte>& data) const -> ErrorHandling::Result<std::size_t, std::string>
        {
            const auto bytes_read = ::read(m_socket_fd, data.data(), data.size());
            if (bytes_read == -1)
            {
                return ErrorHandling::Err(std::format("{}: failed to read from socket: {}",
                                                      ErrorHandling::format_source_location(),
                                                      ErrorHandling::get_error_message(errno)));
            }
            return {static_cast<std::size_t>(bytes_read)};
        }

        [[nodiscard]] auto read_exactly(std::span<std::byte>& data) const -> ErrorHandling::Result<std::size_t, std::string>
        {
            auto result = read(data);
            if (!result)
            {
                return ErrorHandling::Err(std::move(result.error()));
            }
            if (*result != data.size())
            {
                return ErrorHandling::Err(std::format("{}: only read {} of {} bytes from socket: {}",
                                                      ErrorHandling::format_source_location(), *result, data.size(),
                                                      ErrorHandling::get_error_message(errno)));
            }
            return {};
        }
    };
} // namespace CppPlayground::Networking

#endif
