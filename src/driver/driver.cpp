#include "driver.hpp"

#include <fstream>
#include <sstream>

#include "core/lex/lexer.hpp"
#include "core/parse/parser.hpp"
#include "core/visitors/debug.hpp"

namespace tlc::driver {

bool Driver::run() {
  if (!read_source()) {
    return false;
  }

  if (!lex()) {
    return false;
  }

  if (!parse()) {
    return false;
  }

  return true;
}

bool Driver::read_source() {
  std::ifstream in(_options.inputFile, std::ios::binary);
  if (!in) {
    // todo: add diagnostic

    return false;
  }

  std::ostringstream ss;
  ss << in.rdbuf();

  if (in.bad()) {
    // todo: add diagnostic

    return false;
  }

  _source = ss.str();

  return true;
}

bool Driver::lex() {
  auto lexer = core::lex::Lexer{_source};
  _tokens = lexer.tokenize();

  if (_options.dumpTokens) {
    for (const auto& [type, lexeme, line, column] : _tokens) {
      std::println("{} ({}) [{}:{}]", lexeme, type, line, column);
    }
  }

  return true;
}

bool Driver::parse() {
  auto parser = core::parse::Parser{std::move(_tokens)};
  auto declarations = parser.parse();
  if (!declarations.has_value()) {
    return false;
  }

  _ast = std::move(*declarations);

  if (_options.dumpAst) {
    for (const auto& decl : _ast) {
      std::println("{}", visitors::debug::visit_declaration(decl, 0));
    }
  }

  return true;
}

} // namespace tlc::driver