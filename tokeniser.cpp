#include "tokeniser.hpp"

#include <exception>
#include <fstream>
#include <stdexcept>

namespace wlp4 {

void Tokeniser::scanFileForTokens(const char* name) {
    std::ifstream fs(name);
    if (fs.is_open()) {
        unsigned char ch = fs.get();
        std::string constructedToken;
        while (!fs.eof()) {
            if (isspace(ch)) {
                ch = fs.get();
                continue;
            } else if (ch == '(') {
                _symbols.push_back(std::make_pair(tok_lparen, "("));
            } else if (ch == ')') {
                _symbols.push_back(std::make_pair(tok_rparen, ")"));
            } else if (ch == '{') {
                _symbols.push_back(std::make_pair(tok_lbrace, "{"));
            } else if (ch == '}') {
                _symbols.push_back(std::make_pair(tok_rbrace, "}"));
            } else if (ch == '+') {
                _symbols.push_back(std::make_pair(tok_plus, "+"));
            } else if (ch == '-') {
                _symbols.push_back(std::make_pair(tok_minus, "-"));
            } else if (ch == '*') {
                _symbols.push_back(std::make_pair(tok_star, "*"));
            } else if (ch == '%') {
                _symbols.push_back(std::make_pair(tok_pct, "%"));
            } else if (ch == ',') {
                _symbols.push_back(std::make_pair(tok_comma, ","));
            } else if (ch == ';') {
                _symbols.push_back(std::make_pair(tok_semi, ";"));
            } else if (ch == '[') {
                _symbols.push_back(std::make_pair(tok_lbrack, "["));
            } else if (ch == ']') {
                _symbols.push_back(std::make_pair(tok_rbrack, "]"));
            } else if (ch == '&') {
                _symbols.push_back(std::make_pair(tok_amp, "&"));
            } else if (ch == '/') {
                ch = fs.get();
                if (ch == '/') {
                    ch = fs.get();
                    std::string comment;
                    while (ch != '\n' && !fs.eof()) {
                        comment += ch;
                        ch = fs.get();
                    }
                    fs.unget();
                } else {
                    _symbols.push_back(std::make_pair(tok_slash, "/"));
                    fs.unget();
                }
            } else if (ch == '=') {
                ch = fs.get();
                if (ch == '=') {
                    _symbols.push_back(std::make_pair(tok_becomes, "=="));
                } else {
                    _symbols.push_back(std::make_pair(tok_becomes, "="));
                    fs.unget();
                }
            } else if (ch == '!') {
                ch = fs.get();
                if (ch == '=') {
                    _symbols.push_back(std::make_pair(tok_ne, "!="));
                } else {
                    throw std::logic_error("Invalid token");
                }
            } else if (ch == '<') {
                ch = fs.get();
                if (ch == '=') {
                    _symbols.push_back(std::make_pair(tok_le, "<="));
                } else {
                    _symbols.push_back(std::make_pair(tok_lt, "<"));
                    fs.unget();
                }
            } else if (ch == '>') {
                ch = fs.get();
                if (ch == '=') {
                    _symbols.push_back(std::make_pair(tok_ge, ">="));
                } else {
                    _symbols.push_back(std::make_pair(tok_gt, ">"));
                    fs.unget();
                }
            } else if (isalpha(ch)) {
                constructedToken += ch;
                ch = fs.get();
                while (!fs.eof() && !isspace(ch) && isalnum(ch)) {
                    constructedToken += ch;
                    ch = fs.get();
                }
                fs.unget();
                if (constructedToken == "return") {
                    _symbols.push_back(std::make_pair(tok_return, constructedToken));
                } else if (constructedToken == "if") {
                    _symbols.push_back(std::make_pair(tok_if, constructedToken));
                } else if (constructedToken == "else") {
                    _symbols.push_back(std::make_pair(tok_else, constructedToken));
                } else if (constructedToken == "while") {
                    _symbols.push_back(std::make_pair(tok_while, constructedToken));
                } else if (constructedToken == "println") {
                    _symbols.push_back(std::make_pair(tok_println, constructedToken));
                } else if (constructedToken == "wain") {
                    _symbols.push_back(std::make_pair(tok_wain, constructedToken));
                } else if (constructedToken == "int") {
                    _symbols.push_back(std::make_pair(tok_int, constructedToken));
                } else if (constructedToken == "new") {
                    _symbols.push_back(std::make_pair(tok_new, constructedToken));
                } else if (constructedToken == "delete") {
                    _symbols.push_back(std::make_pair(tok_delete, constructedToken));
                } else if (constructedToken == "NULL") {
                    _symbols.push_back(std::make_pair(tok_null, constructedToken));
                } else {
                    _symbols.push_back(std::make_pair(tok_id, constructedToken));
                }

                constructedToken.clear();
            } else if (isdigit(ch)) {
                constructedToken += ch;
                ch = fs.get();
                while (!fs.eof() && !isspace(ch) && isdigit(ch)) {
                    constructedToken += ch;
                    ch = fs.get();
                }
                fs.unget();
                if (constructedToken.length() > 1 && constructedToken[0] == '0') {
                    throw std::logic_error("Numbers cannot start with zero");
                }
                if (std::stol(constructedToken) > INT32_MAX) {
                    throw std::logic_error("Number value exceeds the maximum allowed limit");
                }
                _symbols.push_back(std::make_pair(tok_num, constructedToken));

                constructedToken.clear();
            } else {
                throw std::logic_error("Unknown token");
            }
            ch = fs.get();
        }
    } else {
        throw std::runtime_error("Could not open source file: " + std::string(name));
    }
}

void Tokeniser::resetTokenHead() {
    _currentTokenIdx = 0;
}

bool Tokeniser::hasNextToken() const {
    return !_symbols.empty() && _currentTokenIdx >= 0 && _currentTokenIdx < _symbols.size()-1;
}

const std::pair<Token, std::string>& Tokeniser::nextToken() {
    if (_currentTokenIdx >= _symbols.size()) {
        throw std::range_error("Trying to get token out of index");
    }
    return _symbols[_currentTokenIdx++];
}

} // namespace wlp4
