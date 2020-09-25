// Self
#include "expr.hpp"

// WLP4-LLVM
#include "term.hpp"

namespace wlp4::ast {

Expr::Expr(std::unique_ptr<Term> term) :
    _term(std::move(term)),
    _op(Op::NONE),
    _leftExpr(nullptr) {}

Expr::~Expr() {}

void Expr::setPlusWith(std::unique_ptr<Expr> expr) {
    _leftExpr = std::move(expr);
    _op = Op::PLUS;
}

void Expr::setMinusWith(std::unique_ptr<Expr> expr) {
    _leftExpr = std::move(expr);
    _op = Op::MINUS;
}

DclType Expr::type() const {

}

} // namespace wlp4::ast
