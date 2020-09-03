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

using Rule = std::pair<Symbol, std::vector<Symbol>>;

class Elem {
public:
    Elem(const Rule& rule, std::size_t startIdx, std::size_t dot);
    ~Elem() = default;

    const Rule& rule() const;
    std::size_t startIdx() const;
    std::size_t dot() const;

    bool operator==(const Elem& el) const;

    Symbol nextSymbol() const;
    bool isComplete() const;

#ifdef DEBUG
    std::string op;
#endif

private:
    const Rule& _rule; // The CFG rule for this element
    std::size_t _startIdx; // The position in the input at which the matching of this production began
    std::size_t _dot; // The current position in that production
};

using Chart = std::vector<std::vector<std::unique_ptr<Elem>>>;

// An Earley Parser implementation
class Parser {
public:
    Parser() = default;
    ~Parser() = default;

    void parse(State& globalState);

private:
    void setupNullableRules();
    bool existsInStateList(const std::unique_ptr<Elem>& el, std::size_t stateListIdx);
#ifdef DEBUG
    void addToChart(const Rule& rule, std::size_t startIdx,
        std::size_t dot, std::size_t stateListIdx, const std::string& op);
#else
    void addToChart(const Rule& rule, std::size_t startIdx,
        std::size_t dot, std::size_t stateListIdx);
#endif
    void predictor(Symbol nextSym, std::size_t k, std::size_t l);
    void scanner(Symbol nextSym, std::size_t k);
    void completer(std::size_t k);
    void verifyCompleteChart(const State& globalState);
    void generateAST();

    Chart _chart;
    std::map<Symbol, bool> _nullable;
};

} // namespace wlp4


#endif // __WLP4_LLVM_PARSER
