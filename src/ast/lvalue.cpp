// Self
#include "ast/lvalue.hpp"

// WLP4-LLVM
#include "ast/factor.hpp"
#include "state.hpp"

namespace wlp4::ast {

Lvalue::Lvalue(std::string procedureName) :
    _procedureName(std::move(procedureName)),
    _value(std::monostate()) {}

Lvalue::~Lvalue() {}

void Lvalue::setValue(LvalueType value) {
    _value = std::move(value);
}

DclType Lvalue::type() const {
    if (std::holds_alternative<std::string>(_value)) {
        // When lvalue derives ID, the derived ID must have a type, and the type of the
        // lvalue is the same as the type of the ID
        return STATE.typeForDcl(_procedureName, std::get<std::string>(_value));
    } else if (std::holds_alternative<LvaluePtr>(_value)) {
        // The type of an lvalue deriving LPAREN lvalue RPAREN is the same as the type
        // of the derived lvalue
        return std::get<LvaluePtr>(_value)->type();
    } else if (_value.index() == 0) {
        return DclType::INVALID;
    }

    // The type of a factor or lvalue deriving STAR factor is int. The type of the derived
    // factor (i.e. the one preceded by STAR) must be int*
    return DclType::INT;
}

} // namespace wlp4::ast
