#pragma once

#include <string>

namespace tlc::core::diagnostics {

enum class Severity {
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