#ifndef __WLP4_LLVM_STATE
#define __WLP4_LLVM_STATE

// STL
#include <vector>

// WKL4-LLVM
#include "token.hpp"
#include "ast/procedure.hpp"

namespace wlp4 {

class State {
public:
    State() = default;
    ~State() = default;

    void setFilename(const char* name);
    const std::string& filename();

    void addToken(Token token);
    const Token& getToken(std::size_t idx);
    std::size_t numTokens();

private:
    std::string _filename;
    std::vector<Token> _tokens;
    std::vector<ast::Procedure> _procedures;
};

} // namespace wlp4

#endif // __WLP4_LLVM_STATE
