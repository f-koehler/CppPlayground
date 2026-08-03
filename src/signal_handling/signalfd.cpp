#include <cstdio>
#include <iostream>

extern "C" {
#include <signal.h>
#include <sys/signalfd.h>
#include <unistd.h>
}

int main() {
  const auto pid = getpid();
  std::cout << "I am process " << pid << ", waiting for interrupt signals\n";

  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGINT);

  // block default signal handling
  if (sigprocmask(SIG_BLOCK, &mask, nullptr) == -1) {
    perror("sigprocmask");
    return 1;
  }

  // open signalfd: -1 -> new signal fd
  int sfd = signalfd(-1, &mask, 0);
  if (sfd == -1) {
    perror("signalfd");
    return 1;
  }

  // wait for signals
  signalfd_siginfo fdsi{};
  ssize_t bytes_read = 0;
  while (true) {
    // blocking read
    bytes_read = read(sfd, &fdsi, sizeof(signalfd_siginfo));
    if (bytes_read != sizeof(signalfd_siginfo)) {
      perror("read");
      return 1;
    }

    // print
    std::cout << "Received signal " << fdsi.ssi_signo << " from process "
              << fdsi.ssi_pid << '\n';
  }

  return 0;
}
