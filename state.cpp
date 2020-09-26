// Self
#include "state.hpp"

// WLP4-LLVM
#include "token.hpp"

namespace wlp4 {

std::vector<std::pair<Symbol, std::vector<Symbol>>> CFG;
#ifdef DEBUG
std::map<Symbol, std::string> symToStr;
#endif

State::State()  {
    CFG.push_back({ Symbol::start, { Symbol::procedures } });
    CFG.push_back({ Symbol::procedures, { Symbol::procedure, Symbol::procedures } });
    CFG.push_back({ Symbol::procedures, { Symbol::main_s } });
    CFG.push_back({ Symbol::procedure, { Symbol::INT, Symbol::ID, Symbol::LPAREN, Symbol::params, Symbol::RPAREN,
        Symbol::LBRACE, Symbol::dcls, Symbol::statements, Symbol::RETURN, Symbol::expr, Symbol::SEMI,
        Symbol::RBRACE } });
    CFG.push_back({ Symbol::main_s, { Symbol::INT, Symbol::WAIN, Symbol::LPAREN, Symbol::dcl, Symbol::COMMA,
        Symbol::dcl, Symbol::RPAREN, Symbol::LBRACE, Symbol::dcls, Symbol::statements, Symbol::RETURN,
        Symbol::expr, Symbol::SEMI, Symbol::RBRACE } });
    CFG.push_back({ Symbol::params, {} });
    CFG.push_back({ Symbol::params, { Symbol::paramlist } });
    CFG.push_back({ Symbol::paramlist, { Symbol::dcl } });
    CFG.push_back({ Symbol::paramlist, { Symbol::dcl, Symbol::COMMA, Symbol::paramlist } });
    CFG.push_back({ Symbol::type, { Symbol::INT } });
    CFG.push_back({ Symbol::type, { Symbol::INT, Symbol::STAR } });
    CFG.push_back({ Symbol::dcls, {} });
    CFG.push_back({ Symbol::dcls, { Symbol::dcls, Symbol::dcl, Symbol::BECOMES, Symbol::NUM, Symbol::SEMI } });
    CFG.push_back({ Symbol::dcls, { Symbol::dcls, Symbol::dcl, Symbol::BECOMES, Symbol::NULL_S, Symbol::SEMI } });
    CFG.push_back({ Symbol::dcl, { Symbol::type, Symbol::ID } });
    CFG.push_back({ Symbol::statements, {} });
    CFG.push_back({ Symbol::statements, { Symbol::statements, Symbol::statement } });
    CFG.push_back({ Symbol::statement, { Symbol::lvalue, Symbol::BECOMES, Symbol::expr, Symbol::SEMI } });
    CFG.push_back({ Symbol::statement, { Symbol::IF, Symbol::LPAREN, Symbol::test, Symbol::RPAREN, Symbol::LBRACE,
        Symbol::statements, Symbol::RBRACE, Symbol::ELSE, Symbol::LBRACE, Symbol::statements,
        Symbol::RBRACE } });
    CFG.push_back({ Symbol::statement, { Symbol::WHILE, Symbol::LPAREN, Symbol::test, Symbol::RPAREN, Symbol::LBRACE,
        Symbol::statements, Symbol::RBRACE } });
    CFG.push_back({ Symbol::statement, { Symbol::PRINTLN, Symbol::LPAREN, Symbol::expr, Symbol::RPAREN, Symbol::SEMI } });
    CFG.push_back({ Symbol::statement, { Symbol::DELETE, Symbol::LBRACK, Symbol::RBRACK, Symbol::expr, Symbol::SEMI } });
    CFG.push_back({ Symbol::test, { Symbol::expr, Symbol::EQ, Symbol::expr } });
    CFG.push_back({ Symbol::test, { Symbol::expr, Symbol::NE, Symbol::expr } });
    CFG.push_back({ Symbol::test, { Symbol::expr, Symbol::LT, Symbol::expr } });
    CFG.push_back({ Symbol::test, { Symbol::expr, Symbol::LE, Symbol::expr } });
    CFG.push_back({ Symbol::test, { Symbol::expr, Symbol::GE, Symbol::expr } });
    CFG.push_back({ Symbol::test, { Symbol::expr, Symbol::GT, Symbol::expr } });
    CFG.push_back({ Symbol::expr, { Symbol::term } });
    CFG.push_back({ Symbol::expr, { Symbol::expr, Symbol::PLUS, Symbol::term } });
    CFG.push_back({ Symbol::expr, { Symbol::expr, Symbol::MINUS, Symbol::term } });
    CFG.push_back({ Symbol::term, { Symbol::factor } });
    CFG.push_back({ Symbol::term, { Symbol::term, Symbol::STAR, Symbol::factor } });
    CFG.push_back({ Symbol::term, { Symbol::term, Symbol::SLASH, Symbol::factor } });
    CFG.push_back({ Symbol::term, { Symbol::term, Symbol::PCT, Symbol::factor } });
    CFG.push_back({ Symbol::factor, { Symbol::ID } });
    CFG.push_back({ Symbol::factor, { Symbol::NUM } });
    CFG.push_back({ Symbol::factor, { Symbol::NULL_S } });
    CFG.push_back({ Symbol::factor, { Symbol::LPAREN, Symbol::expr, Symbol::RPAREN } });
    CFG.push_back({ Symbol::factor, { Symbol::AMP, Symbol::lvalue } });
    CFG.push_back({ Symbol::factor, { Symbol::STAR, Symbol::factor } });
    CFG.push_back({ Symbol::factor, { Symbol::NEW, Symbol::INT, Symbol::LBRACK, Symbol::expr, Symbol::RBRACK } });
    CFG.push_back({ Symbol::factor, { Symbol::ID, Symbol::LPAREN, Symbol::RPAREN } });
    CFG.push_back({ Symbol::factor, { Symbol::ID, Symbol::LPAREN, Symbol::arglist, Symbol::RPAREN } });
    CFG.push_back({ Symbol::arglist, { Symbol::expr } });
    CFG.push_back({ Symbol::arglist, { Symbol::expr, Symbol::COMMA, Symbol::arglist } });
    CFG.push_back({ Symbol::lvalue, { Symbol::ID } });
    CFG.push_back({ Symbol::lvalue, { Symbol::STAR, Symbol::factor } });
    CFG.push_back({ Symbol::lvalue, { Symbol::LPAREN, Symbol::lvalue, Symbol::RPAREN } });

#ifdef DEBUG
    std::map<Symbol, std::string> symToStrCopy = {
        { Symbol::ID, "ID" }, { Symbol::NUM, "NUM" }, { Symbol::LPAREN, "LPAREN" }, { Symbol::RPAREN, "RPAREN" },
        { Symbol::LBRACE, "LBRACE" }, { Symbol::RBRACE, "RBRACE" }, { Symbol::RETURN, "RETURN" }, { Symbol::IF, "IF" },
        { Symbol::ELSE, "ELSE" }, { Symbol::WHILE, "WHILE" }, { Symbol::PRINTLN, "PRINTLN" }, { Symbol::WAIN, "WAIN" },
        { Symbol::BECOMES, "BECOMES" }, { Symbol::INT, "INT" }, { Symbol::EQ, "EQ" }, { Symbol::NE, "NE" },
        { Symbol::LT, "LT" }, { Symbol::GT, "GT" }, { Symbol::LE, "LE" }, { Symbol::GE, "GE" }, { Symbol::PLUS, "PLUS" },
        { Symbol::MINUS, "MINUS" }, { Symbol::STAR, "STAR" }, { Symbol::SLASH, "SLASH" }, { Symbol::PCT, "PCT" },
        { Symbol::COMMA, "COMMA" }, { Symbol::SEMI, "SEMI" }, { Symbol::NEW, "NEW" }, { Symbol::DELETE, "DELETE" },
        { Symbol::LBRACK, "LBRACK" }, { Symbol::RBRACK, "RBRACK" }, { Symbol::AMP, "AMP" }, { Symbol::NULL_S, "NULL" },
        { Symbol::procedures, "procedures" }, { Symbol::procedure, "procedure" }, { Symbol::main_s, "main" },
        { Symbol::params, "params" }, { Symbol::paramlist, "paramlist" }, { Symbol::type, "type" }, { Symbol::dcl, "dcl" },
        { Symbol::dcls, "dcls" }, { Symbol::statements, "statements" }, { Symbol::lvalue, "lvalue" }, { Symbol::expr, "expr" },
        { Symbol::test, "test" }, { Symbol::statement, "statement" }, { Symbol::term, "term" }, { Symbol::factor, "factor" },
        { Symbol::start, "start" }, { Symbol::arglist, "arglist" }
    };
    symToStr = symToStrCopy;
#endif
}

State& State::instance() {
    static State state;
    return state;
}

void State::setFilename(const char* name) {
    _filename = name;
}

const std::string& State::filename() const {
    return _filename;
}

void State::addToken(Token token) {
    _tokens.push_back(token);
}

const Token& State::getToken(std::size_t idx) const {
    return _tokens[idx];
}

std::size_t State::numTokens() const {
    return _tokens.size();
}

void State::addToFinalChart(std::unique_ptr<Elem> elem) {
    _chart.push_back(std::move(elem));
}

const std::vector<std::unique_ptr<Elem>>& State::finalChart() const {
    return _chart;
}

const std::vector<std::unique_ptr<ast::Procedure>>& State::procedures() const {
    return _procedures;
}

void State::addProcedure(std::unique_ptr<ast::Procedure> proc) {
    _procedures.push_back(std::move(proc));
}

void State::addDclToProc(const std::string& procedureName, const std::string& dclName, ast::DclType dclType) {
    if (_dclsMap.find(procedureName) == _dclsMap.end()) {
        _dclsMap[procedureName] = std::map<std::string, ast::DclType>();
    }

    _dclsMap[procedureName][dclName] = dclType;
}

ast::DclType State::typeForDcl(const std::string& procedureName, const std::string& dclName) {
    return _dclsMap[procedureName][dclName];
}

} // namespace wlp4
