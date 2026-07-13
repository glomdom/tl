add_executable(tests)
target_sources(tests PRIVATE lexer_tests.cpp)
target_link_libraries(tests PRIVATE tlc_core Catch2::Catch2WithMain)

catch_discover_tests(tests)