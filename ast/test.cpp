// Self
#include "test.hpp"

// WLP4-LLVM
#include "expr.hpp"

namespace wlp4::ast {

void Test::setType(Symbol type) {
    _type = type;
}

Symbol Test::type() const {
    return _type;
}

void Test::setLeftExpr(std::unique_ptr<Expr> expr) {
    _leftExpr = std::move(expr);
}

const std::unique_ptr<Expr>& Test::leftExpr() {
    return _leftExpr;
}

void Test::setRightExpr(std::unique_ptr<Expr> expr) {
    _rightExpr = std::move(expr);
}

const std::unique_ptr<Expr>& Test::rightExpr() {
    return _rightExpr;
}

} // namespace wlp4::ast
