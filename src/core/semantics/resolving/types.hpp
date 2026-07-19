#pragma once

#include "core/ast/types.hpp"
#include "core/semantics/type.hpp"

namespace tlc::core::semantics::resolving {

semantics::Type resolve_type(const ast::types::Type& type);

}
