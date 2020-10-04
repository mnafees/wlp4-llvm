// Self
#include "state.hpp"

#ifdef DEBUG
// STL
#include <iostream>
#endif

// WLP4-LLVM
#include "token.hpp"

// LLVM
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"

namespace wlp4 {

void State::initLLVMCodegen() {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    Builder = std::make_unique<llvm::IRBuilder<>>(TheContext);
    auto TargetTriple = llvm::sys::getDefaultTargetTriple();
    TheModule = std::make_unique<llvm::Module>(State::instance().filename(), TheContext);
    TheModule->setSourceFileName(State::instance().filename());
    std::string Error;
    auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);
    if (!Target) {
        throw std::runtime_error(Error);
    }
    auto CPU = "generic";
    auto Features = "";
    llvm::TargetOptions opt;
    auto RM = llvm::Optional<llvm::Reloc::Model>();
    TargetMachine = std::unique_ptr<llvm::TargetMachine>(Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM));
    TheModule->setDataLayout(TargetMachine->createDataLayout());
    TheModule->setTargetTriple(TargetTriple);

    // printf
    auto printfType = llvm::FunctionType::get(Builder->getInt32Ty(), {llvm::Type::getInt8PtrTy(TheContext)}, true);
    auto printfFunc = llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", TheModule.get());
    printfFunc->setDSOLocal(true);

    // malloc
    auto mallocType = llvm::FunctionType::get(Builder->getInt8PtrTy(), {llvm::Type::getInt32Ty(TheContext)}, false);
    auto mallocFunc = llvm::Function::Create(mallocType, llvm::Function::ExternalLinkage, "malloc", TheModule.get());
    mallocFunc->addAttribute(0, llvm::Attribute::NoAlias);
    mallocFunc->setDSOLocal(true);

    // free
    auto freeType = llvm::FunctionType::get(Builder->getVoidTy(), {llvm::Type::getInt8PtrTy(TheContext)}, false);
    auto freeFunc = llvm::Function::Create(freeType, llvm::Function::ExternalLinkage, "free", TheModule.get());
    freeFunc->setDSOLocal(true);
}

void State::dumpObjectFile() {
    using namespace std::string_literals;

    auto Filename = State::instance().filename() + ".o"s;
    std::error_code EC;
    llvm::raw_fd_ostream dest(Filename, EC, llvm::sys::fs::OF_None);

    if (EC) {
        throw std::runtime_error("Could not open file: "s + EC.message());
    }

    llvm::legacy::PassManager pass;
    auto FileType = llvm::TargetMachine::CGFT_ObjectFile;

    if (TargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
        throw std::runtime_error("TargetMachine can't emit a file of this type"s);
    }
    llvm::errs() << *TheModule;

    pass.run(*TheModule);
    dest.flush();
}

std::vector<std::pair<Symbol, std::vector<Symbol>>> CFG;
#ifdef DEBUG
std::map<Symbol, std::string> symToStr;
#endif

State::State()  {
    CFG.push_back({ Symbol::start, { Symbol::procedures } });
    CFG.push_back({ Symbol::procedures, { Symbol::procedure, Symbol::procedures } });
    CFG.push_back({ Symbol::procedures, { Symbol::main_s } });
    CFG.push_back({ Symbol::procedure, { Symbol::INT, Symbol::ID, Symbol::LPAREN, Symbol::params, Symbol::RPAREN,
        Symbol::LBRACE, Symbol::dcls, Symbol::statements, Symbol::RETURN, Symbol::expr, Symbol::SEMI,
        Symbol::RBRACE } });
    CFG.push_back({ Symbol::main_s, { Symbol::INT, Symbol::WAIN, Symbol::LPAREN, Symbol::dcl, Symbol::COMMA,
        Symbol::dcl, Symbol::RPAREN, Symbol::LBRACE, Symbol::dcls, Symbol::statements, Symbol::RETURN,
        Symbol::expr, Symbol::SEMI, Symbol::RBRACE } });
    CFG.push_back({ Symbol::params, {} });
    CFG.push_back({ Symbol::params, { Symbol::paramlist } });
    CFG.push_back({ Symbol::paramlist, { Symbol::dcl } });
    CFG.push_back({ Symbol::paramlist, { Symbol::dcl, Symbol::COMMA, Symbol::paramlist } });
    CFG.push_back({ Symbol::type, { Symbol::INT } });
    CFG.push_back({ Symbol::type, { Symbol::INT, Symbol::STAR } });
    CFG.push_back({ Symbol::dcls, {} });
    CFG.push_back({ Symbol::dcls, { Symbol::dcls, Symbol::dcl, Symbol::BECOMES, Symbol::NUM, Symbol::SEMI } });
    CFG.push_back({ Symbol::dcls, { Symbol::dcls, Symbol::dcl, Symbol::BECOMES, Symbol::NULL_S, Symbol::SEMI } });
    CFG.push_back({ Symbol::dcl, { Symbol::type, Symbol::ID } });
    CFG.push_back({ Symbol::statements, {} });
    CFG.push_back({ Symbol::statements, { Symbol::statements, Symbol::statement } });
    CFG.push_back({ Symbol::statement, { Symbol::lvalue, Symbol::BECOMES, Symbol::expr, Symbol::SEMI } });
    CFG.push_back({ Symbol::statement, { Symbol::IF, Symbol::LPAREN, Symbol::test, Symbol::RPAREN, Symbol::LBRACE,
        Symbol::statements, Symbol::RBRACE, Symbol::ELSE, Symbol::LBRACE, Symbol::statements,
        Symbol::RBRACE } });
    CFG.push_back({ Symbol::statement, { Symbol::WHILE, Symbol::LPAREN, Symbol::test, Symbol::RPAREN, Symbol::LBRACE,
        Symbol::statements, Symbol::RBRACE } });
    CFG.push_back({ Symbol::statement, { Symbol::PRINTLN, Symbol::LPAREN, Symbol::expr, Symbol::RPAREN, Symbol::SEMI } });
    CFG.push_back({ Symbol::statement, { Symbol::DELETE, Symbol::LBRACK, Symbol::RBRACK, Symbol::expr, Symbol::SEMI } });
    CFG.push_back({ Symbol::test, { Symbol::expr, Symbol::EQ, Symbol::expr } });
    CFG.push_back({ Symbol::test, { Symbol::expr, Symbol::NE, Symbol::expr } });
    CFG.push_back({ Symbol::test, { Symbol::expr, Symbol::LT, Symbol::expr } });
    CFG.push_back({ Symbol::test, { Symbol::expr, Symbol::LE, Symbol::expr } });
    CFG.push_back({ Symbol::test, { Symbol::expr, Symbol::GE, Symbol::expr } });
    CFG.push_back({ Symbol::test, { Symbol::expr, Symbol::GT, Symbol::expr } });
    CFG.push_back({ Symbol::expr, { Symbol::term } });
    CFG.push_back({ Symbol::expr, { Symbol::expr, Symbol::PLUS, Symbol::term } });
    CFG.push_back({ Symbol::expr, { Symbol::expr, Symbol::MINUS, Symbol::term } });
    CFG.push_back({ Symbol::term, { Symbol::factor } });
    CFG.push_back({ Symbol::term, { Symbol::term, Symbol::STAR, Symbol::factor } });
    CFG.push_back({ Symbol::term, { Symbol::term, Symbol::SLASH, Symbol::factor } });
    CFG.push_back({ Symbol::term, { Symbol::term, Symbol::PCT, Symbol::factor } });
    CFG.push_back({ Symbol::factor, { Symbol::ID } });
    CFG.push_back({ Symbol::factor, { Symbol::NUM } });
    CFG.push_back({ Symbol::factor, { Symbol::NULL_S } });
    CFG.push_back({ Symbol::factor, { Symbol::LPAREN, Symbol::expr, Symbol::RPAREN } });
    CFG.push_back({ Symbol::factor, { Symbol::AMP, Symbol::lvalue } });
    CFG.push_back({ Symbol::factor, { Symbol::STAR, Symbol::factor } });
    CFG.push_back({ Symbol::factor, { Symbol::NEW, Symbol::INT, Symbol::LBRACK, Symbol::expr, Symbol::RBRACK } });
    CFG.push_back({ Symbol::factor, { Symbol::ID, Symbol::LPAREN, Symbol::RPAREN } });
    CFG.push_back({ Symbol::factor, { Symbol::ID, Symbol::LPAREN, Symbol::arglist, Symbol::RPAREN } });
    CFG.push_back({ Symbol::arglist, { Symbol::expr } });
    CFG.push_back({ Symbol::arglist, { Symbol::expr, Symbol::COMMA, Symbol::arglist } });
    CFG.push_back({ Symbol::lvalue, { Symbol::ID } });
    CFG.push_back({ Symbol::lvalue, { Symbol::STAR, Symbol::factor } });
    CFG.push_back({ Symbol::lvalue, { Symbol::LPAREN, Symbol::lvalue, Symbol::RPAREN } });

#ifdef DEBUG
    std::map<Symbol, std::string> symToStrCopy = {
        { Symbol::ID, "ID" }, { Symbol::NUM, "NUM" }, { Symbol::LPAREN, "LPAREN" }, { Symbol::RPAREN, "RPAREN" },
        { Symbol::LBRACE, "LBRACE" }, { Symbol::RBRACE, "RBRACE" }, { Symbol::RETURN, "RETURN" }, { Symbol::IF, "IF" },
        { Symbol::ELSE, "ELSE" }, { Symbol::WHILE, "WHILE" }, { Symbol::PRINTLN, "PRINTLN" }, { Symbol::WAIN, "WAIN" },
        { Symbol::BECOMES, "BECOMES" }, { Symbol::INT, "INT" }, { Symbol::EQ, "EQ" }, { Symbol::NE, "NE" },
        { Symbol::LT, "LT" }, { Symbol::GT, "GT" }, { Symbol::LE, "LE" }, { Symbol::GE, "GE" }, { Symbol::PLUS, "PLUS" },
        { Symbol::MINUS, "MINUS" }, { Symbol::STAR, "STAR" }, { Symbol::SLASH, "SLASH" }, { Symbol::PCT, "PCT" },
        { Symbol::COMMA, "COMMA" }, { Symbol::SEMI, "SEMI" }, { Symbol::NEW, "NEW" }, { Symbol::DELETE, "DELETE" },
        { Symbol::LBRACK, "LBRACK" }, { Symbol::RBRACK, "RBRACK" }, { Symbol::AMP, "AMP" }, { Symbol::NULL_S, "NULL" },
        { Symbol::procedures, "procedures" }, { Symbol::procedure, "procedure" }, { Symbol::main_s, "main" },
        { Symbol::params, "params" }, { Symbol::paramlist, "paramlist" }, { Symbol::type, "type" }, { Symbol::dcl, "dcl" },
        { Symbol::dcls, "dcls" }, { Symbol::statements, "statements" }, { Symbol::lvalue, "lvalue" }, { Symbol::expr, "expr" },
        { Symbol::test, "test" }, { Symbol::statement, "statement" }, { Symbol::term, "term" }, { Symbol::factor, "factor" },
        { Symbol::start, "start" }, { Symbol::arglist, "arglist" }
    };
    symToStr = std::move(symToStrCopy);
#endif
}

State& State::instance() {
    static State state;
    return state;
}

void State::setFilename(const char* name) {
    _filename = name;
}

const std::string& State::filename() const {
    return _filename;
}

void State::addToken(Token token) {
    _tokens.push_back(token);
}

const Token& State::getToken(std::size_t idx) const {
    return _tokens[idx];
}

std::size_t State::numTokens() const {
    return _tokens.size();
}

void State::addToFinalChart(std::unique_ptr<Elem> elem) {
    const auto& rule = CFG[elem->ruleIdx()];
    if (rule.first == Symbol::type && rule.second.size() == 2) {
        // elem has the rule: type -> INT STAR
        // This means that we must have already added another elem with rule: type -> INT previously
        // We should get rid of this for sanity in the next steps
        _chart.pop_back();
    } else if (rule.first == Symbol::factor && rule.second.size() == 4) {
        // elem has the rule: type -> ID LPAREN arglist RPAREN
        // This means that we must have already added another elem with
        // rule: factor -> ID LPAREN arglist RPAREN previously
        // We should get rid of this for sanity in the next steps
        _chart.pop_back();
    }

    _chart.push_back(std::move(elem));
}

const std::vector<std::unique_ptr<Elem>>& State::finalChart() const {
    return _chart;
}

const std::list<std::unique_ptr<ast::Procedure>>& State::procedures() const {
    return _procedures;
}

void State::addProcedure(std::unique_ptr<ast::Procedure> proc) {
    _procedures.push_front(std::move(proc));
}

void State::addDclToProc(const std::string& procedureName, const std::string& dclName, ast::DclType dclType) {
    if (_dclsMap.find(procedureName) == _dclsMap.end()) {
        _dclsMap[procedureName] = std::map<std::string, ast::DclType>();
    }

    _dclsMap[procedureName][dclName] = dclType;
}

ast::DclType State::typeForDcl(const std::string& procedureName, const std::string& dclName) {
    return _dclsMap[procedureName][dclName];
}

} // namespace wlp4