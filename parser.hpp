#ifndef __WLP4_LLVM_PARSER
#define __WLP4_LLVM_PARSER

// STL
#include <map>
#include <memory>
#include <vector>

// WLP4-LLVM
#include "symbol.hpp"

namespace wlp4 {

class State;

struct EarleyState {
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

using Chart = std::vector<std::vector<EarleyState>>;

// An Earley Parser implementation
class Parser {
public:
    Parser() = default;
    ~Parser() = default;

    void parse(State& globalState);

private:
    void setupNullableRules();
    bool existsInStateList(const EarleyState& state, std::size_t stateListIdx);
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
    void generateAST();

    Chart _chart;
    std::map<Symbol, bool> _nullable;
};

} // namespace wlp4


#endif // __WLP4_LLVM_PARSER
