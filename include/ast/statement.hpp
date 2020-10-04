#pragma once
#ifndef __WLP4_LLVM_AST_STATEMENT
#define __WLP4_LLVM_AST_STATEMENT

// STL
#include <list>

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
    void addTrueStatement(StatementPtr stmt);
    const std::list<StatementPtr>& trueStatements() { return _trueStatements; }
    void addFalseStatement(StatementPtr stmt);
    const std::list<StatementPtr>& falseStatements() { return _falseStatements; }
    llvm::Value* codegen() override;

private:
    TestPtr _test;
    std::list<StatementPtr> _trueStatements;
    std::list<StatementPtr> _falseStatements;
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
    void addStatement(StatementPtr stmt);
    llvm::Value* codegen() override;

private:
    TestPtr _test;
    std::list<StatementPtr> _stmts;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_STATEMENT
