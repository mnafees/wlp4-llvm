#ifndef __WLP4_LLVM_AST_DCL
#define __WLP4_LLVM_AST_DCL

// STL
#include <string>

// LLVM
#include "llvm/IR/Value.h"

// WLP4-LLVM
#include "type.hpp"

namespace wlp4::ast {

class Dcl {
public:
    DclType type() const;
    void setType(DclType type);

    const std::string& id() const;
    void setId(std::string id);

    const auto& value() const;
    void setValue(std::string value);

    llvm::Value* codegen();

private:
    DclType _type;
    std::string _id;
    std::string _value;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_DCL
