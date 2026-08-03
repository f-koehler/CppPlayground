#include <chrono>
#include <cstdint>
#include <cstdio>
#include <print>

extern "C" {
#include <sys/time.h>
#include <sys/timerfd.h>
#include <unistd.h>
}

int main() {
  // create timerfd
  int tfd = timerfd_create(CLOCK_MONOTONIC, 0);
  if (tfd == -1) {
    perror("timerfd_create");
    return 1;
  }

  // arm timerfd: flags==0 -> relative time
  itimerspec spec{};
  spec.it_value.tv_sec = 1;    // first trigger after 1s
  spec.it_interval.tv_sec = 1; // periodic, every 1s
  if (timerfd_settime(tfd, 0, &spec, nullptr) == -1) {
    perror("timerfd_settime");
    return 1;
  }

  ssize_t bytes_read = 0;
  uint64_t num_expirations = 0;
  while (true) {
    bytes_read = read(tfd, &num_expirations, sizeof(num_expirations));
    if (bytes_read != sizeof(num_expirations)) {
      perror("read");
      return 1;
    }
    auto now = std::chrono::system_clock::now();
    std::println("[{:%H:%M:%S}] +{:06} µs Timer expired {} time(s)",
                 std::chrono::floor<std::chrono::seconds>(now),
                 std::chrono::duration_cast<std::chrono::microseconds>(
                     now.time_since_epoch())
                         .count() %
                     1'000'000,
                 num_expirations);
  }

  return 0;
}
