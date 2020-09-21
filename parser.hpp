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
};

} // namespace wlp4


#endif // __WLP4_LLVM_PARSER
