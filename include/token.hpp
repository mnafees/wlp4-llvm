#pragma once
#ifndef __WLP4_LLVM_TOKEN
#define __WLP4_LLVM_TOKEN

// STL
#include <string>

// WLP4-LLVM
#include "symbol.hpp"

namespace wlp4 {

bool isTerminal(Symbol sym);

struct Token {
    Symbol type;
    int col;
    int line;
    std::string value;
};

} // namespace wlp4

#endif // __WLP4_LLVM_TOKEN
