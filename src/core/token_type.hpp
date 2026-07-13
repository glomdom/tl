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

  Plus,
  Minus,
  Star,
  Slash,

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

    case TokenType::Plus: return "plus";
    case TokenType::Minus: return "minus";
    case TokenType::Star: return "star";
    case TokenType::Slash: return "slash";

    case TokenType::Identifier: return "identifier";
    case TokenType::Number: return "number";
    case TokenType::String: return "string";

    case TokenType::EndOfFile: return "endoffile";
  }

  return "<invalid TokenType>";
}

} // namespace tlc::core

template <>
struct std::formatter<tlc::core::TokenType> : std::formatter<std::string_view> {
  auto format(const tlc::core::TokenType type, auto& ctx) const {
    return std::formatter<std::string_view>::format(tlc::core::to_view(type), ctx);
  }
};