// Self
#include "state.hpp"

// WLP4-LLVM
#include "token.hpp"

namespace wlp4 {

void State::setFilename(const char* name) {
    _filename = name;
}

const std::string& State::filename() {
    return _filename;
}

void State::addToken(Token token) {
    _tokens.push_back(token);
}

const Token& State::getToken(std::size_t idx) const {
    return _tokens[idx];
}

std::size_t State::numTokens() const {
    return _tokens.size();
}

} // namespace wlp4
