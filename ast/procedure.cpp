// Self
#include "procedure.hpp"

// WLP4-LLVM
#include "dcl.hpp"
#include "expr.hpp"
#include "statement.hpp"

namespace wlp4::ast {

Procedure::Procedure(const std::string& name) :
    _name(name)
{}

Procedure::~Procedure() {}

bool Procedure::isWain() const {
    return _name == "wain";
}

const std::string& Procedure::name() const {
    return _name;
}

void Procedure::addParam(std::unique_ptr<Dcl> dcl) {
    validateName(dcl->id());
    _params.push_back(std::move(dcl));
}

void Procedure::addDeclaration(std::unique_ptr<Dcl> dcl) {
    validateName(dcl->id());
    _dcls.push_back(std::move(dcl));
}

void Procedure::addStatement(std::unique_ptr<Statement> stmt) {
    _stmts.push_back(std::move(stmt));
}

void Procedure::setReturnExpr(std::unique_ptr<Expr> expr) {
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

} // namespace wlp4::ast
