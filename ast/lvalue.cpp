// Self
#include "lvalue.hpp"

// WLP4-LLVM
#include "factor.hpp"
#include "state.hpp"

namespace wlp4::ast {

Lvalue::Lvalue(std::string procedureName, std::string id) :
    _procedureName(std::move(procedureName)),
    _value(std::move(id)) {}

Lvalue::Lvalue(std::string procedureName, std::unique_ptr<Factor> factor) :
    _procedureName(std::move(procedureName)),
    _value(std::move(factor)) {}

Lvalue::Lvalue(std::string procedureName, std::unique_ptr<Lvalue> lvalue) :
    _procedureName(std::move(procedureName)),
    _value(std::move(lvalue)) {}

Lvalue::~Lvalue() {}

DclType Lvalue::type() const {
    if (std::holds_alternative<std::string>(_value)) {
        // When lvalue derives ID, the derived ID must have a type, and the type of the
        // lvalue is the same as the type of the ID
        return State::instance().typeForDcl(_procedureName, std::get<std::string>(_value));
    } else if (std::holds_alternative<std::unique_ptr<Lvalue>>(_value)) {
        // The type of an lvalue deriving LPAREN lvalue RPAREN is the same as the type
        // of the derived lvalue
        return std::get<std::unique_ptr<Lvalue>>(_value)->type();
    }

    // The type of a factor or lvalue deriving STAR factor is int. The type of the derived
    // factor (i.e. the one preceded by STAR) must be int*
    return DclType::INT;
}

} // namespace wlp4::ast
