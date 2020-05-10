#ifndef __WLP4_LLVM_PARSER
#define __WLP4_LLVM_PARSER

#include "tokeniser.hpp"
#include "ast/procedure.hpp"

#include <map>
#include <memory>
#include <vector>

namespace wlp4 {

class Parser {
public:
    Parser(Tokeniser *tokeniser);

    void parseTokens(std::vector<std::unique_ptr<ast::Procedure>>& procedures);

private:
    std::unique_ptr<ast::Procedure> parseProcedure();
    std::unique_ptr<ast::Dcl> parseDcl();

    Tokeniser *_tokeniser;
    std::map<int, std::string> _tokenToTerminal;
    std::vector<std::string> _procedureNames;
};

} // namespace wlp4


#endif // __WLP4_LLVM_PARSER
