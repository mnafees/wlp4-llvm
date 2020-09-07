#ifndef __WLP4_LLVM_AST_LVALUE
#define __WLP4_LLVM_AST_LVALUE

// STL
#include <memory>
#include <string>

namespace wlp4::ast {

union Lvalue {
    std::string id;
    std::unique_ptr<Factor> factor;
    std::unique_ptr<Lvalue> lvalue;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_LVALUE
