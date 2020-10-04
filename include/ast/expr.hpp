#pragma once
#ifndef __WLP4_LLVM_AST_EXPR
#define __WLP4_LLVM_AST_EXPR

// STL
#include <memory>

// WLP4-LLVM
#include "astfwd.hpp"
#include "type.hpp"

namespace wlp4::ast {

class Expr {
public:
    enum class Op : unsigned char { NONE, PLUS, MINUS }; // FIXME: Reuse from Symbol?

    explicit Expr(TermPtr term);
    ~Expr();

    void setPlusWith(ExprPtr expr);
    void setMinusWith(ExprPtr expr);
    Expr::Op op() const;
    DclType type() const;
    const TermPtr& term() const;
    const ExprPtr& expr() const;

    llvm::Value* codegen();

private:
    TermPtr _term;
    Op _op;
    ExprPtr _leftExpr;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_EXPR
