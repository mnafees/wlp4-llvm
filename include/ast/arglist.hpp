#pragma once
#ifndef __WLP4_LLVM_AST_ARGLIST
#define __WLP4_LLVM_AST_ARGLIST

// STL
#include <vector>

// WLP4-LLVM
#include "astfwd.hpp"

namespace wlp4::ast {

class Arglist {
public:
    explicit Arglist(ExprPtr expr);
    explicit Arglist(ArglistPtr arglist);
    ~Arglist();

    void setExpr(ExprPtr expr);
    std::size_t numArgs() const;

    std::vector<llvm::Value*> codegen();

private:
    ExprPtr _expr;
    ArglistPtr _arglist;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_ARGLIST
