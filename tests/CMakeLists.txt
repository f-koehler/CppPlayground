add_executable(
  tests test_compile_time_gcd.cpp test_arc_pointer.cpp test_owning_pointer.cpp
        test_thread_local_lifetime_tracker.cpp)
target_link_libraries(tests PRIVATE CppPlayground Catch2::Catch2WithMain)
target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
catch_discover_tests(tests)
