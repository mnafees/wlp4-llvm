// Self
#include "elem.hpp"

// WLP4-LLVM
#include "state.hpp"

namespace wlp4 {

Elem::Elem(std::size_t ruleIdx, std::size_t startIdx, std::size_t dot)
    : _ruleIdx(ruleIdx), _startIdx(startIdx), _dot(dot) {}

Symbol Elem::nextSymbol() const {
    return CFG[_ruleIdx].second[_dot];
}

bool Elem::isComplete() const {
    return _dot == CFG[_ruleIdx].second.size();
}

std::size_t Elem::ruleIdx() const {
    return _ruleIdx;
}

std::size_t Elem::startIdx() const {
    return _startIdx;
}

std::size_t Elem::dot() const {
    return _dot;
}

} // namespace wlp4
