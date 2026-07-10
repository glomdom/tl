/**
  * tl - riscv64-first toy language
  * Copyright (C) 2026  glomdom

  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.

  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.

  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <https://www.gnu.org/licenses/>.
  */

#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <utility>
#include <variant>

#include "core/lex/lexer.hpp"
#include "core/parse/parser.hpp"
#include "core/visitors/debug.hpp"

namespace fs = std::filesystem;

using namespace tlc::core;

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: tlc <file>" << '\n';

    return -1;
  }

  fs::path inputFile = argv[1];
  std::ifstream in(inputFile, std::ios::binary);
  std::ostringstream ss;
  ss << in.rdbuf();

  auto lexer = lex::Lexer{ss.view()};
  auto tokens = lexer.tokenize();

  for (const auto& [type, lexeme, line, column] : tokens) {
    std::println("{} ({}) [{}:{}]", lexeme, type, line, column);
  }

  auto parser = parse::Parser{std::move(tokens)};
  auto declarations = parser.parse();
  if (!declarations) {
    std::println("error occurred during parsing, exiting");

    return -2;
  }

  for (const auto& decl : *declarations) {
    std::println("{}", visitors::debug::visit_declaration(decl, 0));
  }

  return 0;
}