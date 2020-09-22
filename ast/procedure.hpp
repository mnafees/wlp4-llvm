#ifndef __WLP4_LLVM_AST_PROCEDURE
#define __WLP4_LLVM_AST_PROCEDURE

// STL
#include <memory>
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
    void addParam(std::unique_ptr<Dcl> dcl);
    void addDeclaration(std::unique_ptr<Dcl> dcl);
    void addStatement(std::unique_ptr<Statement> stmt);
    void setReturnExpr(std::unique_ptr<Expr> expr);

private:
    void validateName(const std::string& name);

    std::string _name;
    std::vector<std::unique_ptr<Dcl>> _params;
    std::vector<std::unique_ptr<Dcl>> _dcls;
    std::vector<std::unique_ptr<Statement>> _stmts;
    std::unique_ptr<Expr> _retExpr;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_PROCEDURE
