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

#include <string>
#include <utility>
#include <exception>
#include <print>
#include <cstdio>

#include <CLI/CLI.hpp>

#include "driver/driver.hpp"
#include "driver/options.hpp"

namespace fs = std::filesystem;

using namespace tlc;

int main(int argc, char* argv[]) {
  try {
    CLI::App app{"A toy language"};

    bool dumpTokens{false};
    bool dumpAst{false};
    std::string inputPath;

    app.add_flag("--dump-tokens", dumpTokens, "Prints the tokens created by the lexer")
        ->group("DEBUG");

    app.add_flag("--dump-ast", dumpAst, "Prints the AST created by the parser")
        ->group("DEBUG");

    app.add_option("input", inputPath, "The file to compile")
        ->required()
        ->check(CLI::ExistingFile);

    CLI11_PARSE(app, argc, argv);

    auto opts = driver::Options{
        .dumpAst = dumpAst,
        .dumpTokens = dumpTokens,
        .inputFile = std::move(inputPath),
    };

    auto driver = driver::Driver{std::move(opts)};
    auto success = driver.run();
    if (!success) {
        // todo: report diagnostics

        return -1;
    }
  } catch (std::exception& e) {
    std::println(stderr, "fatal: {}", e.what());

    return 1;
  }

  return 0;
}
