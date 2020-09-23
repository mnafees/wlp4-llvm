// Self
#include "factor.hpp"

// WLP4-LLVM
#include "expr.hpp"
#include "lvalue.hpp"
#include "arglist.hpp"

namespace wlp4::ast {

Factor::Factor() {}

Factor::Factor(std::string id) :
    _value(std::move(id)),
    _parenExpr(false) {}

Factor::Factor(unsigned int num) :
    _value(num),
    _procedureCall(false),
    _parenExpr(false) {}

Factor::Factor(NullType nullType) :
    _value(nullType),
    _procedureCall(false),
    _parenExpr(false) {}

Factor::Factor(std::unique_ptr<Expr> expr) :
    _value(std::move(expr)),
    _procedureCall(false),
    _parenExpr(false) {}

Factor::Factor(std::unique_ptr<Lvalue> lvalue) :
    _value(std::move(lvalue)),
    _procedureCall(false),
    _parenExpr(false) {}

Factor::Factor(std::unique_ptr<Factor> factor) :
    _value(std::move(factor)),
    _procedureCall(false),
    _parenExpr(false) {}

Factor::Factor(std::unique_ptr<Arglist> arglist) :
    _value(std::move(arglist)),
    _procedureCall(false),
    _parenExpr(false) {}

Factor::~Factor() {}

void Factor::setProcedureCall(std::string name) {
    _procedureCall = true;
    _procedureName = std::move(name);
}

void Factor::setParenExpr() {
    _parenExpr = true;
}

void Factor::setNewIntExpr() {
    _parenExpr = false;
}

DclType Factor::type() const {
    if (std::holds_alternative<unsigned int>(_value)) {
        return DclType::INT;
    } else if (std::holds_alternative<NullType>(_value)) {
        return DclType::INT_STAR;
    } else if (std::holds_alternative<std::string>(_value)) {

    }
}

} // namespace wlp4::ast
