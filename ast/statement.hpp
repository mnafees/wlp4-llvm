#ifndef __WLP4_LLVM_AST_STATEMENT
#define __WLP4_LLVM_AST_STATEMENT

// STL
#include <vector>

// WLP4-LLVM
#include "astfwd.hpp"

namespace wlp4::ast {

class Statement {
public:
    virtual ~Statement() {}
    virtual llvm::Value* codegen() = 0;
};

class DeleteStatement : public Statement {
public:
    void setExpr(ExprPtr expr);
    llvm::Value* codegen() override;

private:
    ExprPtr _expr;
};

class IfStatement : public Statement {
public:
    void setTest(TestPtr test);
    const TestPtr& test();
    void addTrueStatement(std::unique_ptr<Statement> stmt);
    const std::vector<std::unique_ptr<Statement>>& trueStatements() { return _trueStatements; }
    void addFalseStatement(std::unique_ptr<Statement> stmt);
    const std::vector<std::unique_ptr<Statement>>& falseStatements() { return _falseStatements; }
    llvm::Value* codegen() override;

private:
    TestPtr _test;
    std::vector<std::unique_ptr<Statement>> _trueStatements;
    std::vector<std::unique_ptr<Statement>> _falseStatements;
};

class LvalueStatement : public Statement {
public:
    ~LvalueStatement();

    void setLvalue(LvaluePtr lvalue);
    void setExpr(ExprPtr expr);
    llvm::Value* codegen() override;

private:
    LvaluePtr _lvalue;
    ExprPtr _expr;
};

class PrintlnStatement : public Statement {
public:
    void setExpr(ExprPtr expr);
    llvm::Value* codegen() override;

private:
    ExprPtr _expr;
};

class WhileStatement : public Statement {
public:
    void setTest(TestPtr test);
    void addStatement(std::unique_ptr<Statement> stmt);
    llvm::Value* codegen() override;

private:
    TestPtr _test;
    std::vector<std::unique_ptr<Statement>> _stmts;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_STATEMENT
