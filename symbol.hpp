#ifndef __WLP4_LLVM_SYMBOL
#define __WLP4_LLVM_SYMBOL

namespace wlp4 {

enum class Symbol : unsigned char {
    // terminal symbols
    ID, NUM, LPAREN, RPAREN, LBRACE, RBRACE, RETURN, IF, ELSE,
    WHILE, PRINTLN, WAIN, BECOMES, INT, EQ, NE, LT, GT, LE, GE,
    PLUS, MINUS, STAR, SLASH, PCT, COMMA, SEMI, NEW, DELETE,
    LBRACK, RBRACK, AMP, NULL_S,

    // non-terminal symbols
    procedures, procedure, main_s, params, paramlist, type, dcl,
    dcls, statements, lvalue, expr, statement, test, term, factor,
    arglist,

    // dummy start state
    start
};

} // namespace wlp4

#endif // __WLP4_LLVM_SYMBOL
