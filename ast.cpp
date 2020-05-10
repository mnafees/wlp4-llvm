#include "ast/procedure.hpp"
#include "ast/dcl.hpp"

namespace wlp4::ast {

llvm::Value* Procedure::codegen() {
    return nullptr;
}

llvm::Value* Dcl::codegen() {
    return nullptr;
}

} // namespace wlp4::ast
