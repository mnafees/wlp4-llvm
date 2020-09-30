// Self
#include "arglist.hpp"

// WLP4-LLVM
#include "expr.hpp"

namespace wlp4::ast {

Arglist::Arglist(ExprPtr expr) :
    _expr(std::move(expr)),
    _arglist(nullptr) {}

Arglist::Arglist(ArglistPtr arglist) :
    _arglist(std::move(arglist)),
    _expr(nullptr) {}

Arglist::~Arglist() {}

void Arglist::setExpr(ExprPtr expr) {
    _expr = std::move(expr);
}

} // namespace wlp4::ast
