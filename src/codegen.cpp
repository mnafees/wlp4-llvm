// STL
#include <iostream>

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
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"

static std::map<std::string, std::map<std::string, llvm::Value*>> dclSymbolTable;
static std::map<std::string, llvm::Function*> functions;

namespace wlp4::ast {

auto& Builder = State::instance().Builder;
auto& TheModule = State::instance().TheModule;

llvm::Value* Procedure::codegen() {
#ifdef DEBUG
    std::cout << "Codegen for procedure " << _name << '\n';
#endif

    dclSymbolTable[_name] = std::map<std::string, llvm::Value*>();
    std::vector<llvm::Type*> args;
    for (const auto& param : _params) {
        if (param->type() == DclType::INT) {
#ifdef DEBUG
            std::cout << _name << " has a parameter of type INT" << '\n';
#endif
            args.push_back(Builder->getInt32Ty());
        } else if (param->type() == DclType::INT_STAR) {
#ifdef DEBUG
            std::cout << _name << " has a parameter of type INT STAR" << '\n';
#endif
            args.push_back(llvm::PointerType::getInt32PtrTy(Builder->getContext()));
        }
    }
    auto func = llvm::Function::Create(llvm::FunctionType::get(Builder->getInt32Ty(), args, false),
        llvm::Function::ExternalLinkage, _name, TheModule.get()
    );
    func->setDSOLocal(true);
    std::size_t idx = 0;
    for (auto& arg : func->args()) {
        arg.setName(_params[idx++]->id());
    }
    functions[_name] = func;
    auto entryBB = llvm::BasicBlock::Create(Builder->getContext(), "entry", func);
    Builder->SetInsertPoint(entryBB);
    for (const auto& param : _params) {
        auto dclCodegen = param->codegen();
        dclSymbolTable[_name][param->id()] = dclCodegen;
    }
    for (const auto& dcl : _dcls) {
        auto dclCodegen = dcl->codegen();
        dclSymbolTable[_name][dcl->id()] = dclCodegen;
    }
    for (auto& arg : func->args()) {
        auto store = Builder->CreateStore(&arg, dclSymbolTable[_name][arg.getName()]);
        store->setAlignment(4);
    }
    for (const auto& dcl : _dcls) {
        auto dclValue = dclSymbolTable[_name][dcl->id()];
        llvm::StoreInst* store = nullptr;
        if (dcl->type() == DclType::INT) {
            store = Builder->CreateStore(Builder->getInt32(std::stoi(dcl->value())), dclValue);
        } else if (dcl->type() == DclType::INT_STAR) {
            store = Builder->CreateStore(llvm::ConstantPointerNull::get(
                llvm::PointerType::get(Builder->getInt32Ty(), 0)), dclValue);
        }
        store->setAlignment(4);
    }
    for (const auto& stmt : _stmts) {
        auto BB = llvm::BasicBlock::Create(Builder->getContext(), "", func);
        Builder->CreateBr(BB);
        Builder->SetInsertPoint(BB);
        stmt->codegen();
    }
    auto BB = llvm::BasicBlock::Create(Builder->getContext(), "", func);
    Builder->CreateBr(BB);
    Builder->SetInsertPoint(BB);
    Builder->CreateRet(_retExpr->codegen());

    return func;
}

llvm::Value* Dcl::codegen() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    llvm::AllocaInst* alloca = nullptr;
    if (_type == DclType::INT) {
        alloca = Builder->CreateAlloca(Builder->getInt32Ty());
    } else if (_type == DclType::INT_STAR) {
        alloca = Builder->CreateAlloca(llvm::PointerType::getInt32PtrTy(Builder->getContext()));
    }
    alloca->setAlignment(4);
    return alloca;
}

llvm::Value* IfStatement::codegen() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    auto func = Builder->GetInsertBlock()->getParent();
    auto BB = Builder->GetInsertBlock();

    auto testCodegen = _test->codegen();

    auto trueBB = llvm::BasicBlock::Create(Builder->getContext(), "", func);
    Builder->SetInsertPoint(trueBB);
    for (const auto& stmt : _trueStatements) {
        stmt->codegen();
        auto BB = llvm::BasicBlock::Create(Builder->getContext(), "", func);
        Builder->CreateBr(BB);
        Builder->SetInsertPoint(BB);
    }
    auto lastTrueBB = Builder->GetInsertBlock();

    auto falseBB = llvm::BasicBlock::Create(Builder->getContext(), "", func);
    Builder->SetInsertPoint(falseBB);
    for (const auto& stmt : _falseStatements) {
        stmt->codegen();
        auto BB = llvm::BasicBlock::Create(Builder->getContext(), "", func);
        Builder->CreateBr(BB);
        Builder->SetInsertPoint(BB);
    }
    auto lastFalseBB = Builder->GetInsertBlock();


    Builder->SetInsertPoint(BB);
    Builder->CreateCondBr(testCodegen, trueBB, falseBB);

    auto exitBB = llvm::BasicBlock::Create(Builder->getContext(), "", func);
    Builder->SetInsertPoint(lastTrueBB);
    Builder->CreateBr(exitBB);
    Builder->SetInsertPoint(lastFalseBB);
    Builder->CreateBr(exitBB);

    Builder->SetInsertPoint(exitBB);

    return nullptr;
}

llvm::Value* Lvalue::codegen() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    if (std::holds_alternative<std::string>(_value)) {
        // lvalue -> ID
        return dclSymbolTable[_procedureName][std::get<std::string>(_value)];
    } else if (std::holds_alternative<FactorPtr>(_value)) {
        // lvalue -> STAR factor
        return std::get<FactorPtr>(_value)->codegen(); // FIXME: is this correct?
    } else if (std::holds_alternative<LvaluePtr>(_value)) {
        // lvalue -> LPAREN lvalue RPAREN
        return std::get<LvaluePtr>(_value)->codegen();
    }
    return nullptr;
}

llvm::Value* LvalueStatement::codegen() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    auto store = Builder->CreateStore(_expr->codegen(), _lvalue->codegen());
    store->setAlignment(4);
    return store;
}

llvm::Value* WhileStatement::codegen() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    auto func = Builder->GetInsertBlock()->getParent();
    auto BB = Builder->GetInsertBlock();

    auto testCodegen = _test->codegen();

    auto whileBB = llvm::BasicBlock::Create(Builder->getContext(), "", func);
    Builder->SetInsertPoint(whileBB);
    for (const auto& stmt : _stmts) {
        stmt->codegen();
        auto BB = llvm::BasicBlock::Create(Builder->getContext(), "", func);
        Builder->CreateBr(BB);
        Builder->SetInsertPoint(BB);
    }
    Builder->CreateBr(BB);

    auto exitBB = llvm::BasicBlock::Create(Builder->getContext(), "", func);

    Builder->SetInsertPoint(BB);
    Builder->CreateCondBr(testCodegen, whileBB, exitBB);

    Builder->SetInsertPoint(exitBB);

    return nullptr;
}

llvm::Value* PrintlnStatement::codegen() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    auto printPhType = llvm::ArrayType::get(Builder->getInt8Ty(), 4);
    auto printPlaceholder = TheModule->getOrInsertGlobal("print_ph", printPhType,
        [=]() -> llvm::GlobalVariable* {
            auto ph = Builder->CreateGlobalString("%d\n", "print_ph");
            ph->setConstant(true);
            ph->setLinkage(llvm::GlobalVariable::PrivateLinkage);
            return ph;
        }
    );

    std::vector<llvm::Value*> args;
    std::vector<llvm::Constant*> gepArgs;
    if constexpr (sizeof(void*) == 8) {
        gepArgs.push_back(Builder->getInt64(0));
        gepArgs.push_back(Builder->getInt64(0));
    } else if (sizeof(void*) == 4) {
        gepArgs.push_back(Builder->getInt32(0));
        gepArgs.push_back(Builder->getInt32(0));
    }
    args.push_back(llvm::ConstantExpr::getInBoundsGetElementPtr(nullptr, printPlaceholder, gepArgs));
    args.push_back(_expr->codegen());
    return Builder->CreateCall(TheModule->getFunction("printf"), args);
}

llvm::Value* DeleteStatement::codegen() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    auto bitcast = Builder->CreateBitCast(_expr->codegen(), Builder->getInt8PtrTy());
    Builder->CreateCall(TheModule->getFunction("free"), {bitcast});

    return nullptr;
}

llvm::Value* Test::codegen() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    if (_op == Symbol::EQ) {
        return Builder->CreateICmpEQ(_leftExpr->codegen(), _rightExpr->codegen());
    } else if (_op == Symbol::NE) {
        return Builder->CreateICmpNE(_leftExpr->codegen(), _rightExpr->codegen());
    } else if (_op == Symbol::LT) {
        return Builder->CreateICmpSLT(_leftExpr->codegen(), _rightExpr->codegen());
    } else if (_op == Symbol::LE) {
        return Builder->CreateICmpSLE(_leftExpr->codegen(), _rightExpr->codegen());
    } else if (_op == Symbol::GE) {
        return Builder->CreateICmpSGE(_leftExpr->codegen(), _rightExpr->codegen());
    }
    return Builder->CreateICmpSGT(_leftExpr->codegen(), _rightExpr->codegen());
}

llvm::Value* Factor::codegen() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    if (std::holds_alternative<std::string>(_value)) {
        // factor -> ID
        return Builder->CreateLoad(dclSymbolTable[_procedureName][std::get<std::string>(_value)]);
    } else if (std::holds_alternative<unsigned int>(_value)) {
        // factor -> NUM
        return Builder->getInt32(std::get<unsigned int>(_value));
    } else if (std::holds_alternative<NullType>(_value)) {
        // factor -> NULL
        return llvm::ConstantPointerNull::get(llvm::PointerType::get(Builder->getInt32Ty(), 0));
    } else if (std::holds_alternative<ExprPtr>(_value)) {
        auto exprCodegen = std::get<ExprPtr>(_value)->codegen();
        if (_parenExpr) {
            // factor -> LPAREN expr RPAREN
            return exprCodegen;
        }
        // factor -> NEW INT LBRACK expr RBRACK
        auto mulInst = Builder->CreateMul(exprCodegen, llvm::ConstantInt::get(Builder->getInt32Ty(), sizeof(int)));
        auto mallocCall = Builder->CreateCall(TheModule->getFunction("malloc"), {mulInst});
        mallocCall->addAttribute(0, llvm::Attribute::NoAlias);
        return Builder->CreateBitCast(mallocCall, Builder->getInt32Ty()->getPointerTo());
    } else if (std::holds_alternative<LvaluePtr>(_value)) {
        // factor -> AMP lvalue
        return Builder->CreateLoad(std::get<LvaluePtr>(_value)->codegen());
    } else if (std::holds_alternative<FactorPtr>(_value)) {
        // factor -> STAR factor
        return Builder->CreateLoad(std::get<FactorPtr>(_value)->codegen());
    } else if (std::holds_alternative<ArglistPtr>(_value)) {
        // factor -> ID LPAREN arglist RPAREN
        return Builder->CreateCall(functions[_callingProcedureName], std::get<ArglistPtr>(_value)->codegen());
    } else if (!_callingProcedureName.empty()) {
        // factor -> ID LPAREN RPAREN
        return Builder->CreateCall(functions[_callingProcedureName]);
    }
    return nullptr;
}

llvm::Value* Term::codegen() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    auto factorCodegen = _factor->codegen();
    if (_op == Term::Op::NONE) {
        // term -> factor
        return factorCodegen;
    }
    auto leftTermCodegen = _leftTerm->codegen();
    if (_op == Term::Op::STAR) {
        // term -> term STAR factor
        return Builder->CreateMul(leftTermCodegen, factorCodegen);
    } else if (_op == Term::Op::SLASH) {
        // term -> term SLASH factor
        return Builder->CreateSDiv(leftTermCodegen, factorCodegen);
    }
    // term -> term PCT factor
    return Builder->CreateSRem(leftTermCodegen, factorCodegen);
}

llvm::Value* Expr::codegen() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    auto termCodegen = _term->codegen();
    if (_op == Expr::Op::NONE) {
        // expr -> term
        return termCodegen;
    }
    auto leftExprCodegen = _leftExpr->codegen();
    if (type() == DclType::INT_STAR) {
        if (_op == Expr::Op::PLUS) {
            // expr -> expr PLUS term
            if (_leftExpr->type() == DclType::INT) {
                return Builder->CreateInBoundsGEP(termCodegen, {leftExprCodegen});
            }
            return Builder->CreateInBoundsGEP(leftExprCodegen, {termCodegen});
        } else {
            // expr -> expr MINUS term
            auto sub = Builder->CreateSub(Builder->getInt32(0), termCodegen);
            return Builder->CreateInBoundsGEP(leftExprCodegen, {sub});
        }
    }
    if (_op == Expr::Op::MINUS) {
        // expr -> expr MINUS term
        if (_term->type() == DclType::INT_STAR && _leftExpr->type() == DclType::INT_STAR) {
            auto ptrToInt1 = Builder->CreatePtrToInt(termCodegen, Builder->getInt32Ty());
            auto ptrToInt2 = Builder->CreatePtrToInt(leftExprCodegen, Builder->getInt32Ty());
            return Builder->CreateExactSDiv(ptrToInt2, ptrToInt1);
        }
        return Builder->CreateSub(leftExprCodegen, termCodegen);
    }
    // expr -> expr PLUS term
    return Builder->CreateAdd(leftExprCodegen, termCodegen);
}

std::vector<llvm::Value*> Arglist::codegen() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

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
