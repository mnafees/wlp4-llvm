#ifndef __WLP4_LLVM_AST_ARGLIST
#define __WLP4_LLVM_AST_ARGLIST

// STL
#include <memory>

// WLP4-LLVM
#include "astfwd.hpp"

namespace wlp4::ast {

class Arglist {
public:
    explicit Arglist(std::unique_ptr<Expr> expr);
    explicit Arglist(std::unique_ptr<Arglist> arglist);
    ~Arglist();

    void setExpr(std::unique_ptr<Expr> expr);
    const auto& expr() const;
    const auto& arglist() const;

    llvm::Value* codegen();

private:
    std::unique_ptr<Expr> _expr;
    std::unique_ptr<Arglist> _arglist;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_ARGLIST
