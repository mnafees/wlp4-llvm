// WLP4-LLVM
#include "ast/procedure.hpp"
#include "ast/dcl.hpp"
#include "ast/statement.hpp"
#include "ast/test.hpp"
#include "ast/expr.hpp"
#include "ast/term.hpp"
#include "ast/factor.hpp"

// LLVM
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"

static llvm::LLVMContext TheContext;
static llvm::IRBuilder<> Builder(TheContext);

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

llvm::Value* LvalueStatement::codegen() {
    return nullptr;
}

llvm::Value* WhileStatement::codegen() {
    return nullptr;
}

llvm::Value* PrintlnStatement::codegen() {
    return nullptr;
}

llvm::Value* DeleteStatement::codegen() {
    return nullptr;
}

llvm::Value* Test::codegen() {
    return nullptr;
}

llvm::Value* Factor::codegen() {
    if (std::holds_alternative<std::string>(_value)) {

    } else if (std::holds_alternative<unsigned int>(_value)) {
        return Builder.getInt32(std::get<unsigned int>(_value));
    } else if (std::holds_alternative<NullType>(_value)) {
        return llvm::ConstantPointerNull::get(llvm::PointerType::get(Builder.getVoidTy(), 0));
    } else if (std::holds_alternative<std::unique_ptr<Expr>>(_value)) {
        if (_parenExpr) {

        } else {

        }
    }
}

llvm::Value* Term::codegen() {
    auto factorCodegen = _factor->codegen();
    if (_op == Term::Op::NONE) {
        return factorCodegen;
    }
    auto leftTermCodegen = _leftTerm->codegen();
    if (_op == Term::Op::STAR) {
        return Builder.CreateMul(leftTermCodegen, factorCodegen);
    } else if (_op == Term::Op::SLASH) {
        return Builder.CreateUDiv(leftTermCodegen, factorCodegen);
    }

    return Builder.CreateURem(leftTermCodegen, factorCodegen);
}

llvm::Value* Expr::codegen() {
    auto termCodegen = _term->codegen();
    if (_op == Expr::Op::NONE) {
        return termCodegen;
    }
    auto leftExprCodegen = _leftExpr->codegen();
    if (_op == Expr::Op::MINUS) {
        return Builder.CreateSub(leftExprCodegen, termCodegen);
    }

    return Builder.CreateAdd(leftExprCodegen, termCodegen);
}

} // namespace wlp4::ast
