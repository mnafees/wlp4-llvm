// Self
#include "test.hpp"

// WLP4-LLVM
#include "expr.hpp"

namespace wlp4::ast {

void Test::setOp(Symbol op) {
    _op = op;
}

void Test::setLeftExpr(std::unique_ptr<Expr> expr) {
    _leftExpr = std::move(expr);
}

void Test::setRightExpr(std::unique_ptr<Expr> expr) {
    _rightExpr = std::move(expr);
}

} // namespace wlp4::ast
