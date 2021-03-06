// Self
#include "ast/statement.hpp"

// WLP4-LLVM
#include "ast/expr.hpp"
#include "ast/lvalue.hpp"
#include "ast/test.hpp"

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

void IfStatement::addTrueStatement(StatementPtr stmt) {
    _trueStatements.push_front(std::move(stmt));
}

void IfStatement::addFalseStatement(StatementPtr stmt) {
    _falseStatements.push_front(std::move(stmt));
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

void WhileStatement::addStatement(StatementPtr stmt) {
    _stmts.push_front(std::move(stmt));
}

} // namespace wlp4::ast
