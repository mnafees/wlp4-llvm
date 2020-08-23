#ifndef __WLP4_LLVM_AST_TEST
#define __WLP4_LLVM_AST_TEST

#include "base.hpp"
#include "expr.hpp"

namespace wlp4::ast {

using TestType = int; // reuse token enum values

class Test : public Base {
public:
    void setType(TestType type) { _type = type; }
    TestType type() const { return _type; }
    void setLeftExpr(std::unique_ptr<Expr> expr) { _leftExpr = std::move(expr); }
    std::unique_ptr<Expr> leftExpr() { return std::move(_leftExpr); }
    void setRightExpr(std::unique_ptr<Expr> expr) { _rightExpr = std::move(expr); }
    std::unique_ptr<Expr> rightExpr() { return std::move(_rightExpr); }
    llvm::Value* codegen() override;

private:
    TestType _type;
    std::unique_ptr<Expr> _leftExpr;
    std::unique_ptr<Expr> _rightExpr;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_TEST
