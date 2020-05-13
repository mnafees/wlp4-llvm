#include "parser.hpp"

#include <fstream>
#include <sstream>
#include <map>
#include <stdexcept>
#include <iostream>

namespace wlp4 {

Parser::Parser(std::unique_ptr<Tokeniser>& tokeniser) :
    _tokeniser(tokeniser)
{}

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
    _procedureNames.push_back(procedure->name());
    token = _tokeniser->nextToken();
    if (token.type != tok_lparen) {
        throwInvalidToken(token);
    }
    if (procedure->isWain()) {
        // We need two params
        parseDcl(procedure, true);
        token = _tokeniser->nextToken();
        if (token.type != tok_comma) {
            throwInvalidToken(token);
        }
        parseDcl(procedure, true);
        token = _tokeniser->nextToken();
        if (token.type != tok_rparen) {
            throwInvalidToken(token);
        }
    } else {
        // We can have zero or more params
        token = _tokeniser->nextToken();
        if (token.type != tok_rparen) {
            _tokeniser->backUpToken();
            while (true) {
                parseDcl(procedure, true);
                token = _tokeniser->nextToken();
                if (token.type == tok_rparen) {
                    break;
                } else if (token.type != tok_comma) {
                    throwInvalidToken(token);
                }
            }
        }
    }
    token = _tokeniser->nextToken();
    if (token.type != tok_lbrace) {
        throwInvalidToken(token);
    }

    token = _tokeniser->nextToken();
    while (token.type == tok_int) {
        // We can expect one or more declarations
        _tokeniser->backUpToken();
        parseDcl(procedure);
        token = _tokeniser->nextToken();
    }

    while (token.type != tok_return) {
        _tokeniser->backUpToken();
        parseStatement(procedure);
        token = _tokeniser->nextToken();
    }

    // if (token.type == tok_id || token.type == tok_if || token.type == tok_while ||
    //     token.type == tok_println || token.type == tok_delete) {
    //     // We can expect one or more statements
    //     _tokeniser->backUpToken();
    // } else {
    //     _tokeniser->backUpToken();
    // }

    token = _tokeniser->nextToken();
    if (token.type != tok_rbrace) {
        throwInvalidToken(token);
    }

    return procedure;
}

void Parser::parseDcl(std::unique_ptr<ast::Procedure>& procedure, bool isParam) {
    auto token = _tokeniser->nextToken();
    if (token.type != tok_int) {
        throwInvalidToken(token);
    }
    std::unique_ptr<ast::Dcl> dcl(new ast::Dcl);
    token = _tokeniser->nextToken();
    if (token.type == tok_id) {
        dcl->setType(ast::Type::INT);
        dcl->setId(token.value);
    } else if (token.type == tok_star) {
        token = _tokeniser->nextToken();
        if (token.type != tok_id) {
            throwInvalidToken(token);
        }
        dcl->setType(ast::Type::INT_STAR);
        dcl->setId(token.value);
    }

    if (!isParam) {
        token = _tokeniser->nextToken();
        if (token.type != tok_becomes) {
            throwInvalidToken(token);
        }
        token = _tokeniser->nextToken();
        if (dcl->type() == ast::Type::INT) {
            if (token.type != tok_num) {
                throwInvalidToken(token);
            }
            dcl->setValue(token.value);
        } else if (dcl->type() == ast::Type::INT_STAR) {
            if (token.type != tok_null) {
                throwInvalidToken(token);
            }
            dcl->setValue("NULL");
        }
        token = _tokeniser->nextToken();
        if (token.type != tok_semi) {
            throwInvalidToken(token);
        }
    }

    procedure->addDeclaration(std::move(dcl));
}

void Parser::parseStatement(std::unique_ptr<ast::Procedure>& procedure) {
    auto token = _tokeniser->nextToken();

}

} // namespace wlp4
