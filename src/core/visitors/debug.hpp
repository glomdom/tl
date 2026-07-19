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
#include <print>
#include <ranges>
#include <string>
#include <variant>

#include "core/ast/declarations.hpp"
#include "core/ast/expressions.hpp"
#include "core/ast/statements.hpp"
#include "core/ast/types.hpp"
#include "core/semantics/resolving/types.hpp"
#include "util/overloaded.hpp"

using namespace tlc::core;

namespace tlc::core::visitors::debug {

inline std::string visit_type(const ast::types::Type& type, const std::size_t depth) {
  const auto padding = std::string(depth * 2, ' ');

  return std::visit(overloaded{
    [=](const ast::types::NamedType& nt) -> std::string {
      std::println("Resolved type: {}", semantics::resolving::resolve_type(nt));

      return std::format("{}NamedType {}", padding, nt.name);
    }
  }, type);
}

inline std::string visit_expression(const ast::expressions::Expression& expr, const std::size_t depth) {
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

inline std::string visit_statement(const ast::statements::Statement& stmt, const std::size_t depth) {
  return std::visit(overloaded{
    [=](const ast::statements::ReturnStatement& s) -> std::string {
      const auto padding = std::string(depth * 2, ' ');

      std::string result = std::format("{}Return\n{}", padding, visit_expression(*s.returned, depth + 1));

      return result;
    },
    [=](const ast::statements::ExpressionStatement& s) -> std::string { return visit_expression(*s.expression, depth); },
  }, stmt);
}

inline std::string visit_function_declaration(const ast::declarations::FunctionDeclaration& fn, const std::size_t depth) {
  const auto padding = std::string(depth * 2, ' ');

  std::string params = fn.parameters
    | std::views::transform([=](const ast::declarations::Parameter& p) {
      return std::format("  {}Parameter {}\n    {}", padding, p.name, visit_type(*p.type, depth));
    })
    | std::views::join_with(std::string{"\n"})
    | std::ranges::to<std::string>();

  std::string statements = fn.body.statements
    | std::views::transform([=](const ast::statements::StatementPointer& s) {
      return std::format("{}", visit_statement(*s, depth + 1));
    })
    | std::views::join_with(std::string{"\n"})
    | std::ranges::to<std::string>();

  std::string result = std::format("{0}Function {1}\n{0}{2}\n{0}{3}\n{0}{4}", padding, fn.name, params, visit_type(*fn.returnType, depth + 1), statements);

  return result;
}

inline std::string visit_declaration(const ast::declarations::Declaration& decl, const std::size_t depth) {
  return std::visit(overloaded{
    [=](const ast::declarations::FunctionDeclaration& fn) {
      return visit_function_declaration(fn, depth);
    },
  }, decl);
}

}
