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

Arglist::~Arglist() {}

void Arglist::setExpr(std::unique_ptr<Expr> expr) {
    _expr = std::move(expr);
}

} // namespace wlp4::ast
