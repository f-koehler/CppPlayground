add_executable(allocation_sizes allocation_sizes.cpp)

find_package(CLI11 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
add_executable(thread_racer thread_racer.cpp)
target_link_libraries(thread_racer PRIVATE CLI11::CLI11 spdlog::spdlog)
