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

#include <print>
#include <unordered_map>

#include "error.hpp"
#include "core/token.hpp"
#include "core/ast/declarations.hpp"

namespace tlc::core::parse {

class Parser {
public:
  explicit Parser(std::vector<Token>&& tokens) : _tokens{std::move(tokens)} {}

  std::optional<std::vector<ast::declarations::Declaration>> parse();

private:
  [[nodiscard]] const Token& peek() const { return _tokens[_pos]; }
  [[nodiscard]] const Token& peek_next() const { return _tokens[_pos + 1]; }
  const Token& advance() { return _tokens[_pos++]; }

  [[nodiscard]] bool at_end() const { return check(TokenType::EndOfFile); }
  [[nodiscard]] bool check(const TokenType t) const { return peek().type == t; }
  [[nodiscard]] bool check_next(const TokenType t) const { return peek_next().type == t; }
  [[nodiscard]] bool check_operator() const { return check(TokenType::Plus) || check(TokenType::Minus) || check(TokenType::Slash) || check(TokenType::Star); }

  [[nodiscard]] bool match(const TokenType t) {
    if (check(t)) {
      advance();

      return true;
    }

    return false;
  }

  Token expect(const TokenType t) {
    if (check(t)) {
      return advance();
    }

    throw parse_exception{std::format("expected {}, but got {}", t, peek().type), peek().line, peek().column};
  }

  ast::declarations::FunctionDeclaration parse_function_declaration();
  ast::statements::StatementPointer parse_statement();
  ast::expressions::ExpressionPointer parse_expression(std::int32_t minBindingPower = 0);
  ast::expressions::ExpressionPointer parse_primary();
  ast::declarations::Block parse_block();

  ast::declarations::Parameter parse_parameter();
  std::vector<ast::declarations::Parameter> parse_parameter_list();

private:
  std::vector<Token> _tokens;
  std::size_t _pos{};

  std::unordered_map<BinaryExpressionOperation, std::int32_t> _bindingPowers = {
    {BinaryExpressionOperation::Addition, 10},
    {BinaryExpressionOperation::Subtraction, 10},
    {BinaryExpressionOperation::Multiplication, 20},
    {BinaryExpressionOperation::Division, 20},
    {BinaryExpressionOperation::Power, 30},
  };
};

} // namespace tlc::core::parse
