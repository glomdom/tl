#pragma once

#include <string>

namespace tlc::driver {

struct Options {
  bool dumpAst;
  bool dumpTokens;
  std::string inputFile;
};

} // namespace tlc::driver