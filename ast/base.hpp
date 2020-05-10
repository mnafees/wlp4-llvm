#ifndef __WLP4_LLVM_AST_BASE
#define __WLP4_LLVM_AST_BASE

#include <llvm/IR/Value.h>

namespace wlp4::ast {

class Base {
public:
    virtual ~Base() {}
    virtual llvm::Value* codegen() = 0;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_BASE
