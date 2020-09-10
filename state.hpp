#ifndef __WLP4_LLVM_STATE
#define __WLP4_LLVM_STATE

// STL
#include <list>
#include <vector>

// WLP4-LLVM
#include "elem.hpp"
#include "token.hpp"
#include "ast/procedure.hpp"

namespace wlp4 {

class State {
public:
    explicit State(const char* name);
    ~State() = default;

    const auto& filename() const;

    void addToken(Token token);
    const Token& getToken(std::size_t idx) const;
    std::size_t numTokens() const;

    auto hasProcedure(const std::string& name) const;
    void addProcedure(std::unique_ptr<ast::Procedure> procedure);
    const auto& getProcedures() const;

private:
    std::string _filename;
    std::vector<Token> _tokens;
    std::vector<std::unique_ptr<Elem>> _chart;
    std::list<std::unique_ptr<ast::Procedure>> _procedures;
};

} // namespace wlp4

#endif // __WLP4_LLVM_STATE
