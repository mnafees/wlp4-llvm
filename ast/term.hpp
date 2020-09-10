#ifndef __WLP4_LLVM_AST_TERM
#define __WLP4_LLVM_AST_TERM

// STL
#include <memory>

// WLP4-LLVM
#include "base.hpp"

namespace wlp4::ast {

class Term : public Base {
public:
    llvm::Value* codegen() override;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_TERM
