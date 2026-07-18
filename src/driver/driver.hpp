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

#pragma once

#include <vector>

#include "options.hpp"
#include "core/token.hpp"
#include "core/ast/declarations.hpp"
#include "core/diagnostics/diagnostics.hpp"

namespace tlc::driver {

class Driver {
public:
  explicit Driver(Options&& options) : _options{std::move(options)} {}

  bool run();

private:
  void read_source();
  void lex();
  void parse();

private:
  Options _options;
  core::diagnostics::Diagnostics _diagnostics;

  std::string _source;
  std::vector<core::Token> _tokens;
  std::vector<core::ast::declarations::Declaration> _ast;
};

} // namespace tlc::driver
