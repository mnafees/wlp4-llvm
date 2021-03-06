// Self
#include "state.hpp"

// STL
#include <fstream>
#include <filesystem>
#include <cstdio>

// WLP4-LLVM
#include "token.hpp"
#include "ast/type.hpp"
#include "ast/dcl.hpp"
#include "embedded_wain.hpp"

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
#include "llvm/Config/llvm-config.h"

using namespace std::string_literals;
namespace fs = std::filesystem;

namespace wlp4 {

std::vector<std::pair<Symbol, std::vector<Symbol>>> CFG;

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

    _outFile = fs::current_path().append("a.out").string();
}

State& State::instance() {
    static State state;
    return state;
}

State& STATE = State::instance();

void State::initLLVMCodegen() {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    Builder = std::make_unique<llvm::IRBuilder<>>(TheContext);
    auto TargetTriple = llvm::sys::getDefaultTargetTriple();
    TheModule = std::make_unique<llvm::Module>(inputFilePath(), TheContext);
    TheModule->setSourceFileName(inputFilePath());
    std::string Error;
    auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);
    if (!Target) {
        throw std::runtime_error(Error);
    }
    auto CPU = "generic";
    auto Features = "";
    llvm::TargetOptions opt;
    TargetMachine = std::unique_ptr<llvm::TargetMachine>(Target->createTargetMachine(
        TargetTriple, CPU, Features, opt, llvm::Reloc::PIC_));
    TheModule->setDataLayout(TargetMachine->createDataLayout());
    TheModule->setTargetTriple(TargetTriple);
}

void dumpObjectFile() {
    std::error_code EC;
    llvm::raw_fd_ostream dest("prog.o", EC, llvm::sys::fs::OF_None);
    if (EC) {
        throw std::runtime_error("Could not open file: "s + EC.message());
    }

    llvm::legacy::PassManager pass;
#if LLVM_VERSION_MAJOR >= 10
    auto FileType = llvm::CodeGenFileType::CGFT_ObjectFile;
#else
    auto FileType = llvm::TargetMachine::CGFT_ObjectFile;
#endif

    if (STATE.TargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
        throw std::runtime_error("TargetMachine can't emit a file of this type"s);
    }

    pass.run(*(STATE.TheModule));
    dest.flush();
}

void State::compile() {
    // create temporary folder
    fs::path tmpDirPath(fs::temp_directory_path());
    tmpDirPath.append("wlp4_tmp"s);
    if (fs::exists(tmpDirPath)) {
        fs::remove_all(tmpDirPath);
    }
    if (fs::create_directory(tmpDirPath)) {
        fs::current_path(tmpDirPath);
        dumpObjectFile();
        std::ofstream ofs("main.c"s);
        if (ofs.is_open()) {
            if (_procedures.back()->params().at(0)->type() == ast::DclType::INT) {
                ofs << intWain;
            } else {
                ofs << arrayWain;
            }
            ofs.close();
            const auto compileCmd = _compiler + " main.c prog.o -o "s + _outFile;
            std::system(compileCmd.c_str());
        }
    } else {
        throw std::runtime_error("internal compiler error");
    }
}

void State::emitLLVM() {
    const auto llFilename = _inFile + ".ll";
    std::error_code EC;
    llvm::raw_fd_ostream dest(llFilename, EC, llvm::sys::fs::OF_None);
    if (EC) {
        throw std::runtime_error("Could not open file: "s + EC.message());
    }
    dest << *TheModule;
    dest.flush();
}

void State::setInputFilePath(const char* name) {
    _inFile = name;
}

const std::string& State::inputFilePath() const {
    return _inFile;
}

void State::setOutputFilePath(const char* name) {
    if (std::strcmp(name, "") != 0) {
        if (fs::path(name).is_absolute()) {
            _outFile = name;
        } else {
            _outFile = fs::current_path().append(name).string();
        }
    }
}

const std::string& State::outputFilePath() const {
    return _outFile;
}

void State::setCompilerPath(const char* name) {
    if (std::strcmp(name, "") == 0) {
        throw std::runtime_error("invalid compiler");
    }
    _compiler = name;
}

const std::string& State::compilerPath() const {
    return _compiler;
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
