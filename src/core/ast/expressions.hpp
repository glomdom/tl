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

#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace tlc::core::ast::expressions {

struct NumberExpression;
struct StringExpression;
struct IdentifierExpression;
struct BinaryExpression;
struct CallExpression;

using Expression = std::variant<NumberExpression, StringExpression, IdentifierExpression, BinaryExpression, CallExpression>;
using ExpressionPointer = std::unique_ptr<Expression>;

struct NumberExpression {
  std::uint64_t value;
};

struct StringExpression {
  std::string value;
};

struct IdentifierExpression {
  std::string name;
};

enum class BinaryExpressionOperation {
  Division,
  Multiplication,
  Power,
  Plus,
  Minus,
};

struct BinaryExpression {
  ExpressionPointer left;
  BinaryExpressionOperation op;
  ExpressionPointer right;
};

struct CallExpression {
  ExpressionPointer callee;
  std::vector<ExpressionPointer> args;
};

constexpr std::string_view to_view(const BinaryExpressionOperation op) {
  switch (op) {
    case BinaryExpressionOperation::Multiplication: return "*";

    default: return "<unhandled BinaryExpressionOperation to_view>";
  }
}

} // namespace tlc::core::ast::expressions

using tlc::core::ast::expressions::BinaryExpressionOperation;

template <>
struct std::formatter<BinaryExpressionOperation> : std::formatter<std::string_view> {
  auto format(const BinaryExpressionOperation op, auto& ctx) const {
    return std::formatter<std::string_view>::format(to_view(op), ctx);
  }
};