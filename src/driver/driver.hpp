#pragma once

#include <vector>

#include "options.hpp"
#include "core/token.hpp"
#include "core/ast/declarations.hpp"

namespace tlc::driver {

class Driver {
public:
  explicit Driver(Options&& options) : _options{std::move(options)} {}

  bool run();

private:
  bool read_source();
  bool lex();
  bool parse();

private:
  Options _options;

  std::string _source;
  std::vector<core::Token> _tokens;
  std::vector<core::ast::declarations::Declaration> _ast;
};

} // namespace tlc::driver
