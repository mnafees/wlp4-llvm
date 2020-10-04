// Self
#include "ast/test.hpp"

// WLP4-LLVM
#include "ast/expr.hpp"

namespace wlp4::ast {

void Test::setOp(Symbol op) {
    _op = op;
}

void Test::setLeftExpr(ExprPtr expr) {
    _leftExpr = std::move(expr);
}

void Test::setRightExpr(ExprPtr expr) {
    _rightExpr = std::move(expr);
}

} // namespace wlp4::ast
