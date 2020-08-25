// Self
#include "tokeniser.hpp"

// STL
#include <exception>
#include <fstream>
#include <stdexcept>

// WLP4-LLVM
#include "state.hpp"
#include "token.hpp"

namespace wlp4 {

bool isTerminal(Symbol sym) {
    return sym >= 0 && sym <= 32;
}

void Tokeniser::loadFile(const char* filename, State& state) {
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
                state.addToken(Token{LPAREN, col, line, "("});
            } else if (ch == ')') {
                state.addToken(Token{RPAREN, col, line, ")"});
            } else if (ch == '{') {
                state.addToken(Token{LBRACE, col, line, "{"});
            } else if (ch == '}') {
                state.addToken(Token{RBRACE, col, line, "}"});
            } else if (ch == '+') {
                state.addToken(Token{PLUS, col, line, "+"});
            } else if (ch == '-') {
                state.addToken(Token{MINUS, col, line, "-"});
            } else if (ch == '*') {
                state.addToken(Token{STAR, col, line, "*"});
            } else if (ch == '%') {
                state.addToken(Token{PCT, col, line, "%"});
            } else if (ch == ',') {
                state.addToken(Token{COMMA, col, line, ","});
            } else if (ch == ';') {
                state.addToken(Token{SEMI, col, line, ";"});
            } else if (ch == '[') {
                state.addToken(Token{LBRACK, col, line, "["});
            } else if (ch == ']') {
                state.addToken(Token{RBRACK, col, line, "]"});
            } else if (ch == '&') {
                state.addToken(Token{AMP, col, line, "&"});
            } else if (ch == '/') {
                ch = fs.get();
                if (ch == '/') {
                    ch = fs.get();
                    while (ch != '\n' && !fs.eof()) {
                        ch = fs.get();
                    }
                    fs.unget();
                } else {
                    state.addToken(Token{SLASH, col, line, "/"});
                    fs.unget();
                }
            } else if (ch == '=') {
                ch = fs.get();
                if (ch == '=') {
                    state.addToken(Token{EQ, col, line, "=="});
                } else {
                    state.addToken(Token{BECOMES, col, line, "="});
                    fs.unget();
                }
            } else if (ch == '!') {
                ch = fs.get();
                if (ch == '=') {
                    state.addToken(Token{NE, col, line, "!="});
                } else {
                    throw std::logic_error("Invalid token"); // FIXME: More descriptive error message
                }
            } else if (ch == '<') {
                ch = fs.get();
                if (ch == '=') {
                    state.addToken(Token{LE, col, line, "<="});
                } else {
                    state.addToken(Token{LT, col, line, "<"});
                    fs.unget();
                }
            } else if (ch == '>') {
                ch = fs.get();
                if (ch == '=') {
                    state.addToken(Token{GE, col, line, ">="});
                } else {
                    state.addToken(Token{GT, col, line, ">"});
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
                    state.addToken(Token{RETURN, col, line, constructedToken});
                } else if (constructedToken == "if") {
                    state.addToken(Token{IF, col, line, constructedToken});
                } else if (constructedToken == "else") {
                    state.addToken(Token{ELSE, col, line, constructedToken});
                } else if (constructedToken == "while") {
                    state.addToken(Token{WHILE, col, line, constructedToken});
                } else if (constructedToken == "println") {
                    state.addToken(Token{PRINTLN, col, line, constructedToken});
                } else if (constructedToken == "wain") {
                    state.addToken(Token{WAIN, col, line, constructedToken});
                } else if (constructedToken == "int") {
                    state.addToken(Token{INT, col, line, constructedToken});
                } else if (constructedToken == "new") {
                    state.addToken(Token{NEW, col, line, constructedToken});
                } else if (constructedToken == "delete") {
                    state.addToken(Token{DELETE, col, line, constructedToken});
                } else if (constructedToken == "NULL") {
                    state.addToken(Token{NULL_S, col, line, constructedToken});
                } else {
                    state.addToken(Token{ID, col, line, constructedToken});
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
                state.addToken(Token{NUM, col, line, constructedToken});

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

} // namespace wlp4
