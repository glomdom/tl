cmake_minimum_required(VERSION 3.30)
project(tl CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(tlc)
target_compile_options(tlc PRIVATE
  -Wall -Wextra -Wpedantic -Werror
)

target_sources(tlc PRIVATE
  src/main.cpp

  src/core/lex/lexer.cpp
  src/core/parse/parser.cpp
)

target_include_directories(tlc PRIVATE src)
