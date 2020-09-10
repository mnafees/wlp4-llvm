#ifndef __WLP4_LLVM_AST_DCL
#define __WLP4_LLVM_AST_DCL

#include <string>

#include "base.hpp"

namespace wlp4::ast {

enum class Type { INT, INT_STAR };

class Dcl : public Base {
public:
    Dcl() = default;
    ~Dcl() = default;

    Type type() const;
    void setType(Type type);
    const std::string& id() const;
    void setId(const std::string& id);
    const std::string& value() const;
    void setValue(const std::string& value);
    llvm::Value* codegen() override;

private:
    Type _type;
    std::string _id;
    std::string _value;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_DCL
