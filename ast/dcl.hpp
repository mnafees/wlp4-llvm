#ifndef __WLP4_LLVM_AST_DCL
#define __WLP4_LLVM_AST_DCL

// STL
#include <string>

// WLP4-LLVM
#include "astfwd.hpp"

namespace wlp4::ast {

enum class Type { INT, INT_STAR };

class Dcl {
public:
    auto type() const;
    void setType(Type type);

    const std::string& id() const;
    void setId(std::string id);

    const auto& value() const;
    void setValue(std::string value);

    llvm::Value* codegen();

private:
    Type _type;
    std::string _id;
    std::string _value;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_DCL
