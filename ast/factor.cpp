// Self
#include "factor.hpp"

// WLP4-LLVM
#include "expr.hpp"
#include "lvalue.hpp"
#include "arglist.hpp"

namespace wlp4::ast {

Factor::Factor(std::string id) :
    _value(std::move(id)) {}

Factor::Factor(unsigned int num) :
    _value(num) {}

Factor::Factor(NullType nullType) :
    _value(nullType) {}

Factor::Factor(std::unique_ptr<Expr> expr) :
    _value(std::move(expr)) {}

Factor::Factor(std::unique_ptr<Lvalue> lvalue) :
    _value(std::move(lvalue)) {}

Factor::Factor(std::unique_ptr<Factor> factor) :
    _value(std::move(factor)) {}

Factor::Factor(std::unique_ptr<Arglist> arglist) :
    _value(std::move(arglist)) {}

Factor::~Factor() {}

} // namespace wlp4::ast
