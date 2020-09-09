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
    return sym >= Symbol::ID && sym <= Symbol::NULL_S;
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
                state.addToken(Token{Symbol::LPAREN, col, line, "("});
            } else if (ch == ')') {
                state.addToken(Token{Symbol::RPAREN, col, line, ")"});
            } else if (ch == '{') {
                state.addToken(Token{Symbol::LBRACE, col, line, "{"});
            } else if (ch == '}') {
                state.addToken(Token{Symbol::RBRACE, col, line, "}"});
            } else if (ch == '+') {
                state.addToken(Token{Symbol::PLUS, col, line, "+"});
            } else if (ch == '-') {
                state.addToken(Token{Symbol::MINUS, col, line, "-"});
            } else if (ch == '*') {
                state.addToken(Token{Symbol::STAR, col, line, "*"});
            } else if (ch == '%') {
                state.addToken(Token{Symbol::PCT, col, line, "%"});
            } else if (ch == ',') {
                state.addToken(Token{Symbol::COMMA, col, line, ","});
            } else if (ch == ';') {
                state.addToken(Token{Symbol::SEMI, col, line, ";"});
            } else if (ch == '[') {
                state.addToken(Token{Symbol::LBRACK, col, line, "["});
            } else if (ch == ']') {
                state.addToken(Token{Symbol::RBRACK, col, line, "]"});
            } else if (ch == '&') {
                state.addToken(Token{Symbol::AMP, col, line, "&"});
            } else if (ch == '/') {
                ch = fs.get();
                if (ch == '/') {
                    ch = fs.get();
                    while (ch != '\n' && !fs.eof()) {
                        ch = fs.get();
                    }
                    fs.unget();
                } else {
                    state.addToken(Token{Symbol::SLASH, col, line, "/"});
                    fs.unget();
                }
            } else if (ch == '=') {
                ch = fs.get();
                if (ch == '=') {
                    state.addToken(Token{Symbol::EQ, col, line, "=="});
                } else {
                    state.addToken(Token{Symbol::BECOMES, col, line, "="});
                    fs.unget();
                }
            } else if (ch == '!') {
                ch = fs.get();
                if (ch == '=') {
                    state.addToken(Token{Symbol::NE, col, line, "!="});
                } else {
                    throw std::logic_error("Invalid token"); // FIXME: More descriptive error message
                }
            } else if (ch == '<') {
                ch = fs.get();
                if (ch == '=') {
                    state.addToken(Token{Symbol::LE, col, line, "<="});
                } else {
                    state.addToken(Token{Symbol::LT, col, line, "<"});
                    fs.unget();
                }
            } else if (ch == '>') {
                ch = fs.get();
                if (ch == '=') {
                    state.addToken(Token{Symbol::GE, col, line, ">="});
                } else {
                    state.addToken(Token{Symbol::GT, col, line, ">"});
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
                    state.addToken(Token{Symbol::RETURN, col, line, constructedToken});
                } else if (constructedToken == "if") {
                    state.addToken(Token{Symbol::IF, col, line, constructedToken});
                } else if (constructedToken == "else") {
                    state.addToken(Token{Symbol::ELSE, col, line, constructedToken});
                } else if (constructedToken == "while") {
                    state.addToken(Token{Symbol::WHILE, col, line, constructedToken});
                } else if (constructedToken == "println") {
                    state.addToken(Token{Symbol::PRINTLN, col, line, constructedToken});
                } else if (constructedToken == "wain") {
                    state.addToken(Token{Symbol::WAIN, col, line, constructedToken});
                } else if (constructedToken == "int") {
                    state.addToken(Token{Symbol::INT, col, line, constructedToken});
                } else if (constructedToken == "new") {
                    state.addToken(Token{Symbol::NEW, col, line, constructedToken});
                } else if (constructedToken == "delete") {
                    state.addToken(Token{Symbol::DELETE, col, line, constructedToken});
                } else if (constructedToken == "NULL") {
                    state.addToken(Token{Symbol::NULL_S, col, line, constructedToken});
                } else {
                    state.addToken(Token{Symbol::ID, col, line, constructedToken});
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
                state.addToken(Token{Symbol::NUM, col, line, constructedToken});

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
