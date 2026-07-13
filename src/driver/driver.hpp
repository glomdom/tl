#pragma once

#include <vector>

#include "options.hpp"
#include "core/token.hpp"
#include "core/ast/declarations.hpp"
#include "core/diagnostics/diagnostics.hpp"

namespace tlc::driver {

class Driver {
public:
  explicit Driver(Options&& options) : _options{std::move(options)} {}

  bool run();

private:
  void read_source();
  void lex();
  void parse();

private:
  Options _options;
  core::diagnostics::Diagnostics _diagnostics;

  std::string _source;
  std::vector<core::Token> _tokens;
  std::vector<core::ast::declarations::Declaration> _ast;
};

} // namespace tlc::driver
