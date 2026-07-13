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

#include "parser.hpp"

#include <charconv>
#include <cstdint>
#include <format>
#include <memory>
#include <optional>
#include <print>
#include <system_error>
#include <utility>
#include <vector>

#include "core/token_type.hpp"
#include "core/ast/declarations.hpp"
#include "core/ast/expressions.hpp"
#include "core/ast/statements.hpp"
#include "core/ast/types.hpp"
#include "core/parse/error.hpp"

namespace tlc::core::parse {

std::vector<ast::declarations::Declaration> Parser::parse() {
  std::vector<ast::declarations::Declaration> declarations{};

  while (!check(TokenType::EndOfFile)) {
    try {
      if (check(TokenType::Function)) {
        declarations.emplace_back(parse_function_declaration());
      } else {
        const auto unhandled = advance();

        _diagnostics.note(std::format("unhandled token {}", unhandled.type), unhandled.line, unhandled.column);
      }
    } catch (const parse_exception& e) {
      _diagnostics.error(e.message, e.line, e.column);

      return {};
    }
  }
  return declarations;
}

ast::declarations::FunctionDeclaration Parser::parse_function_declaration() {
  expect(TokenType::Function);

  const auto name = expect(TokenType::Identifier);
  expect(TokenType::LeftParenthesis);

  auto parameters = parse_parameter_list();
  expect(TokenType::RightParenthesis);
  expect(TokenType::RightArrow);

  const auto returnType = expect(TokenType::Identifier);
  auto body = parse_block();

  return ast::declarations::FunctionDeclaration{
    .name = std::string{name.lexeme},
    .parameters = std::move(parameters),
    .returnType = std::make_unique<ast::types::Type>(ast::types::NamedType{std::string{returnType.lexeme}}),
    .body = std::move(body),
  };
}

ast::statements::StatementPointer Parser::parse_statement() {
  if (match(TokenType::Return)) {
    auto returnExpression = parse_expression();

    expect(TokenType::Semicolon);

    return std::make_unique<ast::statements::Statement>(ast::statements::ReturnStatement{std::move(returnExpression)});
  }

  if (check(TokenType::Identifier) && check_next(TokenType::LeftParenthesis)) {
    const auto calleeName = advance();
    expect(TokenType::LeftParenthesis);

    auto callee = std::make_unique<ast::expressions::Expression>(
      ast::expressions::IdentifierExpression{std::string{calleeName.lexeme}}
    );

    std::vector<ast::expressions::ExpressionPointer> args{};

    while (!check(TokenType::RightParenthesis)) {
      args.emplace_back(parse_expression());

      if (check(TokenType::Comma)) {
        continue;
      }

      break;
    }

    expect(TokenType::RightParenthesis);
    expect(TokenType::Semicolon);

    return std::make_unique<ast::statements::Statement>(
      ast::statements::ExpressionStatement{
        .expression = std::make_unique<ast::expressions::Expression>(
          ast::expressions::CallExpression{std::move(callee), std::move(args)}
        )
      }
    );
  }

  throw parse_exception{std::format("failed to parse statement, last token: {}", peek().type), peek().line, peek().column};
}

ast::expressions::ExpressionPointer Parser::parse_expression(std::int32_t minBindingPower) {
  auto left = parse_primary();

  while (check_operator()) {
    const auto operatorToken = peek();
    const auto lexeme = operatorToken.lexeme;

    BinaryExpressionOperation operation;
    if (lexeme == "+") operation = BinaryExpressionOperation::Addition;
    else if (lexeme == "-") operation = BinaryExpressionOperation::Subtraction;
    else if (lexeme == "*") operation = BinaryExpressionOperation::Multiplication;
    else if (lexeme == "/") operation = BinaryExpressionOperation::Division;
    else {
      throw parse_exception{std::format("invalid operator {}", operatorToken.lexeme), operatorToken.line, operatorToken.column};
    }

    const auto bindingPower = _bindingPowers[operation];
    if (bindingPower < minBindingPower) {
      break;
    }

    advance(); // operator

    auto right = parse_expression(bindingPower + 1);
    left = std::make_unique<ast::expressions::Expression>(ast::expressions::BinaryExpression{
      .left = std::move(left),
      .op = operation,
      .right = std::move(right),
    });
  }

  return left;
}

ast::expressions::ExpressionPointer Parser::parse_primary() {
  if (check(TokenType::Identifier)) {
    const auto token = advance();

    return std::make_unique<ast::expressions::Expression>(
      ast::expressions::IdentifierExpression{std::string{token.lexeme}}
    );
  }

  if (check(TokenType::Number)) {
    const auto token = advance();

    std::uint64_t val;
    auto [ptr, ec] = std::from_chars(token.lexeme.data(), token.lexeme.data() + token.lexeme.size(), val);
    if (ec != std::errc{}) {
      throw parse_exception{std::format("unable to parse number {}", token.lexeme), token.line, token.column};
    }

    return std::make_unique<ast::expressions::Expression>(
      ast::expressions::NumberExpression{val}
    );
  }

  if (check(TokenType::String)) {
    const auto value = advance().lexeme;

    return std::make_unique<ast::expressions::Expression>(
      ast::expressions::StringExpression{ .value = std::string{value} }
    );
  }

  throw parse_exception{std::format("expected expression, got {}", peek().type), peek().line, peek().column};
}

ast::declarations::Block Parser::parse_block() {
  std::vector<ast::statements::StatementPointer> statements{};

  expect(TokenType::LeftBrace);

  while (!check(TokenType::RightBrace)) {
    statements.emplace_back(parse_statement());
  }

  expect(TokenType::RightBrace);

  return ast::declarations::Block{std::move(statements)};
}

ast::declarations::Parameter Parser::parse_parameter() {
  const auto name = expect(TokenType::Identifier);
  expect(TokenType::Colon);
  const auto type = expect(TokenType::Identifier);

  return ast::declarations::Parameter{
    .name = std::string{name.lexeme},
    .type = std::make_unique<ast::types::Type>(ast::types::NamedType{std::string{type.lexeme}})
  };
}

/**
 * @brief Parses parameters from a function. Assumed that the opening parenthesis is consumed.
 */
std::vector<ast::declarations::Parameter> Parser::parse_parameter_list() {
  std::vector<ast::declarations::Parameter> parameters{};

  while (!check(TokenType::RightParenthesis)) {
    parameters.emplace_back(parse_parameter());
  }

  return parameters;
}

} // namespace tlc::core::parse
