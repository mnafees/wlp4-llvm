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
    // When expr derives expr MINUS term
    if (_op == Op::MINUS) {
        if (_leftExpr->type() == DclType::INT && _term->type() == DclType::INT) {
            // The derived expr and the derived term may both have type int, in which case the
            // type of the expr deriving them is int
            return DclType::INT;
        } else if (_leftExpr->type() == DclType::INT_STAR && _term->type() == DclType::INT) {
            // The derived expr may have type int* and the derived term may have type int, in
            // which case the type of the expr deriving them is int*
            return DclType::INT_STAR;
        } else if (_leftExpr->type() == DclType::INT_STAR && _term->type() == DclType::INT_STAR) {
            // The derived expr and the derived term may both have type int*, in which case
            // the type of the expr deriving them is int
            return DclType::INT;
        } else {
            return DclType::INVALID;
        }
    } else if (_op == Op::PLUS) {
        // When expr derives expr PLUS term
        if (_leftExpr->type() == DclType::INT && _term->type() == DclType::INT) {
            // The derived expr and the derived term may both have type int, in which case
            // the type of the expr deriving them is int
            return DclType::INT;
        } else if ((_leftExpr->type() == DclType::INT_STAR && _term->type() == DclType::INT) ||
                   (_leftExpr->type() == DclType::INT && _term->type() == DclType::INT_STAR)) {
            // The derived expr may have type int* and the derived term may have type int, in
            // which case the type of the expr deriving them is int*
            // The derived expr may have type int and the derived term may have type int*, in
            // which case the type of the expr deriving them is int*
            return DclType::INT_STAR;
        } else {
            return DclType::INVALID;
        }
    }

    // The type of an expr deriving term is the same as the type of the derived term
    return _term->type();
}

} // namespace wlp4::ast
