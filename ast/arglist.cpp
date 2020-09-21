// Self
#include "arglist.hpp"

// WLP4-LLVM
#include "expr.hpp"

namespace wlp4::ast {

Arglist::Arglist(std::unique_ptr<Expr> expr) :
    _expr(std::move(expr)),
    _arglist(nullptr) {}

Arglist::Arglist(std::unique_ptr<Arglist> arglist) :
    _arglist(std::move(arglist)),
    _expr(nullptr) {}

void Arglist::setExpr(std::unique_ptr<Expr> expr) {
    _expr = std::move(expr);
}

const auto& Arglist::expr() const {
    return _expr;
}

const auto& Arglist::arglist() const {
    return _arglist;
}

} // namespace wlp4::ast
