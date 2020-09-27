// STL
#include <system_error>

// WLP4-LLVM
#include "ast/procedure.hpp"
#include "ast/dcl.hpp"
#include "ast/statement.hpp"
#include "ast/test.hpp"
#include "ast/expr.hpp"
#include "ast/term.hpp"
#include "ast/factor.hpp"
#include "ast/lvalue.hpp"
#include "ast/arglist.hpp"
#include "state.hpp"

// LLVM
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Module.h"

static std::map<std::string, std::map<std::string, llvm::Value*>> dclSymbolTable;
static std::map<std::string, llvm::Function*> functions;

namespace wlp4::ast {

llvm::Value* Procedure::codegen() {
    dclSymbolTable[_name] = std::map<std::string, llvm::Value*>();
    std::vector<llvm::Type*> args;
    for (const auto& param : _params) {
        if (param->type() == DclType::INT) {
            args.push_back(Builder.getInt32Ty());
        } else if (param->type() == DclType::INT_STAR) {
            args.push_back(llvm::PointerType::getInt32PtrTy(TheContext));
        }
    }
    auto func = llvm::Function::Create(llvm::FunctionType::get(Builder.getInt32Ty(), args, false),
        llvm::Function::ExternalLinkage, _name, *TheModule
    );
    functions[_name] = func;
    auto entryBB = llvm::BasicBlock::Create(TheContext, "entry", func);
    Builder.SetInsertPoint(entryBB);
    for (const auto& dcl : _dcls) {
        auto dclCodegen = dcl->codegen();
        dclSymbolTable[_name][dcl->id()] = dclCodegen;
    }
    for (const auto& dcl : _dcls) {
        auto dclValue = dclSymbolTable[_name][dcl->id()];
        if (dcl->type() == DclType::INT && !dcl->value().empty()) {
            Builder.CreateStore(Builder.getInt32(std::stoi(dcl->value())), dclValue);
        }
    }
    for (const auto& stmt : _stmts) {
        Builder.SetInsertPoint(llvm::BasicBlock::Create(TheContext, "", func)); // FIXME: do we need this?
        stmt->codegen();
    }
    Builder.CreateRet(_retExpr->codegen());
    TheModule->getFunctionList().push_back(func);

    return func;
}

llvm::Value* Dcl::codegen() {
    if (_type == DclType::INT) {
        return Builder.CreateAlloca(Builder.getInt32Ty(), nullptr, _id);
    }
    return Builder.CreateAlloca(llvm::PointerType::getInt32PtrTy(TheContext), nullptr, _id);
}

llvm::Value* IfStatement::codegen() {
    auto testCodegen = _test->codegen();
    auto prevBB = Builder.GetInsertBlock();
    auto func = prevBB->getParent();
    auto trueBB = llvm::BasicBlock::Create(TheContext, "", func);
    Builder.SetInsertPoint(trueBB);
    for (const auto& stmt : _trueStatements) {
        stmt->codegen();
    }
    auto falseBB = llvm::BasicBlock::Create(TheContext, "", func);
    Builder.SetInsertPoint(falseBB);
    for (const auto& stmt : _falseStatements) {
        stmt->codegen();
    }
    Builder.SetInsertPoint(prevBB);

    return Builder.CreateCondBr(testCodegen, trueBB, falseBB);
}

llvm::Value* Lvalue::codegen() {
    if (std::holds_alternative<std::string>(_value)) {
        // lvalue -> ID
        return dclSymbolTable[_procedureName][std::get<std::string>(_value)];
    } else if (std::holds_alternative<std::unique_ptr<Factor>>(_value)) {
        // lvalue -> STAR factor
        return std::get<std::unique_ptr<Factor>>(_value)->codegen(); // FIXME: is this correct?
    } else if (std::holds_alternative<std::unique_ptr<Lvalue>>(_value)) {
        // lvalue -> LPAREN lvalue RPAREN
        return std::get<std::unique_ptr<Lvalue>>(_value)->codegen();
    }
    return nullptr;
}

llvm::Value* LvalueStatement::codegen() {
    return Builder.CreateStore(_expr->codegen(), _lvalue->codegen());
}

llvm::Value* WhileStatement::codegen() {
    return nullptr;
}

llvm::Value* PrintlnStatement::codegen() {
    return Builder.CreateCall(TheModule->getOrInsertFunction("printf",
        llvm::FunctionType::get(Builder.getInt32Ty(), {Builder.getInt8PtrTy()}, true)),
        {TheModule->getGlobalVariable("print_ph"), _expr->codegen()});
}

llvm::Value* DeleteStatement::codegen() {
    return nullptr;
}

llvm::Value* Test::codegen() {
    if (_op == Symbol::EQ) {
        return Builder.CreateICmpEQ(_leftExpr->codegen(), _rightExpr->codegen());
    } else if (_op == Symbol::NE) {
        return Builder.CreateICmpNE(_leftExpr->codegen(), _rightExpr->codegen());
    } else if (_op == Symbol::LT) {
        return Builder.CreateICmpSLT(_leftExpr->codegen(), _rightExpr->codegen());
    } else if (_op == Symbol::LE) {
        return Builder.CreateICmpSLE(_leftExpr->codegen(), _rightExpr->codegen());
    } else if (_op == Symbol::GE) {
        return Builder.CreateICmpSGE(_leftExpr->codegen(), _rightExpr->codegen());
    }
    return Builder.CreateICmpSGT(_leftExpr->codegen(), _rightExpr->codegen());
}

llvm::Value* Factor::codegen() {
    if (std::holds_alternative<std::string>(_value)) {
        // factor -> ID
        return Builder.CreateLoad(dclSymbolTable[_procedureName][std::get<std::string>(_value)]);
    } else if (std::holds_alternative<unsigned int>(_value)) {
        // factor -> NUM
        return Builder.getInt32(std::get<unsigned int>(_value));
    } else if (std::holds_alternative<NullType>(_value)) {
        // factor -> NULL
        return llvm::ConstantPointerNull::get(llvm::PointerType::get(Builder.getVoidTy(), 0));
    } else if (std::holds_alternative<std::unique_ptr<Expr>>(_value)) {
        if (_parenExpr) {
            // factor -> LPAREN expr RPAREN
            return Builder.CreateLoad(std::get<std::unique_ptr<Expr>>(_value)->codegen());
        } else {
            // factor -> NEW INT LBRACK expr RBRACK
            llvm::Type* argType;
            if constexpr (sizeof(void*) == 8) { // 64-bit system
                argType = Builder.getInt64Ty();
            } else { // 32-bit system
                argType = Builder.getInt32Ty();
            }
            return Builder.CreateCall(TheModule->getOrInsertFunction("malloc", llvm::FunctionType::get(
                Builder.getInt8PtrTy(), {argType}, false)));
        }
    } else if (std::holds_alternative<std::unique_ptr<Lvalue>>(_value)) {
        // factor -> AMP lvalue
        return Builder.CreateLoad(std::get<std::unique_ptr<Lvalue>>(_value)->codegen());
    } else if (std::holds_alternative<std::unique_ptr<Factor>>(_value)) {
        // factor -> STAR factor
        return Builder.CreateLoad(std::get<std::unique_ptr<Factor>>(_value)->codegen());
    } else if (std::holds_alternative<std::unique_ptr<Arglist>>(_value)) {
        // factor -> ID LPAREN arglist RPAREN
        return Builder.CreateCall(functions[_callingProcedureName], std::get<std::unique_ptr<Arglist>>(_value)->codegen());
    } else if (!_callingProcedureName.empty()) {
        // factor -> ID LPAREN RPAREN
        return Builder.CreateCall(functions[_callingProcedureName]);
    }
    return nullptr;
}

llvm::Value* Term::codegen() {
    auto factorCodegen = _factor->codegen();
    if (_op == Term::Op::NONE) {
        // term -> factor
        return factorCodegen;
    }
    auto leftTermCodegen = _leftTerm->codegen();
    if (_op == Term::Op::STAR) {
        // term -> term STAR factor
        return Builder.CreateMul(leftTermCodegen, factorCodegen);
    } else if (_op == Term::Op::SLASH) {
        // term -> term SLASH factor
        return Builder.CreateSDiv(leftTermCodegen, factorCodegen);
    }
    // term -> term PCT factor
    return Builder.CreateSRem(leftTermCodegen, factorCodegen);
}

llvm::Value* Expr::codegen() {
    auto termCodegen = _term->codegen();
    if (_op == Expr::Op::NONE) {
        // expr -> term
        return termCodegen;
    }
    auto leftExprCodegen = _leftExpr->codegen();
    if (_op == Expr::Op::MINUS) {
        // expr -> expr MINUS term
        return Builder.CreateSub(leftExprCodegen, termCodegen);
    }
    // expr -> expr PLUS term
    return Builder.CreateAdd(leftExprCodegen, termCodegen);
}

std::vector<llvm::Value*> Arglist::codegen() {
    std::vector<llvm::Value*> args;
    args.push_back(_expr->codegen()); // arglist -> expr
    if (_arglist) {
        // arglist -> expr COMMA arglist
        auto arglistCodegen = _arglist->codegen();
        args.insert(args.end(), arglistCodegen.begin(), arglistCodegen.end());
    }
    return args;
}

} // namespace wlp4::ast
