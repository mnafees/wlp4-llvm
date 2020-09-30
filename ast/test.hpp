#ifndef __WLP4_LLVM_AST_TEST
#define __WLP4_LLVM_AST_TEST

// WLP4-LLVM
#include "astfwd.hpp"
#include "symbol.hpp"

namespace wlp4::ast {

class Test {
public:
    void setOp(Symbol op);
    void setLeftExpr(ExprPtr expr);
    void setRightExpr(ExprPtr expr);

    llvm::Value* codegen();

private:
    Symbol _op;
    ExprPtr _leftExpr;
    ExprPtr _rightExpr;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_TEST
