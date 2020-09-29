#ifndef __WLP4_LLVM_PARSER
#define __WLP4_LLVM_PARSER

// STL
#include <stack>

// WLP4-LLVM
#include "ast/astfwd.hpp"
#include "symbol.hpp"

namespace wlp4 {

class Parser {
public:
    void parse();

private:
    std::unique_ptr<ast::Expr> parseExpr();
    std::unique_ptr<ast::Term> parseTerm();
    std::unique_ptr<ast::Factor> parseFactor();
    std::unique_ptr<ast::Lvalue> parseLvalue();
    std::unique_ptr<ast::Arglist> parseArglist();
    bool parseStatements();
    std::unique_ptr<ast::Statement> parseStatement();
    std::unique_ptr<ast::Test> parseTest();
    bool parseDcls();
    std::unique_ptr<ast::Dcl> parseDcl();

    long _elemIdx;
    std::stack<Symbol> _symbolStack;
    std::string _currProcedureName;
};

} // namespace wlp4


#endif // __WLP4_LLVM_PARSER
