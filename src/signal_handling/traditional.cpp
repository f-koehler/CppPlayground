#include <chrono>
#include <cstdio>
#include <iostream>
#include <thread>

extern "C" {
#include <signal.h>
#include <unistd.h>
}

/**
 * @param sig The number of the signal
 * @param info further info about the signal
 * @param ucontext Signal context information (not typically used)
 */
void handler(int sig, siginfo_t *info, [[maybe_unused]] void *ucontext) {
  std::cerr << "Received signal " << sig;
  if (info != nullptr) {
    std::cerr << " from process " << info->si_pid;
  }
  std::cerr << '\n';
}

int main() {
  const auto pid = getpid();
  std::cout << "I am process " << pid << ", waiting for interrupt signals\n";

  struct sigaction action{};
  action.sa_flags = SA_SIGINFO;
  action.sa_sigaction = &handler;
  if (sigaction(SIGINT, &action, nullptr) == -1) {
    perror("sigaction");
    return 1;
  }

  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  return 0;
}
