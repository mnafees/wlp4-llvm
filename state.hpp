#ifndef __WLP4_LLVM_STATE
#define __WLP4_LLVM_STATE

// STL
#include <list>
#include <memory>
#include <vector>
#ifdef DEBUG
#include <map>
#endif

// WLP4-LLVM
#include "elem.hpp"
#include "token.hpp"
#include "ast/procedure.hpp"

namespace wlp4 {

// Context-free grammar taken from https://www.student.cs.uwaterloo.ca/~cs241/wlp4/WLP4.html
extern std::vector<std::pair<Symbol, std::vector<Symbol>>> CFG;
#ifdef DEBUG
extern std::map<Symbol, std::string> symToStr;
#endif

class State {
public:
    explicit State(const char* name);

    const std::string& filename() const;

    void addToken(Token token);
    const Token& getToken(std::size_t idx) const;
    std::size_t numTokens() const;

    void addToFinalChart(std::unique_ptr<Elem> elem);
    const std::vector<std::unique_ptr<Elem>>& finalChart() const;

    void addProcedure(std::unique_ptr<ast::Procedure> proc);

private:
    std::string _filename;
    std::vector<Token> _tokens;
    std::vector<std::unique_ptr<Elem>> _chart;
    std::vector<std::unique_ptr<ast::Procedure>> _procedures;
};

} // namespace wlp4

#endif // __WLP4_LLVM_STATE
