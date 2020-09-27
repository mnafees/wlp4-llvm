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
// LLVM
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Target/TargetMachine.h"

namespace wlp4 {

static std::unique_ptr<llvm::Module> TheModule;
static llvm::LLVMContext TheContext;
static llvm::IRBuilder<> Builder(TheContext);
static std::unique_ptr<llvm::TargetMachine> TargetMachine;

// Context-free grammar taken from https://www.student.cs.uwaterloo.ca/~cs241/wlp4/WLP4.html
extern std::vector<std::pair<Symbol, std::vector<Symbol>>> CFG;
#ifdef DEBUG
extern std::map<Symbol, std::string> symToStr;
#endif

class State {
    State();
public:
    static void initLLVMCodegen();
    static void dumpObjectFile();

    State(const State&) = delete;
    State(State&&) = delete;
    State& operator=(const State&) = delete;
    State& operator=(State&&) = delete;

    static State& instance();

    void setFilename(const char* name);
    const std::string& filename() const;

    void addToken(Token token);
    const Token& getToken(std::size_t idx) const;
    std::size_t numTokens() const;

    void addToFinalChart(std::unique_ptr<Elem> elem);
    const std::vector<std::unique_ptr<Elem>>& finalChart() const;

    const std::vector<std::unique_ptr<ast::Procedure>>& procedures() const;
    void addProcedure(std::unique_ptr<ast::Procedure> proc);

    void addDclToProc(const std::string& procedureName, const std::string& dclName, ast::DclType dclType);
    ast::DclType typeForDcl(const std::string& procedureName, const std::string& dclName);

private:
    std::string _filename;
    std::vector<Token> _tokens;
    std::vector<std::unique_ptr<Elem>> _chart;
    std::vector<std::unique_ptr<ast::Procedure>> _procedures;
    std::map<std::string, std::map<std::string, ast::DclType>> _dclsMap;
};

} // namespace wlp4

#endif // __WLP4_LLVM_STATE
