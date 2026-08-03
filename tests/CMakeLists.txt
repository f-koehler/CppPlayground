add_executable(
  tests test_compile_time_gcd.cpp test_arc_pointer.cpp)
target_link_libraries(tests PRIVATE CppPlayground Catch2::Catch2WithMain)
target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR}/include)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  # Catch2's TEST_CASE macro relies on __COUNTER__, which recent Clang
  # versions misclassify as a C2y extension even in C++ mode.
  target_compile_options(tests PRIVATE -Wno-c2y-extensions)
endif()

add_subdirectory(containers)
add_subdirectory(concurrency)
add_subdirectory(functional)
# add_subdirectory(testing)
add_subdirectory(threading)
add_subdirectory(utility)

catch_discover_tests(tests)
