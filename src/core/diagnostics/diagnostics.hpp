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