cmake_minimum_required(VERSION 3.30)
project(tl CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
  Catch2
  QUIET
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG devel
)
FetchContent_Declare(
  cli11
  QUIET
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG v2.6.2
)
FetchContent_MakeAvailable(Catch2 cli11)

enable_testing()
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)

add_library(tlc_core)
target_compile_options(tlc_core PRIVATE
  -Wall -Wextra -Wpedantic -Werror
)

target_sources(tlc_core PRIVATE
  src/core/diagnostics/diagnostics.cpp
  src/core/lex/lexer.cpp
  src/core/parse/parser.cpp
  src/core/semantics/resolving/types.cpp

  src/driver/driver.cpp
)

target_include_directories(tlc_core PUBLIC src)

add_executable(tlc)
target_compile_options(tlc PRIVATE
  -Wall -Wextra -Wpedantic -Werror
)

target_sources(tlc PRIVATE
  src/main.cpp
)

target_link_libraries(tlc PRIVATE CLI11::CLI11 tlc_core)

add_subdirectory(tests)
