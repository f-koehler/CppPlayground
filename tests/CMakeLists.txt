add_executable(tests test_compile_time_gcd.cpp test_arc_pointer.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
catch_discover_tests(tests)
