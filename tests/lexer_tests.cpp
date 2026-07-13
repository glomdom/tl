#include <catch2/catch_test_macros.hpp>

#include "core/token.hpp"
#include "core/diagnostics/diagnostics.hpp"
#include "core/lex/lexer.hpp"

using namespace tlc;

TEST_CASE("punctuation lexed properly", "[lexer]") {
  core::diagnostics::Diagnostics diagnostics;
  core::lex::Lexer lexer{"->", diagnostics};
  const std::vector<core::Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 2);
  REQUIRE(tokens[0].type == core::TokenType::RightArrow);
  REQUIRE(tokens[0].line == 1);
}
