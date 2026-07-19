#include <catch2/catch_test_macros.hpp>

#include "core/diagnostics/diagnostics.hpp"
#include "core/lex/lexer.hpp"
#include "core/lex/token.hpp"

using namespace tlc;

TEST_CASE("keywords lexed properly", "[lexer]") {
  core::diagnostics::Diagnostics diagnostics;
  core::lex::Lexer lexer{"func return", diagnostics};
  const std::vector<core::lex::Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 3);
  REQUIRE(tokens[0].type == core::lex::TokenType::Function);
  REQUIRE(tokens[1].type == core::lex::TokenType::Return);
  REQUIRE(tokens[2].type == core::lex::TokenType::EndOfFile);
}

TEST_CASE("single-char punctuation lexed properly", "[lexer]") {
  core::diagnostics::Diagnostics diagnostics;
  core::lex::Lexer lexer{"(){}:;,", diagnostics};
  const std::vector<core::lex::Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 8);
  REQUIRE(tokens[0].type == core::lex::TokenType::LeftParenthesis);
  REQUIRE(tokens[1].type == core::lex::TokenType::RightParenthesis);
  REQUIRE(tokens[2].type == core::lex::TokenType::LeftBrace);
  REQUIRE(tokens[3].type == core::lex::TokenType::RightBrace);
  REQUIRE(tokens[4].type == core::lex::TokenType::Colon);
  REQUIRE(tokens[5].type == core::lex::TokenType::Semicolon);
  REQUIRE(tokens[6].type == core::lex::TokenType::Comma);
  REQUIRE(tokens[7].type == core::lex::TokenType::EndOfFile);
}

TEST_CASE("operators lexed properly", "[lexer]") {
  core::diagnostics::Diagnostics diagnostics;
  core::lex::Lexer lexer{"+-*/", diagnostics};
  const std::vector<core::lex::Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 5);
  REQUIRE(tokens[0].type == core::lex::TokenType::Plus);
  REQUIRE(tokens[1].type == core::lex::TokenType::Minus);
  REQUIRE(tokens[2].type == core::lex::TokenType::Star);
  REQUIRE(tokens[3].type == core::lex::TokenType::Slash);
  REQUIRE(tokens[4].type == core::lex::TokenType::EndOfFile);
}

TEST_CASE("right arrow not confused with minus", "[lexer]") {
  core::diagnostics::Diagnostics diagnostics;
  core::lex::Lexer lexer{"- ->", diagnostics};
  const std::vector<core::lex::Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 3);
  REQUIRE(tokens[0].type == core::lex::TokenType::Minus);
  REQUIRE(tokens[1].type == core::lex::TokenType::RightArrow);
  REQUIRE(tokens[2].type == core::lex::TokenType::EndOfFile);
}

TEST_CASE("identifiers lexed properly", "[lexer]") {
  core::diagnostics::Diagnostics diagnostics;
  core::lex::Lexer lexer{"foo _bar baz123", diagnostics};
  const std::vector<core::lex::Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 4);
  REQUIRE(tokens[0].type == core::lex::TokenType::Identifier);
  REQUIRE(tokens[1].type == core::lex::TokenType::Identifier);
  REQUIRE(tokens[2].type == core::lex::TokenType::Identifier);
  REQUIRE(tokens[3].type == core::lex::TokenType::EndOfFile);
}

TEST_CASE("numbers lexed properly", "[lexer]") {
  core::diagnostics::Diagnostics diagnostics;
  core::lex::Lexer lexer{"0 42 123", diagnostics};
  const std::vector<core::lex::Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 4);
  REQUIRE(tokens[0].type == core::lex::TokenType::Number);
  REQUIRE(tokens[1].type == core::lex::TokenType::Number);
  REQUIRE(tokens[2].type == core::lex::TokenType::Number);
  REQUIRE(tokens[3].type == core::lex::TokenType::EndOfFile);
}

TEST_CASE("strings lexed properly", "[lexer]") {
  core::diagnostics::Diagnostics diagnostics;
  core::lex::Lexer lexer{R"("hello" "world")", diagnostics};
  const std::vector<core::lex::Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 3);
  REQUIRE(tokens[0].type == core::lex::TokenType::String);
  REQUIRE(tokens[1].type == core::lex::TokenType::String);
  REQUIRE(tokens[2].type == core::lex::TokenType::EndOfFile);
}

TEST_CASE("unterminated string has error", "[lexer]") {
  core::diagnostics::Diagnostics diagnostics;
  core::lex::Lexer lexer{R"("hello)", diagnostics};
  const std::vector<core::lex::Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 1);
  REQUIRE(diagnostics.has_error());
}

TEST_CASE("empty input yields only EndOfFile", "[lexer]") {
  core::diagnostics::Diagnostics diagnostics;
  core::lex::Lexer lexer{"", diagnostics};
  const std::vector<core::lex::Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 1);
  REQUIRE(tokens[0].type == core::lex::TokenType::EndOfFile);
}

TEST_CASE("function declaration lexed properly", "[lexer]") {
  core::diagnostics::Diagnostics diagnostics;
  core::lex::Lexer lexer{"func add(a: i32, b: i32) -> i32 { return a + b; }", diagnostics};
  const std::vector<core::lex::Token> tokens = lexer.tokenize();

  const std::vector<core::lex::TokenType> expected{
      core::lex::TokenType::Function,         core::lex::TokenType::Identifier,
      core::lex::TokenType::LeftParenthesis,  core::lex::TokenType::Identifier,
      core::lex::TokenType::Colon,            core::lex::TokenType::Identifier,
      core::lex::TokenType::Comma,            core::lex::TokenType::Identifier,
      core::lex::TokenType::Colon,            core::lex::TokenType::Identifier,
      core::lex::TokenType::RightParenthesis, core::lex::TokenType::RightArrow,
      core::lex::TokenType::Identifier,       core::lex::TokenType::LeftBrace,
      core::lex::TokenType::Return,           core::lex::TokenType::Identifier,
      core::lex::TokenType::Plus,             core::lex::TokenType::Identifier,
      core::lex::TokenType::Semicolon,        core::lex::TokenType::RightBrace,
      core::lex::TokenType::EndOfFile,
  };

  REQUIRE(tokens.size() == expected.size());

  for (std::size_t i = 0; i < expected.size(); ++i) {
    REQUIRE(tokens[i].type == expected[i]);
  }
}

TEST_CASE("line numbers tracked across newlines", "[lexer]") {
  core::diagnostics::Diagnostics diagnostics;
  core::lex::Lexer lexer{"func\nreturn", diagnostics};
  const std::vector<core::lex::Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 3);
  REQUIRE(tokens[0].line == 1);
  REQUIRE(tokens[1].line == 2);
}
