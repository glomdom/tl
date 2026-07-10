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
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>

#include "core/lex/lexer.hpp"
#include "core/parse/parser.hpp"
#include "util/overloaded.hpp"

namespace fs = std::filesystem;

using namespace tlc::core;

std::string visit_type_variant(const ast::types::Type& type) {
  return std::visit(overloaded{
    [](const ast::types::NamedType& nt) -> std::string { return nt.name; }
  }, type);
}

std::string visit_expression(const ast::expressions::Expression& expr) {
  return std::visit(overloaded{
    [](const ast::expressions::BinaryExpression& e) -> std::string { return std::format("BinaryExpression(left={} op={} right={})", visit_expression(*e.left), e.op, visit_expression(*e.right)); },
    [](const ast::expressions::NumberExpression& e) -> std::string { return std::format("NumberExpression(value={})", e.value);},
    [](const ast::expressions::IdentifierExpression& e) -> std::string { return std::format("IdentifierExpression(name={})", e.name); },
    [](const ast::expressions::StringExpression& e) -> std::string { return std::format("StringExpression(value={})", e.value); },
    [](const ast::expressions::CallExpression& e) -> std::string {
      std::string args = e.args
        | std::views::transform([](const ast::expressions::ExpressionPointer& p) {
          return std::format("Argument(value={})", visit_expression(*p));
        })
        | std::views::join_with(std::string{", "})
        | std::ranges::to<std::string>();

      return std::format("CallExpression(callee={} args={})", visit_expression(*e.callee), args);
    },
  }, expr);
}

std::string visit_statement(const ast::statements::Statement& stmt) {
  return std::visit(overloaded{
    [](const ast::statements::ReturnStatement& s) -> std::string { return std::format("Return(expr={})", visit_expression(*s.returned)); },
    [](const ast::statements::ExpressionStatement& s) -> std::string { return visit_expression(*s.expression); },
  }, stmt);
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
    std::visit(overloaded{
      [](const ast::declarations::FunctionDeclaration& fn) {
        std::string params = fn.parameters
          | std::views::transform([](const ast::declarations::Parameter& p) {
            return std::format("Parameter(name={} type={})", p.name, visit_type_variant(*p.type));
          })
          | std::views::join_with(std::string{", "})
          | std::ranges::to<std::string>();

        std::string statements = fn.body.statements
          | std::views::transform([](const ast::statements::StatementPointer& s) {
            return std::format("  {}", visit_statement(*s));
          })
          | std::views::join_with(std::string{"\n"})
          | std::ranges::to<std::string>();

        std::println("Function(name={} params={} returnType={}) {{", fn.name, params, visit_type_variant(*fn.returnType));
        std::println("{}", statements);
        std::println("}}");
      },
    }, decl);
  }

  return 0;
}