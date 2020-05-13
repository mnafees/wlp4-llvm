#ifndef __WLP4_LLVM_AST_DCL
#define __WLP4_LLVM_AST_DCL

#include <string>

#include "base.hpp"

namespace wlp4::ast {

enum Type { INT, INT_STAR };

class Dcl : public Base {
public:
    llvm::Value* codegen() override;
    Type type() const;
    void setType(Type type);
    const std::string& id() const;
    void setId(const std::string& id);
    const std::string& value() const;
    void setValue(const std::string& value);

private:
    Type _type;
    std::string _id;
    std::string _value;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_DCL
