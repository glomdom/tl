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

#include "lexer.hpp"

#include <cstddef>
#include <vector>

#include "core/token.hpp"
#include "core/token_type.hpp"

namespace {

constexpr bool isDigit(const char c) {
  return c >= '0' && c <= '9';
}

constexpr bool isAlpha(const char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

constexpr bool isAlphaNumeric(const char c) {
  return isAlpha(c) || isDigit(c);
}

} // unnamed namespace

namespace tlc::core::lex {

std::vector<Token> Lexer::tokenize() {
  const std::size_t textSize = _text.size();

  while (_pos < textSize) {
    skip_trivia();

    if (_pos >= textSize) {
      break;
    }

    const char c = peek();
    if (isAlpha(c)) {
      scan_identifier();
    } else if (isDigit(c)) {
      scan_digit();
    } else if (c == '"') {
      scan_string();
    } else {
      scan_punctuation();
    }
  }

  _tokens.push_back(Token{TokenType::EndOfFile, "EOF", _line, _col});

  return _tokens;
}

char Lexer::advance() {
  const char c = _text[_pos];
  _pos += 1;
  _col += 1;

  return c;
}

char Lexer::peek() const {
  if (_pos >= _text.size()) {
    return '\0';
  }

  return _text[_pos];
}

char Lexer::peek_next(std::size_t amount) const {
  if (_pos + amount >= _text.size()) {
    return '\0';
  }

  return _text[_pos + amount];
}

void Lexer::skip_trivia() {
  while (true) {
    const char c = peek();
    if (c == ' ' || c == '\t' || c == '\r') {
      advance();
    } else if (c == '\n') {
      _pos += 1;
      _line += 1;
      _col = 1;
    } else if (c == '/' && peek_next() == '/') {
      while (peek() != '\n' && peek() != '\0') {
        advance();
      }
    } else {
      return;
    }
  }
}

void Lexer::scan_punctuation() {
  const int startLine = _line;
  const int startCol = _col;
  const char c = peek();

  switch (c) {
    case '-': {
      if (peek_next() == '>') {
        advance();
        advance();

        _tokens.push_back(Token{TokenType::RightArrow, "->", startLine, startCol});
      } else {
        advance();
      }

      break;
    }

    case '(': {
      advance();
      _tokens.push_back(Token{TokenType::LeftParenthesis, "(", startLine, startCol});

      break;
    }

    case ')': {
      advance();
      _tokens.push_back(Token{TokenType::RightParenthesis, ")", startLine, startCol});

      break;
    }

    case '{': {
      advance();
      _tokens.push_back(Token{TokenType::LeftBrace, "{", startLine, startCol});

      break;
    }

    case '}': {
      advance();
      _tokens.push_back(Token{TokenType::RightBrace, "}", startLine, startCol});

      break;
    }

    case ':': {
      advance();
      _tokens.push_back(Token{TokenType::Colon, ":", startLine, startCol});

      break;
    }

    case ';': {
      advance();
      _tokens.push_back(Token{TokenType::Semicolon, ";", startLine, startCol});

      break;
    }

    case ',': {
      advance();
      _tokens.push_back(Token{TokenType::Comma, ",", startLine, startCol});

      break;
    }

    case '*': {
      advance();
      _tokens.push_back(Token{TokenType::Star, "*", startLine, startCol});

      break;
    }

    default: {
      advance();

      break;
    }
  }
}

void Lexer::scan_string() {
  const int startLine = _line;
  const int startCol = _col;

  advance();

  const std::size_t start = _pos;

  while (peek() != '"' && peek() != '\0') {
    advance();
  }

  if (peek() == '\0') {
    return; // todo: diagnostics
  }

  const std::string_view lexeme = _text.substr(start, _pos - start);

  advance();
  _tokens.push_back(Token{TokenType::String, lexeme, startLine, startCol});
}

void Lexer::scan_digit() {
  const int startLine = _line;
  const int startCol = _col;
  const std::size_t start = _pos;

  while (isDigit(peek())) {
    advance();
  }

  const std::string_view lexeme = _text.substr(start, _pos - start);
  _tokens.push_back(Token{TokenType::Number, lexeme, startLine, startCol});
}

void Lexer::scan_identifier() {
  const int startLine = _line;
  const int startCol = _col;
  const std::size_t start = _pos;

  while (isAlphaNumeric(peek())) {
    advance();
  }

  const std::string_view lexeme = _text.substr(start, _pos - start);

  auto type = TokenType::Identifier;
  if (lexeme == "fn") {
    type = TokenType::Function;
  } else if (lexeme == "return") {
    type = TokenType::Return;
  }

  _tokens.push_back(Token{type, lexeme, startLine, startCol});
}

} // namespace tlc::core::lex
