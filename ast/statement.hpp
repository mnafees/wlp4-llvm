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
    void setExpr(std::unique_ptr<Expr> expr);
    llvm::Value* codegen() override;

private:
    std::unique_ptr<Expr> _expr;
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

class LvalueStatement : public Statement {
public:
    void setLvalue(std::unique_ptr<Lvalue> lvalue);
    void setExpr(std::unique_ptr<Expr> expr);
    llvm::Value* codegen() override;

private:
    std::unique_ptr<Lvalue> _lvalue;
    std::unique_ptr<Expr> _expr;
};

class PrintlnStatement : public Statement {
public:
    void setExpr(std::unique_ptr<Expr> expr);
    llvm::Value* codegen() override;

private:
    std::unique_ptr<Expr> _expr;
};

class WhileStatement : public Statement {
public:
    void setTest(std::unique_ptr<Test> test);
    void setStatement(std::unique_ptr<Statement> stmts);
    llvm::Value* codegen() override;

private:
    std::unique_ptr<Test> _test;
    std::unique_ptr<Statement> _stmts;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_STATEMENT
