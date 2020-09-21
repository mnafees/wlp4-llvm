// Self
#include "dcl.hpp"

namespace wlp4::ast {

auto Dcl::type() const {
    return _type;
}

void Dcl::setType(Type type) {
    _type = type;
}

const std::string& Dcl::id() const {
    return _id;
}

void Dcl::setId(std::string id) {
    _id = std::move(id);
}

const auto& Dcl::value() const {
    return _value;
}

void Dcl::setValue(std::string value) {
    _value = std::move(value);
}

} // namespace wlp4::ast
