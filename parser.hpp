#ifndef __WLP4_LLVM_PARSER
#define __WLP4_LLVM_PARSER

#include "tokeniser.hpp"

namespace wlp4 {

class Parser {
public:
    void parseTokens(Tokeniser* tokeniser);
};

} // namespace wlp4


#endif // __WLP4_LLVM_PARSER
