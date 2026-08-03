add_subdirectory(signal_handling)
add_subdirectory(timers)

add_executable(allocation_sizes allocation_sizes.cpp)
add_executable(crtp crtp.cpp)
add_executable(crtp_cpp23 crtp.cpp)
target_compile_definitions(crtp_cpp23 PRIVATE CPP23_VERSION)

find_package(CLI11 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
add_executable(thread_racer thread_racer.cpp)
target_link_libraries(thread_racer PRIVATE CLI11::CLI11 spdlog::spdlog)

add_executable(matrix_view matrix_view.cpp)
set_property(TARGET matrix_view PROPERTY CXX_STANDARD 26)

add_executable(interference_size interference_size.cpp)

add_executable(coroutines_fibonacci coroutines_fibonacci.cpp)
