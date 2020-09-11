// Self
#include "state.hpp"

// WLP4-LLVM
#include "token.hpp"

namespace wlp4 {

State::State(const char* name) : _filename(name) {}

const std::string& State::filename() const {
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

void State::addToFinalChart(std::unique_ptr<Elem> elem) {
    _chart.push_back(std::move(elem));
}

} // namespace wlp4
