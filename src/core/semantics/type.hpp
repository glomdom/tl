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

#include <cstdint>
#include <format>
#include <string_view>

namespace tlc::core::semantics {

enum class Type : std::uint8_t {
  I32,
  I64,
  String,
  Unit,
  Error,
};

constexpr std::string_view to_view(const Type type) {
  switch (type) {
    case Type::I32: return "i32";
    case Type::I64: return "i64";

    case Type::String: return "string";
    case Type::Unit: return "unit";

    case Type::Error: return "error";
  }

  return "<invalid Type>";
}

}

template <>
struct std::formatter<tlc::core::semantics::Type> : std::formatter<std::string_view> {
  auto format(const tlc::core::semantics::Type type, auto& ctx) const {
    return std::formatter<std::string_view>::format(tlc::core::semantics::to_view(type), ctx);
  }
};
