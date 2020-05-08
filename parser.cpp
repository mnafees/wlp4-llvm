#include "parser.hpp"

#include <fstream>
#include <sstream>
#include <map>

namespace wlp4 {

Parser::Parser() :
    _startSym("procedures")
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

void Parser::createAST(Tokeniser* tokeniser) {
    if (_cfg.empty()) {
        constructCFGTable();
    }

    tokeniser->resetTokenHead();
    std::string sym = std::move(_startSym);
    while (tokeniser->hasNextToken()) {
        const auto& tokenPair = tokeniser->nextToken();

        auto it = _cfg.find(sym);
        for (; it != _cfg.end(); ++it) {

        }

        switch (tokenPair.first) {
            case tok_id: break;
            case tok_num: break;
            case tok_lparen: break;
            case tok_rparen: break;
            case tok_lbrace: break;
            case tok_rbrace: break;
            case tok_return: break;
            case tok_if: break;
            case tok_else: break;
            case tok_while: break;
            case tok_println: break;
            case tok_wain: break;
            case tok_becomes: break;
            case tok_int: break;
            case tok_eq: break;
            case tok_ne: break;
            case tok_lt: break;
            case tok_gt: break;
            case tok_le: break;
            case tok_ge: break;
            case tok_plus: break;
            case tok_minus: break;
            case tok_star: break;
            case tok_slash: break;
            case tok_pct: break;
            case tok_comma: break;
            case tok_semi: break;
            case tok_new: break;
            case tok_delete: break;
            case tok_lbrack: break;
            case tok_rbrack: break;
            case tok_amp: break;
            case tok_null: break;
        }
    }
}

void Parser::constructCFGTable() {
    std::ifstream fs("syntax.cfg");
    if (fs.is_open()) {
        std::string line;
        std::string first;
        std::string word;
        while (std::getline(fs, line)) {
            std::istringstream ss(line);
            ss >> first;
            std::vector<std::string> values;
            while (ss >> word) {
                values.push_back(word);
            }
            _cfg.insert(std::make_pair(first, std::move(values)));
        }
    } else {
        throw std::runtime_error("Could not open syntax.cfg");
    }
}

bool Parser::isTerminal(const std::string& sym) const {
    return isupper(sym[0]);
}

} // namespace wlp4
