#ifndef __WLP4_LLVM_DEBUG
#define __WLP4_LLVM_DEBUG

#ifdef DEBUG

// STL
#include <map>

namespace wlp4 {

std::map<Symbol, std::string> symToStr = {
    { ID, "ID" },
    { NUM, "NUM" },
    { LPAREN, "LPAREN" },
    { RPAREN, "RPAREN" },
    { LBRACE, "LBRACE" },
    { RBRACE, "RBRACE" },
    { RETURN, "RETURN" },
    { IF, "IF" },
    { ELSE, "ELSE" },
    { WHILE, "WHILE" },
    { PRINTLN, "PRINTLN" },
    { WAIN, "WAIN" },
    { BECOMES, "BECOMES" },
    { INT, "INT" },
    { EQ, "EQ" },
    { NE, "NE" },
    { LT, "LT" },
    { GT, "GT" },
    { LE, "LE" },
    { GE, "GE" },
    { PLUS, "PLUS" },
    { MINUS, "MINUS" },
    { STAR, "STAR" },
    { SLASH, "SLASH" },
    { PCT, "PCT" },
    { COMMA, "COMMA" },
    { SEMI, "SEMI" },
    { NEW, "NEW" },
    { DELETE, "DELETE" },
    { LBRACK, "LBRACK" },
    { RBRACK, "RBRACK" },
    { AMP, "AMP" },
    { NULL_S, "NULL" },
    { procedures, "procedures" },
    { procedure, "procedure" },
    { main, "main" },
    { params, "params" },
    { paramlist, "paramlist" },
    { type, "type" },
    { dcl, "dcl" },
    { dcls, "dcls" },
    { statements, "statements" },
    { lvalue, "lvalue" },
    { expr, "expr" },
    { statement, "statement" },
    { test, "test" },
    { term, "term" },
    { factor, "factor" },
    { arglist, "arglist" },
    { start, "start" }
};

} // namespace wlp4

#endif

#endif // __WLP4_LLVM_DEBUG
