#include <CLI/CLI.hpp>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdint>
#include <future>
#include <iterator>
#include <limits>
#include <memory>
#include <print>
#include <ranges>
#include <thread>
#include <vector>

struct Result {
  uint64_t thread_id = std::numeric_limits<uint64_t>::max();
  uint64_t time = 0;
  std::chrono::high_resolution_clock::time_point start_time;
};

static constexpr uint64_t Goal = 1UL << 34UL;

int main(int argc, char **argv) {
  CLI::App app{"Which thread will be the fastest"};
  argv = app.ensure_utf8(argv);

  std::uint64_t num_threads = 4;
  app.add_option("-n,--num-threads", num_threads, "Number of threads to race")
      ->check(CLI::Range(uint64_t{1}, std::numeric_limits<uint64_t>::max()));
  CLI11_PARSE(app, argc, argv);

  std::atomic_uint64_t num_threads_ready{0};
  std::condition_variable cv_start;
  std::mutex mutex;

  auto thread_func = [num_threads, &num_threads_ready, &cv_start, &mutex](
                         uint64_t thread_id, std::promise<Result> &&promise) {
    const bool ready_to_go = (++num_threads_ready) == num_threads;
    if (ready_to_go) {
      cv_start.notify_all();
    } else {
      std::unique_lock<std::mutex> lock(mutex);
      cv_start.wait(lock, [&num_threads_ready, &num_threads] {
        return num_threads_ready.load() == num_threads;
      });
    }

    volatile uint64_t counter = 0;
    const auto start = std::chrono::high_resolution_clock::now();
    while (counter < Goal) {
      counter = counter + 1;
    }
    const auto stop = std::chrono::high_resolution_clock::now();
    std::move(promise).set_value(
        Result{.thread_id = thread_id,
               .time = static_cast<uint64_t>(
                   std::chrono::nanoseconds(stop - start).count()),
               .start_time = start});
  };

  std::vector<std::jthread> threads;
  std::vector<std::future<Result>> future_results;
  threads.reserve(num_threads);
  future_results.reserve(num_threads);
  for (uint64_t i = 0; i < num_threads; ++i) {
    std::promise<Result> promise;
    future_results.push_back(promise.get_future());
    threads.emplace_back(thread_func, i, std::move(promise));
  }

  std::vector<Result> results(
      std::from_range,
      future_results | std::views::transform([](std::future<Result> &future) {
        return future.get();
      }));

  std::ranges::sort(results, [](const Result &result1, const Result &result2) {
    return result1.time < result2.time;
  });

  // print the scoreboard
  uint64_t position = 0;
  for (const auto &result : results) {
    std::println("#{}: thread {}: {} ns", ++position, result.thread_id,
                 result.time);
  }

  // sprint the start times for debugging
  const auto first_start_time =
      std::ranges::min_element(results, [](const Result &result1,
                                           const Result &result2) {
        return result1.start_time < result2.start_time;
      })->start_time;
  std::vector<uint64_t> start_delay(
      std::from_range,
      results | std::views::transform([&first_start_time](
                                          const Result &result) {
        return std::chrono::nanoseconds(result.start_time - first_start_time)
            .count();
      }));
  for (uint64_t i = 0; i < num_threads; ++i) {
    std::println("thread {}: {} ns", results[i].thread_id, start_delay[i]);
  }

  return 0;
}
