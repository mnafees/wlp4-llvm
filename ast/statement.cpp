// Self
#include "statement.hpp"

// WLP4-LLVM
#include "expr.hpp"
#include "lvalue.hpp"
#include "test.hpp"

namespace wlp4::ast {

void DeleteStatement::setExpr(ExprPtr expr) {
    _expr = std::move(expr);
}

void IfStatement::setTest(TestPtr test) {
    _test = std::move(test);
}

const TestPtr& IfStatement::test() {
    return _test;
}

void IfStatement::addTrueStatement(std::unique_ptr<Statement> stmt) {
    _trueStatements.push_back(std::move(stmt));
}

void IfStatement::addFalseStatement(std::unique_ptr<Statement> stmt) {
    _falseStatements.push_back(std::move(stmt));
}

LvalueStatement::~LvalueStatement() {}

void LvalueStatement::setLvalue(LvaluePtr lvalue) {
    _lvalue = std::move(lvalue);
}

void LvalueStatement::setExpr(ExprPtr expr) {
    _expr = std::move(expr);
}

void PrintlnStatement::setExpr(ExprPtr expr) {
    _expr = std::move(expr);
}

void WhileStatement::setTest(TestPtr test) {
    _test = std::move(test);
}

void WhileStatement::addStatement(std::unique_ptr<Statement> stmt) {
    _stmts.push_back(std::move(stmt));
}

} // namespace wlp4::ast
