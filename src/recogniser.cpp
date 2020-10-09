// Self
#include "recogniser.hpp"

// STL
#include <fstream>
#include <sstream>
#include <stdexcept>

// fmt
#include "fmt/format.h"

// WLP4-LLVM
#include "state.hpp"
#include "token.hpp"
#include "ast/procedure.hpp"
#include "ast/statement.hpp"
#include "ast/test.hpp"
#include "ast/expr.hpp"

namespace wlp4 {

void Recogniser::recognise() {
    setupNullableRules();

    addToChart(0, 0, 0, 0);

    for (std::size_t i = 0; i < STATE.numTokens(); ++i) {
        if (i > _chart.size() - 1) {
            const auto& tok = STATE.getToken(i - 1);
            throw std::runtime_error(fmt::format("invalid token {} in {}:{}:{}", tok.value,
                STATE.inputFilePath(), tok.line, tok.col));
        }

        for (std::size_t j = 0; j < _chart[i].size(); ++j) {
            const auto& el = _chart[i][j];
            if (el->isComplete()) {
                completer(i);
            } else {
                const auto nextSym = el->nextSymbol();
                if (isTerminal(nextSym)) {
                    scanner(STATE.getToken(i).type, i);
                } else {
                    predictor(nextSym, i, j);
                }
            }
        }
    }

    if (_chart.size() - 1 == STATE.numTokens()) {
        populateFinalChart();
    } else {
        throw std::runtime_error("invalid WLP4 code");
    }
}

void Recogniser::setupNullableRules() {
    for (unsigned char i = 33; i <= 48; ++i) {
        _nullable[Symbol(i)] = false;
    }

    for (const auto& r : CFG) {
        if (r.second.empty()) {
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

void Recogniser::addToChart(std::size_t ruleIdx, std::size_t startIdx, std::size_t dot, std::size_t stateListIdx) {
    if (stateListIdx == _chart.size()) {
        _chart.push_back(std::vector<std::unique_ptr<Elem>>());
    }

    std::unique_ptr<Elem> el(new Elem(ruleIdx, startIdx, dot));
    if (!existsInStateList(el, stateListIdx)) {
        _chart[stateListIdx].push_back(std::move(el));
    }
}

void Recogniser::predictor(Symbol nextSym, std::size_t k, std::size_t l) {
    for (std::size_t i = 0; i < CFG.size(); ++i) {
        if (CFG[i].first != nextSym) continue;

        addToChart(i, k, 0, k);
        if (_nullable[nextSym]) {
            addToChart(_chart[k][l]->ruleIdx(), _chart[k][l]->startIdx(), _chart[k][l]->dot() + 1, k);
        }
    }
}

void Recogniser::scanner(Symbol nextSym, std::size_t k) {
    for (const auto& el : _chart[k]) {
        if (!el->isComplete() && el->nextSymbol() == nextSym) {
            addToChart(el->ruleIdx(), el->startIdx(), el->dot() + 1, k + 1);
        }
    }
}

void Recogniser::completer(std::size_t k) {
    for (std::size_t j = 0; j < _chart[k].size(); ++j) {
        if (_chart[k][j]->isComplete()) {
            for (std::size_t i = 0; i < _chart[_chart[k][j]->startIdx()].size(); ++i) {
                if (!_chart[_chart[k][j]->startIdx()][i]->isComplete() &&
                    _chart[_chart[k][j]->startIdx()][i]->nextSymbol() == CFG[_chart[k][j]->ruleIdx()].first) {
                    addToChart(_chart[_chart[k][j]->startIdx()][i]->ruleIdx(),
                        _chart[_chart[k][j]->startIdx()][i]->startIdx(),
                        _chart[_chart[k][j]->startIdx()][i]->dot() + 1, k);
                }
            }
        }
    }
}

void Recogniser::populateFinalChart() {
    // Iterate and keep only complete elems and elem lists
    for (auto& lst : _chart) {
        for (auto& el : lst) {
            if (el->isComplete()) {
                STATE.addToFinalChart(std::move(el));
            }
        }
    }
}

} // namespace wlp4
