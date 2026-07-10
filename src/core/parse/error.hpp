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

#include <exception>
#include <string>
#include <utility>

namespace tlc::core::parse {

struct ParseError : std::exception {
  std::string message;
  int line{}, column{};

  explicit ParseError() = default;
  explicit ParseError(std::string message, const int line, const int column) : message{std::move(message)}, line{line}, column{column} {}

  [[nodiscard]] const char* what() const noexcept override {
    return message.c_str();
  }
};

} // namespace tlc::core::parse