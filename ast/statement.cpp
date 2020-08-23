#include "statement.hpp"

namespace wlp4::ast {

void IfStatement::setTest(std::unique_ptr<Test> test) {
    _test = std::move(test);
}

void IfStatement::addTrueStatement(std::unique_ptr<Statement> stmt) {
    _trueStatements.push_back(std::move(stmt));
}

void IfStatement::addFalseStatement(std::unique_ptr<Statement> stmt) {
    _falseStatements.push_back(std::move(stmt));
}

} // namespace wlp4::ast
