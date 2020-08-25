// Self
#include "parser.hpp"

// STL
#include <fstream>
#include <sstream>
#include <map>
#include <stdexcept>
#include <iostream>

// WLP4-LLVM
#include "state.hpp"
#include "token.hpp"
#include "debug.hpp"
#include "ast/procedure.hpp"
#include "ast/statement.hpp"
#include "ast/test.hpp"
#include "ast/expr.hpp"

namespace wlp4 {

// Context-free grammar taken from https://www.student.cs.uwaterloo.ca/~cs241/wlp4/WLP4.html
const std::multimap<Symbol, std::vector<Symbol>> CFG = {
    { procedures, { procedure, procedures } },
    { procedures, { main_s } },
    { procedure, { INT, ID, LPAREN, params, RPAREN, LBRACE, dcls, statements, RETURN,
                        expr, SEMI, RBRACE } },
    { main_s, { INT, WAIN, LPAREN, dcl, COMMA, dcl, RPAREN, LBRACE, dcls, statements,
                RETURN, expr, SEMI, RBRACE } },
    { params, {} },
    { params, { paramlist } },
    { paramlist, { dcl } },
    { paramlist, { dcl, COMMA, paramlist } },
    { type, { INT } },
    { type, { INT, STAR } },
    { dcls, {} },
    { dcls, { dcls, dcl, BECOMES, NUM, SEMI } },
    { dcls, { dcls, dcl, BECOMES, NULL_S, SEMI } },
    { dcl, { type, ID } },
    { statements, {} },
    { statements, { statements, statement } },
    { statement, { lvalue, BECOMES, expr, SEMI } },
    { statement, { IF, LPAREN, test, RPAREN, LBRACE, statements, RBRACE, ELSE, LBRACE,
                        statements, RBRACE } },
    { statement, { WHILE, LPAREN, test, RPAREN, LBRACE, statements, RBRACE } },
    { statement, { PRINTLN, LPAREN, expr, RPAREN, SEMI } },
    { statement, { DELETE, LBRACK, RBRACK, expr, SEMI } },
    { test, { expr, EQ, expr } },
    { test, { expr, NE, expr } },
    { test, { expr, LT, expr } },
    { test, { expr, LE, expr } },
    { test, { expr, GE, expr } },
    { test, { expr, GT, expr } },
    { expr, { term } },
    { expr, { expr, PLUS, term } },
    { expr, { expr, MINUS, term } },
    { term, { factor } },
    { term, { term, STAR, factor } },
    { term, { term, SLASH, factor } },
    { term, { term, PCT, factor } },
    { factor, { ID } },
    { factor, { NUM } },
    { factor, { NULL_S } },
    { factor, { LPAREN, expr, RPAREN } },
    { factor, { AMP, lvalue } },
    { factor, { STAR, factor } },
    { factor, { NEW, INT, LBRACK, expr, RBRACK } },
    { factor, { ID, LPAREN, RPAREN } },
    { factor, { ID, LPAREN, arglist, RPAREN } },
    { arglist, { expr } },
    { arglist, { expr, COMMA, arglist } },
    { lvalue, { ID } },
    { lvalue, { STAR, factor } },
    { lvalue, { LPAREN, lvalue, RPAREN } }
};

Symbol EarleyState::nextSymbol() const {
    return rhs[dot];
}

bool EarleyState::isComplete() const {
    return dot == rhs.size();
}

void Parser::parse(State& globalState) {
    setupNullableRules();

#ifdef DEBUG
    addToChart({start, {procedures}}, 0, 0, 0, "initial");
#else
    addToChart({start, {procedures}}, 0, 0, 0);
#endif

    for (std::size_t i = 0; i < globalState.numTokens(); ++i) {
        if (i > _chart.size() - 1) {
            throw std::runtime_error("Invalid token " + globalState.getToken(i - 1).value );
        }

        for (std::size_t j = 0; j < _chart[i].size(); ++j) {
            auto& state = _chart[i][j];
#ifdef DEBUG
            std::cout << "Processing state list #" << i << std::endl
                      << "  EarleyState { " << std::endl
                      << "    lhs: " << symToStr[state.lhs] << std::endl
                      << "    rhs: ";
            for (auto sym : state.rhs) {
                std::cout << symToStr[sym] << " ";
            }
            std::cout << std::endl
                      << "    startIdx: " << state.startIdx << std::endl
                      << "    dot: " << state.dot << std::endl
                      << "  }" << std::endl;
#endif
            if (state.isComplete()) {
                completer(i);
            } else {
                const auto nextSym = state.nextSymbol();
                if (isTerminal(nextSym)) {
                    scanner(globalState.getToken(i).type, i);
                } else {
                    predictor(nextSym, i, j);
                }
            }
        }
    }

#ifdef DEBUG
    if (_chart.size() != globalState.numTokens() + 1) {
        std::cerr << "Produced chart is incomplete!" << std::endl;
    }

    std::cout << "Final chart is as follows:" << std::endl;
    for (std::size_t i = 0; i < _chart.size(); ++i) {
        std::cout << "EarleyState list #" << i << ": " << std::endl;
        for (const auto& state : _chart[i]) {
            std::cout << "  EarleyState { " << std::endl
                      << "    lhs: " << symToStr[state.lhs] << std::endl
                      << "    rhs: ";
            for (auto sym : state.rhs) {
                std::cout << symToStr[sym] << " ";
            }
            std::cout << std::endl
                      << "    startIdx: " << state.startIdx << std::endl
                      << "    dot: " << state.dot << std::endl
                      << "    op: " << state.op << std::endl
                      << "  }" << std::endl;
        }
    }
#endif
}

void Parser::setupNullableRules() {
    for (std::size_t i = 33; i <= 48; ++i) {
        _nullable[(Symbol)i] = false;
        auto itPair = CFG.equal_range((Symbol)i);
        for (auto it = itPair.first; it != itPair.second; ++it) {
            if (it->second.size() == 0) {
#ifdef DEBUG
                std::cout << symToStr[(Symbol)i] << " is nullable" << std::endl;
#endif
                _nullable[(Symbol)i] = true;
                break;
            }
        }
    }
}

bool Parser::existsInStateList(const EarleyState& state, std::size_t stateListIdx) {
    for (std::size_t i = 0; i < _chart[stateListIdx].size(); ++i) {
        auto& s = _chart[stateListIdx][i];
        if (s.lhs == state.lhs && std::equal(s.rhs.begin(), s.rhs.end(), state.rhs.begin(), state.rhs.end()) &&
            s.startIdx == state.startIdx && s.dot == state.dot) {
                return true;
        }
    }

    return false;
}

#ifdef DEBUG
void Parser::addToChart(std::pair<Symbol, std::vector<Symbol>> rule, std::size_t startIdx,
        std::size_t dot, std::size_t stateListIdx, const std::string& op) {
#else
void Parser::addToChart(std::pair<Symbol, std::vector<Symbol>> rule, std::size_t startIdx,
    std::size_t dot, std::size_t stateListIdx) {
#endif
#ifdef DEBUG
    std::cout << "addToChart():" << std::endl;
#endif

    if (stateListIdx == _chart.size()) {
        _chart.push_back(std::vector<EarleyState>());
    }

    EarleyState state;
    state.lhs = rule.first;
    state.rhs = rule.second;
    state.startIdx = startIdx;
    state.dot = dot;
#ifdef DEBUG
    state.op = op;
#endif

    if (!existsInStateList(state, stateListIdx)) {
        _chart[stateListIdx].push_back(state);
#ifdef DEBUG
        std::cout << "Pushed new state in state list #" << stateListIdx << std::endl
                << "    EarleyState { " << std::endl
                << "      lhs: " << symToStr[state.lhs] << std::endl
                << "      rhs: ";
        for (auto sym : state.rhs) {
            std::cout << symToStr[sym] << " ";
        }
        std::cout << std::endl
                << "      startIdx: " << state.startIdx << std::endl
                << "      dot: " << state.dot << std::endl
                << "    }" << std::endl;
#endif
    }
}

void Parser::predictor(Symbol nextSym, std::size_t k, std::size_t l) {
#ifdef DEBUG
    std::cout << "predictor for " << symToStr[nextSym] << std::endl;
#endif

    auto itPair = CFG.equal_range(nextSym);
    for (auto it = itPair.first; it != itPair.second; ++it) {
        auto rule = *it;
#ifdef DEBUG
        addToChart({nextSym, rule.second}, k, 0, k, "predictor");
#else
        addToChart({nextSym, rule.second}, k, 0, k);
#endif
        if (_nullable[nextSym]) {
#ifdef DEBUG
            addToChart({_chart[k][l].lhs, _chart[k][l].rhs}, _chart[k][l].startIdx,
                _chart[k][l].dot + 1, k, "predictor_nullable");
#else
            addToChart({_chart[k][l].lhs, _chart[k][l].rhs}, _chart[k][l].startIdx,
                _chart[k][l].dot + 1, k);
#endif
        }
    }
}

void Parser::scanner(Symbol nextSym, std::size_t k) {
#ifdef DEBUG
    std::cout << "scanner for " << symToStr[nextSym] << std::endl;
#endif

    for (const auto& state : _chart[k]) {
        if (!state.isComplete() && state.nextSymbol() == nextSym) {
#ifdef DEBUG
            addToChart({state.lhs, state.rhs}, state.startIdx, state.dot + 1, k + 1, "scanner");
#else
            addToChart({state.lhs, state.rhs}, state.startIdx, state.dot + 1, k + 1);
#endif
        }
    }
}

void Parser::completer(std::size_t k) {
#ifdef DEBUG
    std::cout << "completer for state list #" << k << std::endl;
#endif

    for (const auto& s1 : _chart[k]) {
        if (s1.isComplete()) {
            for (const auto& s2 : _chart[s1.startIdx]) {
                if (!s2.isComplete() && s2.nextSymbol() == s1.lhs) {
#ifdef DEBUG
                    addToChart({s2.lhs, s2.rhs}, s2.startIdx, s2.dot + 1, k, "completer");
#else
                    addToChart({s2.lhs, s2.rhs}, s2.startIdx, s2.dot + 1, k);
#endif
                }
            }
        }
    }
}

} // namespace wlp4
