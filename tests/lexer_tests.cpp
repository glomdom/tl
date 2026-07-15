#include <catch2/catch_test_macros.hpp>

#include "core/token.hpp"
#include "core/diagnostics/diagnostics.hpp"
#include "core/lex/lexer.hpp"

using namespace tlc;

TEST_CASE("keywords lexed properly", "[lexer]") {
  core::diagnostics::Diagnostics diagnostics;
  core::lex::Lexer lexer{"func return", diagnostics};
  const std::vector<core::Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 3);
  REQUIRE(tokens[0].type == core::TokenType::Function);
  REQUIRE(tokens[1].type == core::TokenType::Return);
  REQUIRE(tokens[2].type == core::TokenType::EndOfFile);
}

TEST_CASE("single-char punctuation lexed properly", "[lexer]") {
  core::diagnostics::Diagnostics diagnostics;
  core::lex::Lexer lexer{"(){}:;,", diagnostics};
  const std::vector<core::Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 8);
  REQUIRE(tokens[0].type == core::TokenType::LeftParenthesis);
  REQUIRE(tokens[1].type == core::TokenType::RightParenthesis);
  REQUIRE(tokens[2].type == core::TokenType::LeftBrace);
  REQUIRE(tokens[3].type == core::TokenType::RightBrace);
  REQUIRE(tokens[4].type == core::TokenType::Colon);
  REQUIRE(tokens[5].type == core::TokenType::Semicolon);
  REQUIRE(tokens[6].type == core::TokenType::Comma);
  REQUIRE(tokens[7].type == core::TokenType::EndOfFile);
}

TEST_CASE("operators lexed properly", "[lexer]") {
  core::diagnostics::Diagnostics diagnostics;
  core::lex::Lexer lexer{"+-*/", diagnostics};
  const std::vector<core::Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 5);
  REQUIRE(tokens[0].type == core::TokenType::Plus);
  REQUIRE(tokens[1].type == core::TokenType::Minus);
  REQUIRE(tokens[2].type == core::TokenType::Star);
  REQUIRE(tokens[3].type == core::TokenType::Slash);
  REQUIRE(tokens[4].type == core::TokenType::EndOfFile);
}

TEST_CASE("right arrow not confused with minus", "[lexer]") {
  core::diagnostics::Diagnostics diagnostics;
  core::lex::Lexer lexer{"- ->", diagnostics};
  const std::vector<core::Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 3);
  REQUIRE(tokens[0].type == core::TokenType::Minus);
  REQUIRE(tokens[1].type == core::TokenType::RightArrow);
  REQUIRE(tokens[2].type == core::TokenType::EndOfFile);
}

TEST_CASE("identifiers lexed properly", "[lexer]") {
  core::diagnostics::Diagnostics diagnostics;
  core::lex::Lexer lexer{"foo _bar baz123", diagnostics};
  const std::vector<core::Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 4);
  REQUIRE(tokens[0].type == core::TokenType::Identifier);
  REQUIRE(tokens[1].type == core::TokenType::Identifier);
  REQUIRE(tokens[2].type == core::TokenType::Identifier);
  REQUIRE(tokens[3].type == core::TokenType::EndOfFile);
}

TEST_CASE("numbers lexed properly", "[lexer]") {
  core::diagnostics::Diagnostics diagnostics;
  core::lex::Lexer lexer{"0 42 123", diagnostics};
  const std::vector<core::Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 4);
  REQUIRE(tokens[0].type == core::TokenType::Number);
  REQUIRE(tokens[1].type == core::TokenType::Number);
  REQUIRE(tokens[2].type == core::TokenType::Number);
  REQUIRE(tokens[3].type == core::TokenType::EndOfFile);
}

TEST_CASE("strings lexed properly", "[lexer]") {
  core::diagnostics::Diagnostics diagnostics;
  core::lex::Lexer lexer{R"("hello" "world")", diagnostics};
  const std::vector<core::Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 3);
  REQUIRE(tokens[0].type == core::TokenType::String);
  REQUIRE(tokens[1].type == core::TokenType::String);
  REQUIRE(tokens[2].type == core::TokenType::EndOfFile);
}

TEST_CASE("unterminated string has error", "[lexer]") {
  core::diagnostics::Diagnostics diagnostics;
  core::lex::Lexer lexer{R"("hello)", diagnostics};
  const std::vector<core::Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 1);
  REQUIRE(diagnostics.has_error());
}

TEST_CASE("empty input yields only EndOfFile", "[lexer]") {
  core::diagnostics::Diagnostics diagnostics;
  core::lex::Lexer lexer{"", diagnostics};
  const std::vector<core::Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 1);
  REQUIRE(tokens[0].type == core::TokenType::EndOfFile);
}

TEST_CASE("function declaration lexed properly", "[lexer]") {
  core::diagnostics::Diagnostics diagnostics;
  core::lex::Lexer lexer{"func add(a: i32, b: i32) -> i32 { return a + b; }", diagnostics};
  const std::vector<core::Token> tokens = lexer.tokenize();

  const std::vector<core::TokenType> expected{
      core::TokenType::Function,         core::TokenType::Identifier,
      core::TokenType::LeftParenthesis,  core::TokenType::Identifier,
      core::TokenType::Colon,            core::TokenType::Identifier,
      core::TokenType::Comma,            core::TokenType::Identifier,
      core::TokenType::Colon,            core::TokenType::Identifier,
      core::TokenType::RightParenthesis, core::TokenType::RightArrow,
      core::TokenType::Identifier,       core::TokenType::LeftBrace,
      core::TokenType::Return,           core::TokenType::Identifier,
      core::TokenType::Plus,             core::TokenType::Identifier,
      core::TokenType::Semicolon,        core::TokenType::RightBrace,
      core::TokenType::EndOfFile,
  };

  REQUIRE(tokens.size() == expected.size());

  for (std::size_t i = 0; i < expected.size(); ++i) {
    REQUIRE(tokens[i].type == expected[i]);
  }
}

TEST_CASE("line numbers tracked across newlines", "[lexer]") {
  core::diagnostics::Diagnostics diagnostics;
  core::lex::Lexer lexer{"func\nreturn", diagnostics};
  const std::vector<core::Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 3);
  REQUIRE(tokens[0].line == 1);
  REQUIRE(tokens[1].line == 2);
}