#include "diagnostics.hpp"

#include <algorithm>
#include <print>

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
    if (diag.severity == Severity::ERROR) hasError = true;
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
