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

#include "diagnostics.hpp"

#include <print>
#include <string>
#include <utility>

#include "diagnostic.hpp"

namespace tlc::core::diagnostics {

void Diagnostics::render() const {
  for (const auto& [severity, message, line, col] : _diagnostics) {
    std::println("{}:{}:{}: {}", get_severity_string(severity), line, col, message);
  }
}

bool Diagnostics::has_error() const {
  bool hasError = false;

  for (const auto& diag : _diagnostics) {
    if (diag.severity == Severity::ERROR) {
      hasError = true;
    }
  }

  return hasError;
}

void Diagnostics::error(std::string message, const int line, const int col) {
  _diagnostics.emplace_back(Diagnostic{
    .severity = Severity::ERROR,
    .message = std::move(message),
    .line = line,
    .col = col
  });
}

void Diagnostics::warning(std::string message, const int line, const int col) {
  _diagnostics.emplace_back(Diagnostic{
    .severity = Severity::WARNING,
    .message = std::move(message),
    .line = line,
    .col = col
  });
}

void Diagnostics::note(std::string message, const int line, const int col) {
  _diagnostics.emplace_back(Diagnostic{
    .severity = Severity::NOTE,
    .message = std::move(message),
    .line = line,
    .col = col
  });
}

}
