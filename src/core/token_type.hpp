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

#include <format>
#include <string_view>

namespace tlc::core {

enum class TokenType {
  Function,
  Return,

  RightArrow,
  LeftParenthesis,
  RightParenthesis,
  LeftBrace,
  RightBrace,
  Colon,
  Semicolon,
  Comma,

  Star,

  Identifier,
  Number,
  String,

  EndOfFile,
};

constexpr std::string_view to_view(const TokenType type) {
  switch (type) {
    case TokenType::Function: return "function";
    case TokenType::Return: return "return";

    case TokenType::RightArrow: return "rightarrow";
    case TokenType::LeftParenthesis: return "leftparenthesis";
    case TokenType::RightParenthesis: return "rightparenthesis";
    case TokenType::LeftBrace: return "leftbrace";
    case TokenType::RightBrace: return "rightbrace";
    case TokenType::Colon: return "colon";
    case TokenType::Semicolon: return "semicolon";
    case TokenType::Comma: return "comma";

    case TokenType::Star: return "star";

    case TokenType::Identifier: return "identifier";
    case TokenType::Number: return "number";
    case TokenType::String: return "string";

    case TokenType::EndOfFile: return "endoffile";
  }

  return "<invalid TokenType>";
}

} // namespace tlc::core

using tlc::core::TokenType;

template <>
struct std::formatter<TokenType> : std::formatter<std::string_view> {
  auto format(const TokenType type, auto& ctx) const {
    return std::formatter<std::string_view>::format(to_view(type), ctx);
  }
};