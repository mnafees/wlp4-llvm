#pragma once
#ifndef __WLP4_LLVM_AST_LVALUE
#define __WLP4_LLVM_AST_LVALUE

// STL
#include <memory>
#include <variant>

// WLP4-LLVM
#include "astfwd.hpp"
#include "type.hpp"

namespace wlp4::ast {

using LvalueType = std::variant<std::monostate,
                                std::string,
                                FactorPtr,
                                LvaluePtr>;

class Lvalue {
public:
    explicit Lvalue(std::string procedureName);
    ~Lvalue();

    void setValue(LvalueType value);

    DclType type() const;

    llvm::Value* codegen();

private:
    std::string _procedureName;
    LvalueType _value;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_LVALUE
