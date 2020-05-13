#include "dcl.hpp"

namespace wlp4::ast {

Type Dcl::type() const {
    return _type;
}

void Dcl::setType(Type type) {
    _type = type;
}

const std::string& Dcl::id() const {
    return _id;
}

void Dcl::setId(const std::string& id) {
    _id = id;
}

const std::string& Dcl::value() const {
    return _value;
}

void Dcl::setValue(const std::string& value) {
    _value = value;
}

} // namespace wlp4::ast
