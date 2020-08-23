#ifndef __WLP4_LLVM_AST_STATEMENT
#define __WLP4_LLVM_AST_STATEMENT

#include "base.hpp"
#include "test.hpp"

#include <vector>

namespace wlp4::ast {

class Statement : public Base {
public:
    virtual llvm::Value* codegen() = 0;
};

class IfStatement : public Statement {
public:
    void setTest(std::unique_ptr<Test> test);
    std::unique_ptr<Test> test() { return std::move(_test); }
    void addTrueStatement(std::unique_ptr<Statement> stmt);
    const std::vector<std::unique_ptr<Statement>>& trueStatements() { return _trueStatements; }
    void addFalseStatement(std::unique_ptr<Statement> stmt);
    const std::vector<std::unique_ptr<Statement>>& falseStatements() { return _falseStatements; }
    llvm::Value* codegen() override;

private:
    std::unique_ptr<Test> _test;
    std::vector<std::unique_ptr<Statement>> _trueStatements;
    std::vector<std::unique_ptr<Statement>> _falseStatements;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_STATEMENT
