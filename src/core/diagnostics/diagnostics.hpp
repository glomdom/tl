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

#include <vector>

#include "diagnostic.hpp"

namespace tlc::core::diagnostics {

class Diagnostics {
public:
  explicit Diagnostics() = default;

  Diagnostics(const Diagnostics&) = delete;
  Diagnostics& operator=(const Diagnostics&) = delete;

  void render() const;
  bool has_error() const;

  void error(std::string message, int line, int col);
  void warning(std::string message, int line, int col);
  void note(std::string message, int line, int col);

private:
  static std::string get_severity_string(const Severity severity) {
    switch (severity) {
      case Severity::NOTE: return "note";
      case Severity::WARNING: return "warning";
      case Severity::ERROR: return "error";
    }

    return "<invalid severity>";
  }

private:
  std::vector<Diagnostic> _diagnostics;
};

} // namespace tlc::core::diagnostics
