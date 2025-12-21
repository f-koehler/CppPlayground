#ifndef CPP_PLAYGROUND_EPOLL_HPP
#define CPP_PLAYGROUND_EPOLL_HPP

#include <sys/epoll.h>
#include <unistd.h>

#include "cpp_playground/error_handling/error_code.hpp"
#include "cpp_playground/error_handling/result.hpp"
#include "cpp_playground/error_handling/source_location.hpp"
#include <vector>

namespace CppPlayground::Networking {
class EpollInstance {
public:
  using Event = ::epoll_event;

  enum class TriggerType : bool {
    LevelTriggered = false,
    EdgeTriggered = true,
  };

private:
  int m_epoll_fd = -1;
  Event m_event{};

  explicit EpollInstance(const int fd) noexcept : m_epoll_fd(fd) {}

public:
  EpollInstance(const EpollInstance &) = delete;
  EpollInstance(EpollInstance &&other) noexcept : m_epoll_fd(other.m_epoll_fd) {
    other.m_epoll_fd = -1;
  }
  EpollInstance &operator=(const EpollInstance &) = delete;

  EpollInstance &operator=(EpollInstance &&other) noexcept {
    if (this == &other) {
      return *this;
    }
    if (m_epoll_fd != -1) {
      if (::close(m_epoll_fd) == -1) {
        std::println("{}: failed to close epoll fd: {}",
                     ErrorHandling::format_source_location(),
                     ErrorHandling::get_error_message(errno));
      }
    }
    m_epoll_fd = other.m_epoll_fd;
    other.m_epoll_fd = -1;
    return *this;
  }

  ~EpollInstance() {
    if (m_epoll_fd != -1) {
      if (::close(m_epoll_fd) == -1) {
        std::println("{}: failed to close epoll fd: {}",
                     ErrorHandling::format_source_location(),
                     ErrorHandling::get_error_message(errno));
      }
    }
  }

  [[nodiscard]] static auto create_epoll_instance()
      -> ErrorHandling::Result<EpollInstance, std::string> {
    const int fd = ::epoll_create1(0);
    if (fd == -1) {
      return ErrorHandling::Err(
          std::format("{}: failed to create epoll file descriptor: {}",
                      ErrorHandling::format_source_location(),
                      ErrorHandling::get_error_message(errno)));
    }
    return EpollInstance(fd);
  }

  [[nodiscard]] auto
  add_socket(const int fd,
             const TriggerType trigger_type = TriggerType::LevelTriggered)
      -> ErrorHandling::Result<void, std::string> {
    if (trigger_type == TriggerType::LevelTriggered) {
      m_event.events = ::EPOLLIN;
    } else {
      m_event.events = ::EPOLLIN | ::EPOLLET;
    }
    m_event.data.fd = fd;
    if (::epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &m_event) == -1) {
      return ErrorHandling::Err(std::format(
          "{}: failed add socket: {}", ErrorHandling::format_source_location(),
          ErrorHandling::get_error_message(errno)));
    }
    return {};
  }

  [[nodiscard]] auto wait(std::vector<Event> &events) const noexcept
      -> ErrorHandling::Result<void, std::string> {
    const int num_ready =
        ::epoll_pwait(m_epoll_fd, events.data(),
                      static_cast<int>(events.size()), -1, nullptr);
    if (num_ready == -1) {
      return ErrorHandling::Err(
          std::format("{}: failed to wait for epoll events: {}",
                      ErrorHandling::format_source_location(),
                      ErrorHandling::get_error_message(errno)));
    }
    events.resize(num_ready);
    return {};
  }
};
} // namespace CppPlayground::Networking

#endif // CPP_PLAYGROUND_EPOLL_HPP
