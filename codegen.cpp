#include "ast/procedure.hpp"
#include "ast/dcl.hpp"
#include "ast/statement.hpp"
#include "ast/test.hpp"
#include "ast/expr.hpp"

namespace wlp4::ast {

llvm::Value* Procedure::codegen() {
    return nullptr;
}

llvm::Value* Dcl::codegen() {
    return nullptr;
}

llvm::Value* IfStatement::codegen() {
    return nullptr;
}

llvm::Value* Test::codegen() {
    return nullptr;
}

llvm::Value* Expr::codegen() {
    return nullptr;
}

} // namespace wlp4::ast
