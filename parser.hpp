#ifndef __WLP4_LLVM_PARSER
#define __WLP4_LLVM_PARSER

// STL
#include <list>
#include <map>
#include <memory>
#include <vector>

// WLP4-LLVM
#include "elem.hpp"

namespace wlp4 {

class State;

using Rule = std::pair<const Symbol, std::vector<Symbol>>;
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
    // void populateProcedureNames(const State& globalState);
    void cleanupChart();

    Chart _chart;
    std::map<Symbol, bool> _nullable;
};

} // namespace wlp4


#endif // __WLP4_LLVM_PARSER
