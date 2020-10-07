// Self
#include "ast/arglist.hpp"

// WLP4-LLVM
#include "ast/expr.hpp"

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

std::size_t Arglist::numArgs() const {
    return 1 + (_arglist ? _arglist->numArgs() : 0);
}

} // namespace wlp4::ast
