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

    explicit Expr(std::unique_ptr<Term> term);
    ~Expr();

    void setPlusWith(std::unique_ptr<Expr> expr);

    void setMinusWith(std::unique_ptr<Expr> expr);

    DclType type() const;

    llvm::Value* codegen();

private:
    std::unique_ptr<Term> _term;
    Op _op;
    std::unique_ptr<Expr> _leftExpr;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_EXPR
