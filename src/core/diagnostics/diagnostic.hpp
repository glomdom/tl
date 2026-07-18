#pragma once

#include <cstdint>
#include <string>

namespace tlc::core::diagnostics {

enum class Severity : std::uint8_t {
  NOTE,
  WARNING,
  ERROR,
};

struct Diagnostic {
  Severity severity;
  std::string message;

  int line, col;
};

} // namespace tlc::core::diagnostics
