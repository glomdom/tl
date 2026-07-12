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
#include <CLI/CLI.hpp>

namespace fs = std::filesystem;

using namespace tlc::core;

int main(int argc, char* argv[]) {
  CLI::App app{"A toy language with a riscv64 backend"};

  bool dumpTokens{false};
  bool dumpAST{false};
  std::string inputPath;

  app.add_flag("--dump-tokens", dumpTokens, "Prints the tokens created by the lexer");
  app.add_flag("--dump-ast", dumpAST, "Prints the AST created by the parser");

  app.add_option("input", inputPath, "The file to compile");

  CLI11_PARSE(app, argc, argv);

  fs::path inputFile = inputPath;
  if (!exists(inputFile)) {
    app.exit(CLI::FileError{"input file was not found", -2});

    return -2;
  }

  std::ifstream in(inputFile, std::ios::binary);
  std::ostringstream ss;
  ss << in.rdbuf();

  auto lexer = lex::Lexer{ss.view()};
  auto tokens = lexer.tokenize();

  if (dumpTokens) {
    for (const auto& [type, lexeme, line, column] : tokens) {
      std::println("{} ({}) [{}:{}]", lexeme, type, line, column);
    }
  }

  auto parser = parse::Parser{std::move(tokens)};
  auto declarations = parser.parse();
  if (!declarations) {
    std::println("error occurred during parsing, exiting");

    return -2;
  }

  if (dumpAST) {
    for (const auto& decl : *declarations) {
      std::println("{}", visitors::debug::visit_declaration(decl, 0));
    }
  }

  return 0;
}