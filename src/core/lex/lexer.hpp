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

#include "core/token.hpp"

#include <string_view>
#include <vector>

namespace tlc::core::lex {

class Lexer {
public:
  explicit Lexer(const std::string_view text) : _text{text} {}

  std::vector<Token> tokenize();

private:
  [[nodiscard]] char peek() const;
  [[nodiscard]] char peek_next(std::size_t amount = 1) const;

  char advance();
  void skip_trivia();
  void skip_whitespace();
  void scan_punctuation();
  void scan_string();
  void scan_identifier();
  void scan_digit();

private:
  std::string_view _text;
  std::vector<Token> _tokens{};

  std::size_t _pos{};
  int _line{1};
  int _col{1};
};

} // namespace tlc::core::lex