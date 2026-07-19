#include "types.hpp"

#include <variant>

#include "core/ast/types.hpp"
#include "core/semantics/type.hpp"
#include "util/overloaded.hpp"

namespace tlc::core::semantics::resolving {

semantics::Type resolve_type(const ast::types::Type& type) {
  return std::visit(overloaded{
    [](const ast::types::NamedType& named) {
      if (named.name == "i32") { return semantics::Type::I32; }
      if (named.name == "i64") { return semantics::Type::I64; }

      if (named.name == "string") { return semantics::Type::String; }
      if (named.name == "unit") { return semantics::Type::Unit; }

      return semantics::Type::Error;
    }
  }, type);
}

}
