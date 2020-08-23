// Self
#include "tokeniser.hpp"

// STL
#include <exception>
#include <fstream>
#include <stdexcept>

namespace wlp4 {

bool isTerminal(Symbol sym) {
    return sym >= 0 && sym <= 32;
}

void Tokeniser::scanFileForTokens(const char* filename) {
    std::ifstream fs(filename);
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
                _symbols.push_back(Token{LPAREN, col, line, "("});
            } else if (ch == ')') {
                _symbols.push_back(Token{RPAREN, col, line, ")"});
            } else if (ch == '{') {
                _symbols.push_back(Token{LBRACE, col, line, "{"});
            } else if (ch == '}') {
                _symbols.push_back(Token{RBRACE, col, line, "}"});
            } else if (ch == '+') {
                _symbols.push_back(Token{PLUS, col, line, "+"});
            } else if (ch == '-') {
                _symbols.push_back(Token{MINUS, col, line, "-"});
            } else if (ch == '*') {
                _symbols.push_back(Token{STAR, col, line, "*"});
            } else if (ch == '%') {
                _symbols.push_back(Token{PCT, col, line, "%"});
            } else if (ch == ',') {
                _symbols.push_back(Token{COMMA, col, line, ","});
            } else if (ch == ';') {
                _symbols.push_back(Token{SEMI, col, line, ";"});
            } else if (ch == '[') {
                _symbols.push_back(Token{LBRACK, col, line, "["});
            } else if (ch == ']') {
                _symbols.push_back(Token{RBRACK, col, line, "]"});
            } else if (ch == '&') {
                _symbols.push_back(Token{AMP, col, line, "&"});
            } else if (ch == '/') {
                ch = fs.get();
                if (ch == '/') {
                    ch = fs.get();
                    while (ch != '\n' && !fs.eof()) {
                        ch = fs.get();
                    }
                    fs.unget();
                } else {
                    _symbols.push_back(Token{SLASH, col, line, "/"});
                    fs.unget();
                }
            } else if (ch == '=') {
                ch = fs.get();
                if (ch == '=') {
                    _symbols.push_back(Token{EQ, col, line, "=="});
                } else {
                    _symbols.push_back(Token{BECOMES, col, line, "="});
                    fs.unget();
                }
            } else if (ch == '!') {
                ch = fs.get();
                if (ch == '=') {
                    _symbols.push_back(Token{NE, col, line, "!="});
                } else {
                    throw std::logic_error("Invalid token"); // FIXME: More descriptive error message
                }
            } else if (ch == '<') {
                ch = fs.get();
                if (ch == '=') {
                    _symbols.push_back(Token{LE, col, line, "<="});
                } else {
                    _symbols.push_back(Token{LT, col, line, "<"});
                    fs.unget();
                }
            } else if (ch == '>') {
                ch = fs.get();
                if (ch == '=') {
                    _symbols.push_back(Token{GE, col, line, ">="});
                } else {
                    _symbols.push_back(Token{GT, col, line, ">"});
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
                    _symbols.push_back(Token{RETURN, col, line, constructedToken});
                } else if (constructedToken == "if") {
                    _symbols.push_back(Token{IF, col, line, constructedToken});
                } else if (constructedToken == "else") {
                    _symbols.push_back(Token{ELSE, col, line, constructedToken});
                } else if (constructedToken == "while") {
                    _symbols.push_back(Token{WHILE, col, line, constructedToken});
                } else if (constructedToken == "println") {
                    _symbols.push_back(Token{PRINTLN, col, line, constructedToken});
                } else if (constructedToken == "wain") {
                    _symbols.push_back(Token{WAIN, col, line, constructedToken});
                } else if (constructedToken == "int") {
                    _symbols.push_back(Token{INT, col, line, constructedToken});
                } else if (constructedToken == "new") {
                    _symbols.push_back(Token{NEW, col, line, constructedToken});
                } else if (constructedToken == "delete") {
                    _symbols.push_back(Token{DELETE, col, line, constructedToken});
                } else if (constructedToken == "NULL") {
                    _symbols.push_back(Token{NULL_S, col, line, constructedToken});
                } else {
                    _symbols.push_back(Token{ID, col, line, constructedToken});
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
                _symbols.push_back(Token{NUM, col, line, constructedToken});

                constructedToken.clear();
            } else {
                throw std::logic_error("Unknown token");
            }
            ch = fs.get();
            col++;
        }
    } else {
        throw std::runtime_error("Could not open source file: " + std::string(filename));
    }
}

std::size_t Tokeniser::size() const {
    return _symbols.size();
}

const Token& Tokeniser::get(std::size_t idx) const {
    return _symbols[idx];
}

} // namespace wlp4
