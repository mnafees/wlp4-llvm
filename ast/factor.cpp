// Self
#include "factor.hpp"

// WLP4-LLVM
#include "expr.hpp"
#include "lvalue.hpp"
#include "arglist.hpp"
#include "state.hpp"

namespace wlp4::ast {

Factor::Factor(std::string procedureName) :
    _procedureName(std::move(procedureName)) {}

Factor::Factor(std::string procedureName, std::string id) :
    _procedureName(std::move(procedureName)),
    _value(std::move(id)),
    _parenExpr(false) {}

Factor::Factor(std::string procedureName, unsigned int num) :
    _procedureName(std::move(procedureName)),
    _value(num),
    _parenExpr(false) {}

Factor::Factor(std::string procedureName, NullType nullType) :
    _procedureName(std::move(procedureName)),
    _value(nullType),
    _parenExpr(false) {}

Factor::Factor(std::string procedureName, std::unique_ptr<Expr> expr) :
    _procedureName(std::move(procedureName)),
    _value(std::move(expr)),
    _parenExpr(false) {}

Factor::Factor(std::string procedureName, std::unique_ptr<Lvalue> lvalue) :
    _procedureName(std::move(procedureName)),
    _value(std::move(lvalue)),
    _parenExpr(false) {}

Factor::Factor(std::string procedureName, std::unique_ptr<Factor> factor) :
    _procedureName(std::move(procedureName)),
    _value(std::move(factor)),
    _parenExpr(false) {}

Factor::Factor(std::string procedureName, std::unique_ptr<Arglist> arglist) :
    _procedureName(std::move(procedureName)),
    _value(std::move(arglist)),
    _parenExpr(false) {}

Factor::~Factor() {}

void Factor::setProcedureCall(std::string name) {
    _callingProcedureName = std::move(name);
}

void Factor::setParenExpr() {
    _parenExpr = true;
}

DclType Factor::type() const {
    if (std::holds_alternative<unsigned int>(_value)) {
        // The type of a factor deriving NUM or NULL is the same as the type of that token
        return DclType::INT;
    } else if (std::holds_alternative<NullType>(_value)) {
        // The type of a factor deriving NUM or NULL is the same as the type of that token
        return DclType::INT_STAR;
    } else if (std::holds_alternative<std::string>(_value)) {
        // When factor derives ID, the derived ID must have a type, and the type of the
        // factor is the same as the type of the ID
        return State::instance().typeForDcl(_procedureName, std::get<std::string>(_value));
    } else if (std::holds_alternative<std::unique_ptr<Expr>>(_value)) {
        if (_parenExpr) {
            // The type of a factor deriving LPAREN expr RPAREN is the same as the type of the expr
            return std::get<std::unique_ptr<Expr>>(_value)->type();
        } else {
            // The type of a factor deriving NEW INT LBRACK expr RBRACK is int*. The type
            // of the derived expr must be int
            return DclType::INT_STAR;
        }
    } else if (std::holds_alternative<std::unique_ptr<Lvalue>>(_value)) {
        // The type of a factor deriving AMP lvalue is int*. The type of the derived lvalue
        // (i.e. the one preceded by AMP) must be int
        return DclType::INT_STAR;
    } else if (std::holds_alternative<std::unique_ptr<Factor>>(_value)) {
        // The type of a factor or lvalue deriving STAR factor is int. The type of the derived
        // factor (i.e. the one preceded by STAR) must be int*
        return DclType::INT;
    } else if (std::holds_alternative<std::unique_ptr<Arglist>>(_value)) {
        // The type of a factor deriving ID LPAREN arglist RPAREN is int
        return DclType::INT;
    }

    // The type of a factor deriving ID LPAREN RPAREN is int. The procedure whose name is
    // ID must have an empty signature
    return DclType::INT;
}

} // namespace wlp4::ast
