#include "tokeniser.hpp"

#include <exception>
#include <fstream>
#include <stdexcept>
#include <iostream>

namespace wlp4 {

void Tokeniser::scanFileForTokens(const char* name) {
    std::ifstream fs(name);
    if (fs.is_open()) {
        unsigned char ch = fs.get();
        std::string constructedToken;
        int col = 1;
        int line = 1;
        while (!fs.eof()) {
            if (isspace(ch)) {
                if (ch == '\n') {
                    line++;
                    col = 1;
                }
                ch = fs.get();
                continue;
            } else if (ch == '(') {
                _symbols.push_back(Token{tok_lparen, col, line, "("});
            } else if (ch == ')') {
                _symbols.push_back(Token{tok_rparen, col, line, ")"});
            } else if (ch == '{') {
                _symbols.push_back(Token{tok_lbrace, col, line, "{"});
            } else if (ch == '}') {
                _symbols.push_back(Token{tok_rbrace, col, line, "}"});
            } else if (ch == '+') {
                _symbols.push_back(Token{tok_plus, col, line, "+"});
            } else if (ch == '-') {
                _symbols.push_back(Token{tok_minus, col, line, "-"});
            } else if (ch == '*') {
                _symbols.push_back(Token{tok_star, col, line, "*"});
            } else if (ch == '%') {
                _symbols.push_back(Token{tok_pct, col, line, "%"});
            } else if (ch == ',') {
                _symbols.push_back(Token{tok_comma, col, line, ","});
            } else if (ch == ';') {
                _symbols.push_back(Token{tok_semi, col, line, ";"});
            } else if (ch == '[') {
                _symbols.push_back(Token{tok_lbrack, col, line, "["});
            } else if (ch == ']') {
                _symbols.push_back(Token{tok_rbrack, col, line, "]"});
            } else if (ch == '&') {
                _symbols.push_back(Token{tok_amp, col, line, "&"});
            } else if (ch == '/') {
                ch = fs.get();
                if (ch == '/') {
                    ch = fs.get();
                    while (ch != '\n' && !fs.eof()) {
                        ch = fs.get();
                    }
                    fs.unget();
                } else {
                    _symbols.push_back(Token{tok_slash, col, line, "/"});
                    fs.unget();
                }
            } else if (ch == '=') {
                ch = fs.get();
                if (ch == '=') {
                    _symbols.push_back(Token{tok_eq, col, line, "=="});
                } else {
                    _symbols.push_back(Token{tok_becomes, col, line, "="});
                    fs.unget();
                }
            } else if (ch == '!') {
                ch = fs.get();
                if (ch == '=') {
                    _symbols.push_back(Token{tok_ne, col, line, "!="});
                } else {
                    throw std::logic_error("Invalid token"); // FIXME: More descriptive error message
                }
            } else if (ch == '<') {
                ch = fs.get();
                if (ch == '=') {
                    _symbols.push_back(Token{tok_le, col, line, "<="});
                } else {
                    _symbols.push_back(Token{tok_lt, col, line, "<"});
                    fs.unget();
                }
            } else if (ch == '>') {
                ch = fs.get();
                if (ch == '=') {
                    _symbols.push_back(Token{tok_ge, col, line, ">="});
                } else {
                    _symbols.push_back(Token{tok_gt, col, line, ">"});
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
                    _symbols.push_back(Token{tok_return, col, line, constructedToken});
                } else if (constructedToken == "if") {
                    _symbols.push_back(Token{tok_if, col, line, constructedToken});
                } else if (constructedToken == "else") {
                    _symbols.push_back(Token{tok_else, col, line, constructedToken});
                } else if (constructedToken == "while") {
                    _symbols.push_back(Token{tok_while, col, line, constructedToken});
                } else if (constructedToken == "println") {
                    _symbols.push_back(Token{tok_println, col, line, constructedToken});
                } else if (constructedToken == "wain") {
                    _symbols.push_back(Token{tok_wain, col, line, constructedToken});
                } else if (constructedToken == "int") {
                    _symbols.push_back(Token{tok_int, col, line, constructedToken});
                } else if (constructedToken == "new") {
                    _symbols.push_back(Token{tok_new, col, line, constructedToken});
                } else if (constructedToken == "delete") {
                    _symbols.push_back(Token{tok_delete, col, line, constructedToken});
                } else if (constructedToken == "NULL") {
                    _symbols.push_back(Token{tok_null, col, line, constructedToken});
                } else {
                    _symbols.push_back(Token{tok_id, col, line, constructedToken});
                }
                col += constructedToken.length();
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
                col += constructedToken.length();
                _symbols.push_back(Token{tok_num, col, line, constructedToken});

                constructedToken.clear();
            } else {
                throw std::logic_error("Unknown token");
            }
            ch = fs.get();
            col++;
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

const Token& Tokeniser::nextToken() {
    if (_currentTokenIdx >= _symbols.size()) {
        throw std::range_error("Trying to get token out of index");
    }
#ifdef DEBUG
    std::cout << "Next token: " << _symbols[_currentTokenIdx].value << std::endl;
#endif
    return _symbols[_currentTokenIdx++];
}

void Tokeniser::backupToken() {
    if (_currentTokenIdx == 0) {
        throw std::range_error("Cannot go backup a token");
    }
    _currentTokenIdx--;
}

} // namespace wlp4
