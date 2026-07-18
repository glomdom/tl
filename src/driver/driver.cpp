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

#include "driver.hpp"

#include <fstream>
#include <functional>
#include <sstream>
#include <print>
#include <ios>
#include <utility>

#include "core/lex/lexer.hpp"
#include "core/parse/parser.hpp"
#include "core/visitors/debug.hpp"

namespace tlc::driver {

bool Driver::run() {
  auto run_phase = [this](const std::function<void()>& phase) {
    phase();

    if (_diagnostics.has_error()) {
      _diagnostics.render();

      return false;
    }

    return true;
  };

  return run_phase([this] { read_source(); }) &&
         run_phase([this] { lex(); }) &&
         run_phase([this] { parse(); });
}

void Driver::read_source() {
  const std::ifstream in(_options.inputFile, std::ios::binary);
  if (!in) {
    _diagnostics.error("I/O error", 0, 0);

    return;
  }

  std::ostringstream ss;
  ss << in.rdbuf();

  if (in.bad()) {
    _diagnostics.error("Input stream is bad", 0, 0);

    return;
  }

  _source = ss.str();
}

void Driver::lex() {
  auto lexer = lex::Lexer{_source, _diagnostics};
  _tokens = lexer.tokenize();

  if (_options.dumpTokens) {
    for (const auto& [type, lexeme, line, column] : _tokens) {
      std::println("{} ({}) [{}:{}]", lexeme, type, line, column);
    }
  }
}

void Driver::parse() {
  auto parser = parse::Parser{std::move(_tokens), _diagnostics};
  auto declarations = parser.parse();

  _ast = std::move(declarations);

  if (_options.dumpAst) {
    for (const auto& decl : _ast) {
      std::println("{}", visitors::debug::visit_declaration(decl, 0));
    }
  }
}

} // namespace tlc::driver
