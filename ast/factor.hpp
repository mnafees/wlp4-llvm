#ifndef __WLP4_LLVM_AST_FACTOR
#define __WLP4_LLVM_AST_FACTOR

// STL
#include <memory>
#include <string>
#include <variant>

// WLP4-LLVM
#include "astfwd.hpp"

namespace wlp4::ast {

struct NullType {};

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
    explicit Factor(std::string id);
    explicit Factor(unsigned int num);
    explicit Factor(NullType nullType);
    explicit Factor(std::unique_ptr<Expr> expr);
    explicit Factor(std::unique_ptr<Lvalue> lvalue);
    explicit Factor(std::unique_ptr<Factor> factor);
    explicit Factor(std::unique_ptr<Arglist> arglist);
    ~Factor();

    llvm::Value* codegen();

private:
    FactorType _value;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_FACTOR
