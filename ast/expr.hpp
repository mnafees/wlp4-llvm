#ifndef __WLP4_LLVM_AST_EXPR
#define __WLP4_LLVM_AST_EXPR

// STL
#include <memory>

// WLP4-LLVM
#include "base.hpp"
#include "term.hpp"

namespace wlp4::ast {

class Expr : public Base {
public:
    enum class Op : unsigned char { NONE, PLUS, MINUS }; // FIXME: Reuse from Symbol?

    explicit Expr(std::unique_ptr<Term> term) : _term(std::move(term)), _op(Op::NONE), _leftExpr(nullptr) {}
    ~Expr() = default;

    void setPlusWith(std::unique_ptr<Expr> expr) { _leftExpr = std::move(expr); _op = Op::PLUS; }
    void setMinusWith(std::unique_ptr<Expr> expr) { _leftExpr = std::move(expr); _op = Op::MINUS; }
    const auto& leftExpr() const { return _leftExpr; }
    Op op() const { return _op; }

    llvm::Value* codegen() override;

private:
    std::unique_ptr<Term> _term;
    Op _op;
    std::unique_ptr<Expr> _leftExpr;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_EXPR
