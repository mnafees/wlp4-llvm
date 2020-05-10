#include "parser.hpp"

#include <fstream>
#include <sstream>
#include <map>
#include <stdexcept>

namespace wlp4 {

Parser::Parser(Tokeniser *tokeniser) :
    _tokeniser(tokeniser)
{
    _tokenToTerminal[tok_id] = "ID";
    _tokenToTerminal[tok_num] = "NUM";
    _tokenToTerminal[tok_lparen] = "LPAREN";
    _tokenToTerminal[tok_rparen] = "RPAREN";
    _tokenToTerminal[tok_lbrace] = "LBRACE";
    _tokenToTerminal[tok_rbrace] = "RBRACE";
    _tokenToTerminal[tok_return] = "RETURN";
    _tokenToTerminal[tok_if] = "IF";
    _tokenToTerminal[tok_else] = "ELSE";
    _tokenToTerminal[tok_while] = "WHILE";
    _tokenToTerminal[tok_println] = "PRINTLN";
    _tokenToTerminal[tok_wain] = "WAIN";
    _tokenToTerminal[tok_becomes] = "BECOMES";
    _tokenToTerminal[tok_int] = "INT";
    _tokenToTerminal[tok_eq] = "EQ";
    _tokenToTerminal[tok_ne] = "NE";
    _tokenToTerminal[tok_lt] = "LT";
    _tokenToTerminal[tok_gt] = "GT";
    _tokenToTerminal[tok_le] = "LE";
    _tokenToTerminal[tok_ge] = "GE";
    _tokenToTerminal[tok_plus] = "PLUS";
    _tokenToTerminal[tok_minus] = "MINUS";
    _tokenToTerminal[tok_star] = "STAR";
    _tokenToTerminal[tok_slash] = "SLASH";
    _tokenToTerminal[tok_pct] = "PCT";
    _tokenToTerminal[tok_comma] = "COMMA";
    _tokenToTerminal[tok_semi] = "SEMI";
    _tokenToTerminal[tok_new] = "NEW";
    _tokenToTerminal[tok_delete] = "DELETE";
    _tokenToTerminal[tok_lbrack] = "LBRACK";
    _tokenToTerminal[tok_rbrack] = "RBRACK";
    _tokenToTerminal[tok_amp] = "AMP";
    _tokenToTerminal[tok_null] = "NULL";
}

void Parser::parseTokens(std::vector<std::unique_ptr<ast::Procedure>>& procedures) {
    _tokeniser->resetTokenHead();
    while (_tokeniser->hasNextToken()) {
        auto procedure = parseProcedure();
        procedures.push_back(std::move(procedure));
    }
}

void throwInvalidToken(const Token& token) {
    throw std::runtime_error("Parse error. Invalid token '" + token.value + "' at " + std::to_string(token.line) + ":" + std::to_string(token.col));
}

std::unique_ptr<ast::Procedure> Parser::parseProcedure() {
    auto token = _tokeniser->nextToken();
    if (token.type != tok_int) {
        throwInvalidToken(token);
    }
    token = _tokeniser->nextToken();
    if (token.type != tok_id && token.type != tok_wain) {
        throwInvalidToken(token);
    }
    std::unique_ptr<ast::Procedure> procedure(new ast::Procedure(token.value));
    if (std::find(_procedureNames.begin(), _procedureNames.end(), procedure->name()) != _procedureNames.end()) {
        throw std::runtime_error("Redeclaration of procedure: " + procedure->name());
    } else if (!procedure->isWain() && std::find(_procedureNames.begin(), _procedureNames.end(), "wain") !=
        _procedureNames.end()) {
        // wain should always be declared as the last function
        throw std::runtime_error("wain should always be the last declared procedure");
    }
    token = _tokeniser->nextToken();
    if (token.type != tok_lparen) {
        throwInvalidToken(token);
    }
    token = _tokeniser->nextToken();
    if (procedure->isWain()) {
        // We need two params
        _tokeniser->backupToken();
        auto dcl = parseDcl();
        procedure->addParam(std::move(dcl));
        token = _tokeniser->nextToken();
        if (token.type != tok_comma) {
            throwInvalidToken(token);
        }
        dcl = parseDcl();
        procedure->addParam(std::move(dcl));
    } else {
        // We can have zero or more params
    }
    token = _tokeniser->nextToken();
    if (token.type != tok_rparen) {
        throwInvalidToken(token);
    }
    token = _tokeniser->nextToken();
    if (token.type != tok_lbrace) {
        throwInvalidToken(token);
    }

    return procedure;
}

std::unique_ptr<ast::Dcl> Parser::parseDcl() {
    auto token = _tokeniser->nextToken();
    if (token.type != tok_int) {
        throwInvalidToken(token);
    }
    token = _tokeniser->nextToken();
    ast::Type type = ast::Type::INT;
    if (token.type == tok_star) {
        type = ast::Type::INT_STAR;
        token = _tokeniser->nextToken();
        if (token.type != tok_id) {
            throwInvalidToken(token);
        }
        return std::make_unique<ast::Dcl>(type, token.value);
    } else if (token.type == tok_id) {
        return std::make_unique<ast::Dcl>(type, token.value);
    }
    throwInvalidToken(token);
}

} // namespace wlp4
