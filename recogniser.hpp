#ifndef __WLP4_LLVM_RECOGNISER
#define __WLP4_LLVM_RECOGNISER

// STL
#include <list>
#include <map>
#include <memory>
#include <vector>

// WLP4-LLVM
#include "elem.hpp"

namespace wlp4 {

class State;

using Chart = std::vector<std::vector<std::unique_ptr<Elem>>>;

// An Earley Recogniser implementation
class Recogniser {
public:
    Recogniser() = default;
    ~Recogniser() = default;

    void recognise(State& globalState);

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
    void populateProcedureNames(State& globalState);
    void populateFinalChart(State& globalState);

    Chart _chart;
    std::map<Symbol, bool> _nullable;
};

} // namespace wlp4

#endif // __WLP4_LLVM_RECOGNISER