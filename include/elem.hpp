#ifndef __WLP4_LLVM_ELEM
#define __WLP4_LLVM_ELEM

// STL
#include <string>

// WLP4-LLVM
#include "symbol.hpp"

namespace wlp4 {

class Elem {
public:
    Elem(std::size_t ruleIdx, std::size_t startIdx, std::size_t dot);

    std::size_t ruleIdx() const;
    std::size_t startIdx() const;
    std::size_t dot() const;

    Symbol nextSymbol() const;
    bool isComplete() const;

#ifdef DEBUG
    std::string op;
#endif

private:
    std::size_t _ruleIdx; // The index of the CFG rule for this element
    std::size_t _startIdx; // The position in the input at which the matching of this production began
    std::size_t _dot; // The current position in that production
};

} // namespace wlp4

#endif // __WLP4_LLVM_ELEM
