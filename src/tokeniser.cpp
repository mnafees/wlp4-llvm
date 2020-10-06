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

void Tokeniser::tokenise() {
    using namespace std::string_literals;

    std::ifstream fs(STATE.inputFilePath());
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
                } else {
                    col++;
                }
                ch = fs.get();
                continue;
            } else if (ch == '(') {
                STATE.addToken(Token{Symbol::LPAREN, col, line, "("});
            } else if (ch == ')') {
                STATE.addToken(Token{Symbol::RPAREN, col, line, ")"});
            } else if (ch == '{') {
                STATE.addToken(Token{Symbol::LBRACE, col, line, "{"});
            } else if (ch == '}') {
                STATE.addToken(Token{Symbol::RBRACE, col, line, "}"});
            } else if (ch == '+') {
                STATE.addToken(Token{Symbol::PLUS, col, line, "+"});
            } else if (ch == '-') {
                STATE.addToken(Token{Symbol::MINUS, col, line, "-"});
            } else if (ch == '*') {
                STATE.addToken(Token{Symbol::STAR, col, line, "*"});
            } else if (ch == '%') {
                STATE.addToken(Token{Symbol::PCT, col, line, "%"});
            } else if (ch == ',') {
                STATE.addToken(Token{Symbol::COMMA, col, line, ","});
            } else if (ch == ';') {
                STATE.addToken(Token{Symbol::SEMI, col, line, ";"});
            } else if (ch == '[') {
                STATE.addToken(Token{Symbol::LBRACK, col, line, "["});
            } else if (ch == ']') {
                STATE.addToken(Token{Symbol::RBRACK, col, line, "]"});
            } else if (ch == '&') {
                STATE.addToken(Token{Symbol::AMP, col, line, "&"});
            } else if (ch == '/') {
                ch = fs.get();
                if (ch == '/') {
                    ch = fs.get();
                    while (ch != '\n' && !fs.eof()) {
                        ch = fs.get();
                    }
                    fs.unget();
                } else {
                    STATE.addToken(Token{Symbol::SLASH, col, line, "/"});
                    fs.unget();
                }
            } else if (ch == '=') {
                ch = fs.get();
                if (ch == '=') {
                    STATE.addToken(Token{Symbol::EQ, col, line, "=="});
                } else {
                    STATE.addToken(Token{Symbol::BECOMES, col, line, "="});
                    fs.unget();
                }
            } else if (ch == '!') {
                ch = fs.get();
                if (ch == '=') {
                    STATE.addToken(Token{Symbol::NE, col, line, "!="});
                } else {
                    throw std::logic_error("Invalid token"); // FIXME: More descriptive error message
                }
            } else if (ch == '<') {
                ch = fs.get();
                if (ch == '=') {
                    STATE.addToken(Token{Symbol::LE, col, line, "<="});
                } else {
                    STATE.addToken(Token{Symbol::LT, col, line, "<"});
                    fs.unget();
                }
            } else if (ch == '>') {
                ch = fs.get();
                if (ch == '=') {
                    STATE.addToken(Token{Symbol::GE, col, line, ">="});
                } else {
                    STATE.addToken(Token{Symbol::GT, col, line, ">"});
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
                    STATE.addToken(Token{Symbol::RETURN, col, line, constructedToken});
                } else if (constructedToken == "if") {
                    STATE.addToken(Token{Symbol::IF, col, line, constructedToken});
                } else if (constructedToken == "else") {
                    STATE.addToken(Token{Symbol::ELSE, col, line, constructedToken});
                } else if (constructedToken == "while") {
                    STATE.addToken(Token{Symbol::WHILE, col, line, constructedToken});
                } else if (constructedToken == "println") {
                    STATE.addToken(Token{Symbol::PRINTLN, col, line, constructedToken});
                } else if (constructedToken == "wain") {
                    STATE.addToken(Token{Symbol::WAIN, col, line, constructedToken});
                } else if (constructedToken == "int") {
                    STATE.addToken(Token{Symbol::INT, col, line, constructedToken});
                } else if (constructedToken == "new") {
                    STATE.addToken(Token{Symbol::NEW, col, line, constructedToken});
                } else if (constructedToken == "delete") {
                    STATE.addToken(Token{Symbol::DELETE, col, line, constructedToken});
                } else if (constructedToken == "NULL") {
                    STATE.addToken(Token{Symbol::NULL_S, col, line, constructedToken});
                } else {
                    STATE.addToken(Token{Symbol::ID, col, line, constructedToken});
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
                STATE.addToken(Token{Symbol::NUM, col, line, constructedToken});

                constructedToken.clear();
            } else {
                throw std::logic_error("Unknown token");
            }
            ch = fs.get();
            col++;
        }
    } else {
        throw std::runtime_error("Could not open source file: "s + STATE.inputFilePath());
    }
}

} // namespace wlp4
