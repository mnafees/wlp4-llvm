#ifndef __WLP4_LLVM_PARSER
#define __WLP4_LLVM_PARSER

// STL
#include <list>
#include <map>
#include <memory>
#include <vector>

// WLP4-LLVM
#include "symbol.hpp"

namespace wlp4 {

namespace ast {
class Expr;
} // namespace ast

class State;

using Rule = std::pair<const Symbol, std::vector<Symbol>>;

class Elem {
public:
    Elem(std::size_t ruleIdx, std::size_t startIdx, std::size_t dot);
    ~Elem() = default;

    std::size_t ruleIdx() const;
    std::size_t startIdx() const;
    std::size_t dot() const;

    Symbol nextSymbol() const;
    bool isComplete() const;

#ifdef DEBUG
    std::string op;
#endif

private:
    std::size_t _ruleIdx; // The index of the CFG rule for this element
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
    void addToChart(std::size_t ruleIdx, std::size_t startIdx,
        std::size_t dot, std::size_t stateListIdx, const std::string& op);
#else
    void addToChart(std::size_t ruleIdx, std::size_t startIdx,
        std::size_t dot, std::size_t stateListIdx);
#endif
    void predictor(Symbol nextSym, std::size_t k, std::size_t l);
    void scanner(Symbol nextSym, std::size_t k);
    void completer(std::size_t k);
    void populateProcedureNames(const State& globalState);
    void cleanupChart();
    void generateAST(State& globalState);
    std::pair<std::size_t, std::unique_ptr<ast::Expr>> parseExpr(std::size_t currElemListIdx);

    Chart _chart;
    std::map<Symbol, bool> _nullable;
    std::list<std::string> _procedureNames;
};

} // namespace wlp4


#endif // __WLP4_LLVM_PARSER
