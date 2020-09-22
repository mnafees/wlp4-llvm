// Self
#include "statement.hpp"

// WLP4-LLVM
#include "expr.hpp"
#include "lvalue.hpp"
#include "test.hpp"

namespace wlp4::ast {

void DeleteStatement::setExpr(std::unique_ptr<Expr> expr) {
    _expr = std::move(expr);
}

void IfStatement::setTest(std::unique_ptr<Test> test) {
    _test = std::move(test);
}

const std::unique_ptr<Test>& IfStatement::test() {
    return _test;
}

void IfStatement::addTrueStatement(std::unique_ptr<Statement> stmt) {
    _trueStatements.push_back(std::move(stmt));
}

void IfStatement::addFalseStatement(std::unique_ptr<Statement> stmt) {
    _falseStatements.push_back(std::move(stmt));
}

LvalueStatement::~LvalueStatement() {}

void LvalueStatement::setLvalue(std::unique_ptr<Lvalue> lvalue) {
    _lvalue = std::move(lvalue);
}

void LvalueStatement::setExpr(std::unique_ptr<Expr> expr) {
    _expr = std::move(expr);
}

void PrintlnStatement::setExpr(std::unique_ptr<Expr> expr) {
    _expr = std::move(expr);
}

void WhileStatement::setTest(std::unique_ptr<Test> test) {
    _test = std::move(test);
}

void WhileStatement::addStatement(std::unique_ptr<Statement> stmt) {
    _stmts.push_back(std::move(stmt));
}

} // namespace wlp4::ast
