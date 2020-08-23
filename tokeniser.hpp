#ifndef __WLP4_LLVM_TOKENISER
#define __WLP4_LLVM_TOKENISER

// STL
#include <string>
#include <vector>

namespace wlp4 {

enum Symbol {
    // terminal symbols
    ID = 0,
    NUM,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    RETURN,
    IF,
    ELSE,
    WHILE,
    PRINTLN,
    WAIN,
    BECOMES,
    INT,
    EQ,
    NE,
    LT,
    GT,
    LE,
    GE,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PCT,
    COMMA,
    SEMI,
    NEW,
    DELETE,
    LBRACK,
    RBRACK,
    AMP,
    NULL_S,

    // non-terminal symbols
    procedures,
    procedure,
    main,
    params,
    paramlist,
    type,
    dcl,
    dcls,
    statements,
    lvalue,
    expr,
    statement,
    test,
    term,
    factor,
    arglist,

    // dummy start state
    start
};

bool isTerminal(Symbol sym);

struct Token {
    Symbol type;
    int col;
    int line;
    std::string value;
};

class Tokeniser {
public:
    Tokeniser() : _currentTokenIdx(0) {}

    void scanFileForTokens(const char* filename);
    std::size_t size() const;
    const Token& get(std::size_t idx) const;

private:
    std::vector<Token> _symbols;
    size_t _currentTokenIdx;
};

} // namespace wlp4

#endif // __WLP4_LLVM_TOKENISER
