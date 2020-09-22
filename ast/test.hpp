#ifndef __WLP4_LLVM_AST_TEST
#define __WLP4_LLVM_AST_TEST

// WLP4-LLVM
#include "astfwd.hpp"
#include "symbol.hpp"

namespace wlp4::ast {

class Test {
public:
    void setType(Symbol type);
    Symbol type() const;
    void setLeftExpr(std::unique_ptr<Expr> expr);
    const std::unique_ptr<Expr>& leftExpr();
    void setRightExpr(std::unique_ptr<Expr> expr);
    const std::unique_ptr<Expr>& rightExpr();
    llvm::Value* codegen();

private:
    Symbol _type;
    std::unique_ptr<Expr> _leftExpr;
    std::unique_ptr<Expr> _rightExpr;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_TEST
