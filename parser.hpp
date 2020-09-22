#ifndef __WLP4_LLVM_PARSER
#define __WLP4_LLVM_PARSER

// STL
#include <stack>

// WLP4-LLVM
#include "ast/astfwd.hpp"
#include "symbol.hpp"

namespace wlp4 {

class State;

class Parser {
public:
    explicit Parser(State& state);
    ~Parser() = default;

    void parse();

private:
    State& _state;
    std::size_t _elemIdx;
    std::stack<Symbol> _symbolStack;

    std::unique_ptr<ast::Expr> parseExpr();
    std::unique_ptr<ast::Term> parseTerm();
    std::unique_ptr<ast::Factor> parseFactor();
    std::unique_ptr<ast::Lvalue> parseLvalue();
    std::unique_ptr<ast::Arglist> parseArglist();
    bool parseStatements();
    std::unique_ptr<ast::Statement> parseStatement();
    std::unique_ptr<ast::Test> parseTest();
};

} // namespace wlp4


#endif // __WLP4_LLVM_PARSER
