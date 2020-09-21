#ifndef __WLP4_LLVM_AST_FACTOR
#define __WLP4_LLVM_AST_FACTOR

// STL
#include <memory>
#include <string>
#include <variant>

// WLP4-LLVM
#include "astfwd.hpp"

namespace wlp4::ast {

using FactorType = std::variant<std::monostate,
                                std::string,
                                unsigned int,
                                void*,
                                std::unique_ptr<Expr>,
                                std::unique_ptr<Lvalue>,
                                std::unique_ptr<Factor>,
                                std::unique_ptr<Arglist>>;

class Factor {
public:
    explicit Factor(std::string id);

    llvm::Value* codegen();

private:
    std::string _id;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_FACTOR
