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
                                std::unique_ptr<Expr>,
                                std::unique_ptr<Lvalue>,
                                std::unique_ptr<Factor>,
                                std::unique_ptr<Arglist>>;

class Factor {
public:
    explicit Factor(std::string procedureName);
    Factor(std::string procedureName, std::string id);
    Factor(std::string procedureName, unsigned int num);
    Factor(std::string procedureName, NullType nullType);
    Factor(std::string procedureName, std::unique_ptr<Expr> expr);
    Factor(std::string procedureName, std::unique_ptr<Lvalue> lvalue);
    Factor(std::string procedureName, std::unique_ptr<Factor> factor);
    Factor(std::string procedureName, std::unique_ptr<Arglist> arglist);
    ~Factor();

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
