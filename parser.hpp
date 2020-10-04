#ifndef __WLP4_LLVM_PARSER
#define __WLP4_LLVM_PARSER

// STL
#include <optional>

// WLP4-LLVM
#include "ast/astfwd.hpp"
#include "symbol.hpp"

namespace wlp4 {

class Parser {
public:
    void parse();

private:
    void gotoCorrectRule(Symbol lhs, const std::string& name);
    std::unique_ptr<ast::Expr> parseExpr();
    std::unique_ptr<ast::Term> parseTerm();
    std::unique_ptr<ast::Factor> parseFactor();
    std::unique_ptr<ast::Lvalue> parseLvalue();
    std::unique_ptr<ast::Arglist> parseArglist();
    bool parseStatements();
    std::unique_ptr<ast::Statement> parseStatement();
    std::unique_ptr<ast::Test> parseTest();
    std::optional<Symbol> parseDcls();
    std::unique_ptr<ast::Dcl> parseDcl(Symbol *sym = nullptr);

    long _elemIdx;
    std::string _currProcedureName;
};

} // namespace wlp4


#endif // __WLP4_LLVM_PARSER
