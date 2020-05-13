#ifndef __WLP4_LLVM_PARSER
#define __WLP4_LLVM_PARSER

#include "tokeniser.hpp"
#include "ast/procedure.hpp"

#include <memory>
#include <vector>

namespace wlp4 {

class Parser {
public:
    Parser(std::unique_ptr<Tokeniser>& tokeniser);

    void parseTokens(std::vector<std::unique_ptr<ast::Procedure>>& procedures) noexcept(false);

private:
    std::unique_ptr<ast::Procedure> parseProcedure() noexcept(false);
    void parseDcl(std::unique_ptr<ast::Procedure>& procedure, bool isParam = false) noexcept(false);
    void parseStatement(std::unique_ptr<ast::Procedure>& procedure) noexcept(false);

    std::unique_ptr<Tokeniser>& _tokeniser;
    std::vector<std::string> _procedureNames;
};

} // namespace wlp4


#endif // __WLP4_LLVM_PARSER
