#include "procedure.hpp"

namespace wlp4::ast {

Procedure::Procedure(const std::string& name) :
    _name(name)
{}

bool Procedure::isWain() const {
    return _name == "wain";
}

const std::string& Procedure::name() const {
    return _name;
}

void Procedure::addParam(std::unique_ptr<Dcl>&& dcl) {
    validateVariable(dcl->id());
    _params.push_back(std::move(dcl));
}

void Procedure::addDeclaration(std::unique_ptr<Dcl>&& dcl) {
    validateVariable(dcl->id());
    _dcls.push_back(std::move(dcl));
}

void Procedure::validateVariable(const std::string& name) {
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
