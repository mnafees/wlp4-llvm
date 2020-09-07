#ifndef __WLP4_LLVM_AST_FACTOR
#define __WLP4_LLVM_AST_FACTOR

// STL
#include <memory>
#include <optional>
#include <string>
#include <variant>

namespace wlp4::ast {

class Expr;
class Lvalue;
class Arglist;

class Factor {
public:
    Factor() = default;
    ~Factor() = default;

    template<typename T>
    std::optional<T> get() const {
        if (std::holds_alternative<T>(_data)) {
            return std::get<T>(_data);
        }
        return std::nullopt;
    }

private:
    std::variant<std::monostate, std::string, unsigned int, void*, Expr, Lvalue, Factor> _data;
    std::optional<bool> _parenExpr;
    std::optional<bool> _newIntExpr;
    std::optional<Arglist> _arglist;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_FACTOR
