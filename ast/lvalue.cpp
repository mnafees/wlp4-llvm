// Self
#include "lvalue.hpp"

// WLP4-LLVM
#include "factor.hpp"

namespace wlp4::ast {

Lvalue::Lvalue(std::string id) :
    _value(std::move(id)) {}

Lvalue::Lvalue(std::unique_ptr<Factor> factor) :
    _value(std::move(factor)) {}

Lvalue::Lvalue(std::unique_ptr<Lvalue> lvalue) :
    _value(std::move(lvalue)) {}

Lvalue::~Lvalue() {}

} // namespace wlp4::ast
