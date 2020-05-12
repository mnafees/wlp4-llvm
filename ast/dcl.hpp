#ifndef __WLP4_LLVM_AST_DCL
#define __WLP4_LLVM_AST_DCL

#include <string>

#include "base.hpp"

namespace wlp4::ast {

enum Type { INT, INT_STAR };

class Dcl : public Base {
public:
    Dcl() {}
    Dcl(Type type, const std::string& id) : _type(type), _id(id) {}

    llvm::Value* codegen() override;

    Type type() const {
        return _type;
    }

    void setType(Type type) {
        _type = type;
    }

    const std::string& id() const {
        return _id;
    }

    void setId(const std::string& id) {
        _id = id;
    }

    const std::string& value() const {
        return _value;
    }

    void setValue(const std::string& value) {
        _value = value;
    }

private:
    Type _type;
    std::string _id;
    std::string _value;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_DCL
