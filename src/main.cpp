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

#include "core/ast/declarations.hpp"
#include "core/ast/expressions.hpp"
#include "core/ast/statements.hpp"
#include "core/ast/types.hpp"
#include "core/lex/lexer.hpp"
#include "core/parse/parser.hpp"
#include "util/overloaded.hpp"

namespace fs = std::filesystem;

using namespace tlc::core;

std::string visit_type(const ast::types::Type& type) {
  return std::visit(overloaded{
    [=](const ast::types::NamedType& nt) -> std::string {
      return nt.name;
    }
  }, type);
}

std::string visit_expression(const ast::expressions::Expression& expr, const std::size_t depth) {
  const auto padding = std::string(depth * 2, ' ');

  return std::visit(overloaded{
    [=](const ast::expressions::BinaryExpression& e) -> std::string {
      std::string result = std::format("{}BinaryExpression {}\n{}\n{}", padding, e.op, visit_expression(*e.left, depth + 1), visit_expression(*e.right, depth + 1));

      return result;
    },
    [=](const ast::expressions::NumberExpression& e) -> std::string { return std::format("{}NumberExpression {}", padding, e.value);},
    [=](const ast::expressions::IdentifierExpression& e) -> std::string { return std::format("{}IdentifierExpression {}", padding, e.name); },
    [=](const ast::expressions::StringExpression& e) -> std::string { return std::format("{}StringExpression {}", padding, e.value); },
    [=](const ast::expressions::CallExpression& e) -> std::string {
      std::string args = e.args
        | std::views::transform([=](const ast::expressions::ExpressionPointer& p) {
          return std::format("{}", visit_expression(*p, depth + 1));
        })
        | std::views::join_with(std::string{", "})
        | std::ranges::to<std::string>();

      return std::format("{}CallExpression\n{}\n{}", padding, visit_expression(*e.callee, depth + 1), args);
    },
  }, expr);
}

std::string visit_statement(const ast::statements::Statement& stmt, const std::size_t depth) {
  return std::visit(overloaded{
    [=](const ast::statements::ReturnStatement& s) -> std::string {
      const auto padding = std::string(depth * 2, ' ');

      std::string result = std::format("{}Return\n{}", padding, visit_expression(*s.returned, depth + 1));

      return result;
    },
    [=](const ast::statements::ExpressionStatement& s) -> std::string { return visit_expression(*s.expression, depth); },
  }, stmt);
}

std::string visit_function_declaration(const ast::declarations::FunctionDeclaration& fn, const std::size_t depth) {
  const auto padding = std::string(depth * 2, ' ');

  std::string params = fn.parameters
    | std::views::transform([=](const ast::declarations::Parameter& p) {
      return std::format("  {}Parameter {}", padding, p.name);
    })
    | std::views::join_with(std::string{"\n"})
    | std::ranges::to<std::string>();

  std::string statements = fn.body.statements
    | std::views::transform([=](const ast::statements::StatementPointer& s) {
      return std::format("{}", visit_statement(*s, depth + 1));
    })
    | std::views::join_with(std::string{"\n"})
    | std::ranges::to<std::string>();

  std::string result = std::format("{0}Function {1}\n{0}{2}\n{0}{3}", padding, fn.name, params, statements);

  return result;
}

std::string visit_declaration(const ast::declarations::Declaration& decl, const std::size_t depth) {
  return std::visit(overloaded{
    [=](const ast::declarations::FunctionDeclaration& fn) {
      return visit_function_declaration(fn, depth);
    },
  }, decl);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: tlc <file>" << '\n';

    return -1;
  }

  fs::path inputFile = argv[1];
  std::ifstream in(inputFile, std::ios::binary);
  std::ostringstream ss;
  ss << in.rdbuf();

  auto lexer = lex::Lexer{ss.view()};
  auto tokens = lexer.tokenize();

  for (const auto& [type, lexeme, line, column] : tokens) {
    std::println("{} ({}) [{}:{}]", lexeme, type, line, column);
  }

  auto parser = parse::Parser{std::move(tokens)};
  auto declarations = parser.parse();
  if (!declarations) {
    std::println("error occurred during parsing, exiting");

    return -2;
  }

  for (const auto& decl : *declarations) {
    std::println("{}", visit_declaration(decl, 0));
  }

  return 0;
}