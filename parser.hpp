#ifndef __WLP4_LLVM_PARSER
#define __WLP4_LLVM_PARSER

// STL
#include <map>
#include <memory>
#include <vector>

// WLP4-LLVM
#include "tokeniser.hpp"
#include "ast/procedure.hpp"
#include "ast/statement.hpp"
#include "ast/test.hpp"
#include "ast/expr.hpp"

namespace wlp4 {

struct State {
    Symbol lhs; // The left hand side non-terminal symbol
    std::vector<Symbol> rhs; // The right hand side set of symbols
    std::size_t startIdx; // The position in the input at which the matching of this production began
    std::size_t dot; // The current position in that production
#ifdef DEBUG
    std::string op;
#endif

    Symbol nextSymbol() const;
    bool isComplete() const;
};

using StateList = std::vector<State>;
using Chart = std::vector<StateList>;

// An Earley Parser implementation
class Parser {
public:
    std::vector<std::unique_ptr<ast::Procedure>> parse(const Tokeniser& tokeniser);

private:
    void setupNullableRules();
    bool existsInStateList(const State& state, std::size_t stateListIdx);
#ifdef DEBUG
    void addToChart(std::pair<Symbol, std::vector<Symbol>> rule, std::size_t startIdx,
        std::size_t dot, std::size_t stateListIdx, const std::string& op);
#else
    void addToChart(std::pair<Symbol, std::vector<Symbol>> rule, std::size_t startIdx,
        std::size_t dot, std::size_t stateListIdx);
#endif
    void predictor(Symbol nextSym, std::size_t k, std::size_t l);
    void scanner(Symbol nextSym, std::size_t k);
    void completer(std::size_t k);

    Chart _chart;
    std::map<Symbol, bool> _nullable;
};

} // namespace wlp4


#endif // __WLP4_LLVM_PARSER
