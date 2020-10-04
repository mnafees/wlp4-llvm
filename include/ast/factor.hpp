#pragma once
#ifndef __WLP4_LLVM_AST_FACTOR
#define __WLP4_LLVM_AST_FACTOR

// STL
#include <memory>
#include <string>
#include <variant>

// WLP4-LLVM
#include "astfwd.hpp"
#include "type.hpp"

namespace wlp4::ast {

using FactorType = std::variant<std::monostate,
                                std::string,
                                unsigned int,
                                NullType,
                                ExprPtr,
                                LvaluePtr,
                                FactorPtr,
                                ArglistPtr>;

class Factor {
public:
    explicit Factor(std::string procedureName);
    ~Factor();

    void setValue(FactorType value);
    void setProcedureCall(std::string name);
    void setParenExpr();

    DclType type() const;

    llvm::Value* codegen();

private:
    FactorType _value;
    std::string _procedureName;
    std::string _callingProcedureName;
    bool _parenExpr;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_FACTOR
