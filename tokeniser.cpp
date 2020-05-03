#include "tokeniser.hpp"

#include <exception>
#include <fstream>
#include <stdexcept>
#include <sstream>

namespace wlp4 {

void Tokeniser::scanFileForTokens(const char* name) {
    std::ifstream fs;
    fs.open(name);
    if (fs.is_open()) {
        std::string line;
        std::istringstream ss;
        std::string token;
        while (std::getline(fs, line)) {
            ss.str(line);
            while (ss >> token) {
                bool endsWithSemi = false;
                if (token[token.length()-1] == ';') {
                    endsWithSemi = true;
                    token = token.substr(0, token.length()-1);
                }
                if (token == "(") {
                    _symbols.push_back(std::make_pair(tok_lparen, token));
                } else if (token == ")") {
                    _symbols.push_back(std::make_pair(tok_rparen, token));
                } else if (token == "{") {
                    _symbols.push_back(std::make_pair(tok_lbrace, token));
                } else if (token == "}") {
                    _symbols.push_back(std::make_pair(tok_rbrace, token));
                } else if (token == "return") {
                    _symbols.push_back(std::make_pair(tok_return, token));
                } else if (token == "if") {
                    _symbols.push_back(std::make_pair(tok_if, token));
                } else if (token == "else") {
                    _symbols.push_back(std::make_pair(tok_else, token));
                } else if (token == "while") {
                    _symbols.push_back(std::make_pair(tok_while, token));
                } else if (token == "println") {
                    _symbols.push_back(std::make_pair(tok_println, token));
                } else if (token == "wain") {
                    _symbols.push_back(std::make_pair(tok_wain, token));
                } else if (token == "=") {
                    _symbols.push_back(std::make_pair(tok_becomes, token));
                } else if (token == "int") {
                    _symbols.push_back(std::make_pair(tok_int, token));
                } else if (token == "==") {
                    _symbols.push_back(std::make_pair(tok_eq, token));
                } else if (token == "!=") {
                    _symbols.push_back(std::make_pair(tok_ne, token));
                } else if (token == "<") {
                    _symbols.push_back(std::make_pair(tok_lt, token));
                } else if (token == ">") {
                    _symbols.push_back(std::make_pair(tok_gt, token));
                } else if (token == ">=") {
                    _symbols.push_back(std::make_pair(tok_ge, token));
                } else if (token == "<=") {
                    _symbols.push_back(std::make_pair(tok_le, token));
                } else if (token == "+") {
                    _symbols.push_back(std::make_pair(tok_plus, token));
                } else if (token == "-") {
                    _symbols.push_back(std::make_pair(tok_minus, token));
                } else if (token == "*") {
                    _symbols.push_back(std::make_pair(tok_star, token));
                } else if (token == "/") {
                    _symbols.push_back(std::make_pair(tok_slash, token));
                } else if (token == "%") {
                    _symbols.push_back(std::make_pair(tok_pct, token));
                } else if (token == ",") {
                    _symbols.push_back(std::make_pair(tok_comma, token));
                } else if (token == ";") {
                    _symbols.push_back(std::make_pair(tok_semi, token));
                } else if (token == "new") {
                    _symbols.push_back(std::make_pair(tok_new, token));
                } else if (token == "delete") {
                    _symbols.push_back(std::make_pair(tok_delete, token));
                } else if (token == "[") {
                    _symbols.push_back(std::make_pair(tok_lbrack, token));
                } else if (token == "]") {
                    _symbols.push_back(std::make_pair(tok_rbrack, token));
                } else if (token == "&") {
                    _symbols.push_back(std::make_pair(tok_amp, token));
                } else if (token == "NULL") {
                    _symbols.push_back(std::make_pair(tok_null, token));
                } else if ((token[0] >= 'a' && token[0] <= 'z') || (token[0] >= 'A' && token[0] <= 'Z')) {
                    _symbols.push_back(std::make_pair(tok_id, token));
                } else if (token.length() == 1 && token[0] >= '0' && token[0] <= '9') {
                    _symbols.push_back(std::make_pair(tok_num, token));
                } else if (token[0] >= '1' && token[0] <= '9') {
                    // Could be a tok_num
                    for (auto& c : token) {
                        if (!isdigit(c)) {
                            throw std::logic_error("Invalid token " + token);
                        }
                    }
                    if (std::stol(token) > INT32_MAX) {
                        throw std::logic_error("Number value exceeds the maximum allowed limit");
                    }
                    _symbols.push_back(std::make_pair(tok_num, token));
                } else {
                    // Unknown token
                    throw std::runtime_error("Unknown token " + token);
                }
                if (endsWithSemi) {
                    _symbols.push_back(std::make_pair(tok_semi, ";"));
                }
            }
        }
        fs.close();
    } else {
        throw std::runtime_error("Could not open source file: " + std::string(name));
    }
}

bool Tokeniser::hasNextToken() const {
    return _currentTokenIdx >= 0 && _currentTokenIdx < _symbols.size()-1;
}

const std::pair<Token, std::string>& Tokeniser::nextToken() {
    if (_currentTokenIdx > _symbols.size()) {
        throw std::range_error("Trying to get token out of index");
    }
    return _symbols[_currentTokenIdx++];
}

} // namespace wlp4
