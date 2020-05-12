#ifndef __WLP4_LLVM_PARSER
#define __WLP4_LLVM_PARSER

#include "tokeniser.hpp"
#include "ast/procedure.hpp"

#include <memory>
#include <vector>

namespace wlp4 {

class Parser {
public:
    Parser(Tokeniser *tokeniser) : _tokeniser(tokeniser) {}

    void parseTokens(std::vector<std::unique_ptr<ast::Procedure>>& procedures) noexcept(false);

private:
    std::unique_ptr<ast::Procedure> parseProcedure() noexcept(false);
    void parseProcedureParam(std::unique_ptr<ast::Procedure>& procedure) noexcept(false);
    void parseDeclaration(std::unique_ptr<ast::Procedure>& procedure) noexcept(false);

    Tokeniser *_tokeniser;
    std::vector<std::string> _procedureNames;
};

} // namespace wlp4


#endif // __WLP4_LLVM_PARSER
