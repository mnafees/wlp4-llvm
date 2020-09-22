#ifndef __WLP4_LLVM_AST_LVALUE
#define __WLP4_LLVM_AST_LVALUE

// STL
#include <memory>
#include <variant>

// WLP4-LLVM
#include "astfwd.hpp"

namespace wlp4::ast {

using LvalueType = std::variant<std::monostate,
                                std::string,
                                std::unique_ptr<Factor>,
                                std::unique_ptr<Lvalue>>;

class Lvalue {
public:
    explicit Lvalue(std::string id);
    explicit Lvalue(std::unique_ptr<Factor> factor);
    explicit Lvalue(std::unique_ptr<Lvalue> lvalue);
    ~Lvalue();

private:
    LvalueType _value;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_LVALUE
