// Self
#include "state.hpp"

// WLP4-LLVM
#include "token.hpp"

namespace wlp4 {

State::State(const char* name) : _filename(name) {}

const auto& State::filename() const {
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

auto State::hasProcedure(const std::string& name) const {
    for (const auto& p : _procedures) {
        if (p->name() == name) {
            return true;
        }
    }
    return false;
}

void State::addProcedure(std::unique_ptr<ast::Procedure> procedure) {
    if (!hasProcedure(procedure->name())) {
        _procedures.push_front(std::move(procedure));
    }
}

const auto& State::getProcedures() const {
    return _procedures;
}

} // namespace wlp4
