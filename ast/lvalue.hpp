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
                                std::unique_ptr<Factor>,
                                std::unique_ptr<Lvalue>>;

class Lvalue {
public:
    Lvalue(std::string procedureName, std::string id);
    Lvalue(std::string procedureName, std::unique_ptr<Factor> factor);
    Lvalue(std::string procedureName, std::unique_ptr<Lvalue> lvalue);
    ~Lvalue();

    DclType type() const;

private:
    std::string _procedureName;
    LvalueType _value;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_LVALUE
