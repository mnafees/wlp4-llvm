#pragma once
#ifndef __WLP4_LLVM_STATE
#define __WLP4_LLVM_STATE

// STL
#include <list>
#include <memory>
#include <vector>
#include <map>

// WLP4-LLVM
#include "elem.hpp"
#include "token.hpp"
#include "ast/type.hpp"
#include "ast/procedure.hpp"

// LLVM
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Target/TargetMachine.h"

namespace wlp4 {

// Context-free grammar taken from https://www.student.cs.uwaterloo.ca/~cs241/wlp4/WLP4.html
extern std::vector<std::pair<Symbol, std::vector<Symbol>>> CFG;

class State {
    State();
public:
    State(const State&) = delete;
    State(State&&) = delete;
    State& operator=(const State&) = delete;
    State& operator=(State&&) = delete;

    static State& instance();

    void initLLVMCodegen();
    void compile();
    void emitLLVM();

    void setInputFilePath(const char* name);
    const std::string& inputFilePath() const;

    void setOutputFilePath(const char* name);
    const std::string& outputFilePath() const;

    void setCompilerPath(const char* name);
    const std::string& compilerPath() const;

    void addToken(Token token);
    const Token& getToken(std::size_t idx) const;
    std::size_t numTokens() const;

    void addToFinalChart(std::unique_ptr<Elem> elem);
    const std::vector<std::unique_ptr<Elem>>& finalChart() const;

    const std::list<std::unique_ptr<ast::Procedure>>& procedures() const;
    void addProcedure(std::unique_ptr<ast::Procedure> proc);

    void addDclToProc(const std::string& procedureName, const std::string& dclName, ast::DclType dclType);
    ast::DclType typeForDcl(const std::string& procedureName, const std::string& dclName);

    llvm::LLVMContext TheContext;
    std::unique_ptr<llvm::Module> TheModule;
    std::unique_ptr<llvm::IRBuilder<>> Builder;
    std::unique_ptr<llvm::TargetMachine> TargetMachine;

private:
    std::string _inFile;
    std::string _outFile;
    std::string _compiler;
    std::vector<Token> _tokens;
    std::vector<std::unique_ptr<Elem>> _chart;
    std::list<std::unique_ptr<ast::Procedure>> _procedures;
    std::map<std::string, std::map<std::string, ast::DclType>> _dclsMap;
};

extern State& STATE;

} // namespace wlp4

#endif // __WLP4_LLVM_STATE
