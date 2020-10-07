#pragma once
#ifndef __WLP4_LLVM_AST_PROCEDURE
#define __WLP4_LLVM_AST_PROCEDURE

// STL
#include <list>
#include <vector>

// WLP4-LLVM
#include "astfwd.hpp"

namespace wlp4::ast {

class Procedure {
public:
    explicit Procedure(std::string name);
    ~Procedure();

    llvm::Value* codegen();
    bool isWain() const;
    const std::string& name() const;
    void addParam(DclPtr dcl);
    std::size_t numParams() const;
    void addDeclaration(DclPtr dcl);
    void addStatement(StatementPtr stmt);
    const ExprPtr& returnExpr() const;
    void setReturnExpr(ExprPtr expr);
    const std::vector<DclPtr>& params() const;

private:
    void validateName(const std::string& name);

    std::string _name;
    std::vector<DclPtr> _params;
    std::vector<DclPtr> _dcls;
    std::list<StatementPtr> _stmts;
    ExprPtr _retExpr;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_PROCEDURE
