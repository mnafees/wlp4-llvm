#ifndef __WLP4_LLVM_PARSER
#define __WLP4_LLVM_PARSER

#include "tokeniser.hpp"

#include <map>

namespace wlp4 {

class Parser {
public:
    Parser();

    void createAST(Tokeniser* tokeniser);

private:
    void constructCFGTable() noexcept(false);
    bool isTerminal(const std::string& sym) const;

    const std::string _startSym;
    std::map<Token, std::string> _tokenToTerminal;
    std::multimap<std::string, std::vector<std::string>> _cfg;
};

} // namespace wlp4


#endif // __WLP4_LLVM_PARSER
