// Self
#include "ast/procedure.hpp"

// WLP4-LLVM
#include "ast/dcl.hpp"
#include "ast/expr.hpp"
#include "ast/statement.hpp"

namespace wlp4::ast {

Procedure::Procedure(std::string name) :
    _name(std::move(name))
{}

Procedure::~Procedure() {}

bool Procedure::isWain() const {
    return _name == "wain";
}

const std::string& Procedure::name() const {
    return _name;
}

void Procedure::addParam(DclPtr dcl) {
    validateName(dcl->id());
    _params.push_back(std::move(dcl));
}

void Procedure::addDeclaration(DclPtr dcl) {
    validateName(dcl->id());
    _dcls.push_back(std::move(dcl));
}

void Procedure::addStatement(StatementPtr stmt) {
    _stmts.push_front(std::move(stmt));
}

void Procedure::setReturnExpr(ExprPtr expr) {
    _retExpr = std::move(expr);
}

void Procedure::validateName(const std::string& name) {
    for (const auto& d : _params) {
        if (d->id() == name) {
            throw std::runtime_error(name + " redeclared in procedure " + _name);
        }
    }
    for (const auto& d : _dcls) {
        if (d->id() == name) {
            throw std::runtime_error(name + " redeclared in procedure " + _name);
        }
    }
}

const std::vector<DclPtr>& Procedure::params() const {
    return _params;
}

} // namespace wlp4::ast
