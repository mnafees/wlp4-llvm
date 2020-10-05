#pragma once
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
    ExprPtr parseExpr();
    TermPtr parseTerm();
    FactorPtr parseFactor();
    LvaluePtr parseLvalue();
    ArglistPtr parseArglist();
    bool parseStatements();
    StatementPtr parseStatement();
    TestPtr parseTest();
    std::optional<Symbol> parseDcls();
    DclPtr parseDcl(Symbol *sym = nullptr);

    long _elemIdx;
    std::string _currProcedureName;
};

} // namespace wlp4


#endif // __WLP4_LLVM_PARSER
