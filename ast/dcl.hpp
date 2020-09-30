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
    // Returns the type of this Dcl
    DclType type() const;
    // Sets the type of this Dcl
    void setType(DclType type);

    // Returns a const-ref to the ID of this Dcl
    const std::string& id() const;
    // Sets the ID of this Dcl
    void setId(std::string id);

    // Returns a const-ref to the value of this Dcl
    const std::string& value() const;
    // Sets the value of this Dcl
    void setValue(std::string value);

    llvm::Value* codegen();

private:
    DclType _type;
    std::string _id;
    std::string _value;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_DCL
