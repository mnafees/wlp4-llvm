// Self
#include "recogniser.hpp"

// STL
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

// WLP4-LLVM
#include "state.hpp"
#include "token.hpp"
#include "ast/procedure.hpp"
#include "ast/statement.hpp"
#include "ast/test.hpp"
#include "ast/expr.hpp"

namespace wlp4 {

Symbol Elem::nextSymbol() const {
    return CFG[_ruleIdx].second[_dot];
}

bool Elem::isComplete() const {
    return _dot == CFG[_ruleIdx].second.size();
}

void Recogniser::recognise(State& globalState) {
    setupNullableRules();

#ifdef DEBUG
    addToChart(0, 0, 0, 0, "initial");
#else
    addToChart(0, 0, 0, 0);
#endif

    for (std::size_t i = 0; i < globalState.numTokens(); ++i) {
        if (i > _chart.size() - 1) {
            throw std::runtime_error("Invalid token " + globalState.getToken(i - 1).value);
        }

        for (std::size_t j = 0; j < _chart[i].size(); ++j) {
            const auto& el = _chart[i][j];
#ifdef DEBUG
            std::cout << "Processing elem list #" << i << std::endl
                      << "  Elem {" << std::endl
                      << "    lhs: " << symToStr[CFG[el->ruleIdx()].first] << std::endl
                      << "    rhs: ";
            for (auto sym : CFG[el->ruleIdx()].second) {
                std::cout << symToStr[sym] << " ";
            }
            std::cout << std::endl
                      << "    startIdx: " << el->startIdx() << std::endl
                      << "    dot: " << el->dot() << std::endl
                      << "  }" << std::endl;
#endif
            if (el->isComplete()) {
                completer(i);
            } else {
                const auto nextSym = el->nextSymbol();
                if (isTerminal(nextSym)) {
                    scanner(globalState.getToken(i).type, i);
                } else {
                    predictor(nextSym, i, j);
                }
            }
        }
    }

    if (_chart.size() - 1 == globalState.numTokens()) {
        populateFinalChart(globalState);
    } else {
        throw std::runtime_error("Invalid WLP4 code");
    }
}

void Recogniser::setupNullableRules() {
    for (unsigned char i = 33; i <= 48; ++i) {
        _nullable[Symbol(i)] = false;
    }

    for (const auto& r : CFG) {
        if (r.second.empty()) {
#ifdef DEBUG
            std::cout << symToStr[r.first] << " is nullable" << std::endl;
#endif
            _nullable[r.first] = true;
        }
    }
}

bool Recogniser::existsInStateList(const std::unique_ptr<Elem>& el, std::size_t stateListIdx) {
    for (const auto& s : _chart[stateListIdx]) {
        if (s->ruleIdx() == el->ruleIdx() && s->startIdx() == el->startIdx() && s->dot() == el->dot()) {
            return true;
        }
    }

    return false;
}

#ifdef DEBUG
void Recogniser::addToChart(std::size_t ruleIdx, std::size_t startIdx, std::size_t dot, std::size_t stateListIdx,
                        const std::string& op) {
#else
void Recogniser::addToChart(std::size_t ruleIdx, std::size_t startIdx, std::size_t dot, std::size_t stateListIdx) {
#endif
#ifdef DEBUG
    std::cout << "addToChart():" << std::endl;
#endif

    if (stateListIdx == _chart.size()) {
        _chart.push_back(std::vector<std::unique_ptr<Elem>>());
    }

    std::unique_ptr<Elem> el(new Elem(ruleIdx, startIdx, dot));
#ifdef DEBUG
    el->op = op;
#endif

    if (!existsInStateList(el, stateListIdx)) {
#ifdef DEBUG
        std::cout << "Pushed new state in state list #" << stateListIdx << std::endl
                  << "Elem {" << std::endl
                  << "  lhs: " << symToStr[CFG[el->ruleIdx()].first] << std::endl
                  << "  rhs: ";
        for (auto sym : CFG[el->ruleIdx()].second) {
            std::cout << symToStr[sym] << " ";
        }
        std::cout << std::endl
                  << "  startIdx: " << el->startIdx() << std::endl
                  << "  dot: " << el->dot() << std::endl
                  << "  op: " << el->op << std::endl
                  << "}" << std::endl;
#endif
        _chart[stateListIdx].push_back(std::move(el));
    }
}

void Recogniser::predictor(Symbol nextSym, std::size_t k, std::size_t l) {
#ifdef DEBUG
    std::cout << "predictor for " << symToStr[nextSym] << std::endl;
#endif

    for (std::size_t i = 0; i < CFG.size(); ++i) {
        if (CFG[i].first != nextSym) continue;

#ifdef DEBUG
        addToChart(i, k, 0, k, "predictor");
#else
        addToChart(i, k, 0, k);
#endif
        if (_nullable[nextSym]) {
#ifdef DEBUG
            addToChart(_chart[k][l]->ruleIdx(), _chart[k][l]->startIdx(), _chart[k][l]->dot() + 1, k,
                "predictor_nullable");
#else
            addToChart(_chart[k][l]->ruleIdx(), _chart[k][l]->startIdx(), _chart[k][l]->dot() + 1, k);
#endif
        }
    }
}

void Recogniser::scanner(Symbol nextSym, std::size_t k) {
#ifdef DEBUG
    std::cout << "scanner for " << symToStr[nextSym] << std::endl;
#endif

    for (const auto& el : _chart[k]) {
        if (!el->isComplete() && el->nextSymbol() == nextSym) {
#ifdef DEBUG
            addToChart(el->ruleIdx(), el->startIdx(), el->dot() + 1, k + 1, "scanner");
#else
            addToChart(el->ruleIdx(), el->startIdx(), el->dot() + 1, k + 1);
#endif
        }
    }
}

void Recogniser::completer(std::size_t k) {
#ifdef DEBUG
    std::cout << "completer for state list #" << k << std::endl;
#endif

    for (std::size_t j = 0; j < _chart[k].size(); ++j) {
        if (_chart[k][j]->isComplete()) {
            for (std::size_t i = 0; i < _chart[_chart[k][j]->startIdx()].size(); ++i) {
                if (!_chart[_chart[k][j]->startIdx()][i]->isComplete() &&
                    _chart[_chart[k][j]->startIdx()][i]->nextSymbol() == CFG[_chart[k][j]->ruleIdx()].first) {
#ifdef DEBUG
                    addToChart(_chart[_chart[k][j]->startIdx()][i]->ruleIdx(),
                        _chart[_chart[k][j]->startIdx()][i]->startIdx(),
                        _chart[_chart[k][j]->startIdx()][i]->dot() + 1, k, "completer");
#else
                    addToChart(_chart[_chart[k][j]->startIdx()][i]->ruleIdx(),
                        _chart[_chart[k][j]->startIdx()][i]->startIdx(),
                        _chart[_chart[k][j]->startIdx()][i]->dot() + 1, k);
#endif
                }
            }
        }
    }
}

void Recogniser::populateFinalChart(State& globalState) {
    // Iterate and keep only complete elems and elem lists
    for (auto it = _chart.begin(); it != _chart.end(); ++it) {
        for (auto elit = it->begin(); elit != it->end(); ++elit) {
            if ((*elit)->isComplete()) {
                globalState.addToFinalChart(std::move(*elit));
            }
        }
    }
}

} // namespace wlp4
