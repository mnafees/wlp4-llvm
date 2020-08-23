#ifndef __WLP4_LLVM_AST_EXPR
#define __WLP4_LLVM_AST_EXPR

#include "base.hpp"

namespace wlp4::ast {

class Expr : public Base {
public:
    llvm::Value* codegen() override;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_EXPR
