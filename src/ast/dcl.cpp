// Self
#include "ast/dcl.hpp"

namespace wlp4::ast {

DclType Dcl::type() const {
    return _type;
}

void Dcl::setType(DclType type) {
    _type = type;
}

const std::string& Dcl::id() const {
    return _id;
}

void Dcl::setId(std::string id) {
    _id = std::move(id);
}

const std::string& Dcl::value() const {
    return _value;
}

void Dcl::setValue(std::string value) {
    _value = std::move(value);
}

} // namespace wlp4::ast
