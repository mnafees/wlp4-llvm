// Self
#include "parser.hpp"

// STL
#include <cassert>
#include <fstream>
#include <sstream>
#include <map>
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

// Context-free grammar taken from https://www.student.cs.uwaterloo.ca/~cs241/wlp4/WLP4.html
const std::multimap<Symbol, std::vector<Symbol>> CFG = {
    { start, { procedures } },
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

#ifdef DEBUG
std::map<Symbol, std::string> symToStr = {
    { ID, "ID" }, { NUM, "NUM" }, { LPAREN, "LPAREN" }, { RPAREN, "RPAREN" },
    { LBRACE, "LBRACE" }, { RBRACE, "RBRACE" }, { RETURN, "RETURN" }, { IF, "IF" },
    { ELSE, "ELSE" }, { WHILE, "WHILE" }, { PRINTLN, "PRINTLN" }, { WAIN, "WAIN" },
    { BECOMES, "BECOMES" }, { INT, "INT" }, { EQ, "EQ" }, { NE, "NE" }, { LT, "LT" },
    { GT, "GT" }, { LE, "LE" }, { GE, "GE" }, { PLUS, "PLUS" }, { MINUS, "MINUS" },
    { STAR, "STAR" }, { SLASH, "SLASH" }, { PCT, "PCT" }, { COMMA, "COMMA" },
    { SEMI, "SEMI" }, { NEW, "NEW" }, { DELETE, "DELETE" }, { LBRACK, "LBRACK" },
    { RBRACK, "RBRACK" }, { AMP, "AMP" }, { NULL_S, "NULL" }, { procedures, "procedures" },
    { procedure, "procedure" }, { main_s, "main" }, { params, "params" },
    { paramlist, "paramlist" }, { type, "type" }, { dcl, "dcl" }, { dcls, "dcls" },
    { statements, "statements" }, { lvalue, "lvalue" }, { expr, "expr" }, { test, "test" },
    { statement, "statement" }, { term, "term" }, { factor, "factor" }, { start, "start" },
    { arglist, "arglist" }
};
#endif

Elem::Elem(const Rule& rule, std::size_t startIdx, std::size_t dot)
    : _rule(rule), _startIdx(startIdx), _dot(dot) {}

const Rule& Elem::rule() const {
    return _rule;
}

std::size_t Elem::startIdx() const {
    return _startIdx;
}

std::size_t Elem::dot() const {
    return _dot;
}

bool Elem::operator==(const Elem& el) const {
    return &_rule == &el._rule && _startIdx == el._startIdx && _dot == el._dot;
}

Symbol Elem::nextSymbol() const {
    return _rule.second[_dot];
}

bool Elem::isComplete() const {
    return _dot == _rule.second.size();
}

void Parser::parse(State& globalState) {
    setupNullableRules();

#ifdef DEBUG
    addToChart(*CFG.find(start), 0, 0, 0, "initial");
#else
    addToChart(*CFG.find(start), 0, 0, 0);
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
                      << "    lhs: " << symToStr[el->rule().first] << std::endl
                      << "    rhs: ";
            for (auto sym : el->rule().second) {
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
        completer(_chart.size() - 1);
        // verifyCompleteChart(globalState);
    } else {
        throw std::runtime_error("Invalid WLP4 code");
    }

#ifdef DEBUG
    if (_chart.size() != globalState.numTokens() + 1) {
        std::cerr << "Produced chart is incomplete!" << std::endl;
    }

    std::cout << "Final chart is as follows:" << std::endl;
    for (std::size_t i = 0; i < _chart.size(); ++i) {
        std::cout << "Elem list #" << i << ": " << std::endl;
        for (const auto& el : _chart[i]) {
            if (!el->isComplete()) continue;
            std::cout << "  Elem {" << std::endl
                      << "    lhs: " << symToStr[el->rule().first] << std::endl
                      << "    rhs: ";
            for (auto sym : el->rule().second) {
                std::cout << symToStr[sym] << " ";
            }
            std::cout << std::endl
                      << "    startIdx: " << el->startIdx() << std::endl
                      << "    dot: " << el->dot() << std::endl
                      << "    op: " << el->op << std::endl
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

bool Parser::existsInStateList(const std::unique_ptr<Elem>& el, std::size_t stateListIdx) {
    for (const auto& s : _chart[stateListIdx]) {
        if (s == el) {
            return true;
        }
    }

    return false;
}

#ifdef DEBUG
void Parser::addToChart(const Rule& rule, std::size_t startIdx, std::size_t dot, std::size_t stateListIdx,
                        const std::string& op) {
#else
void Parser::addToChart(const Rule& rule, std::size_t startIdx, std::size_t dot, std::size_t stateListIdx) {
#endif
#ifdef DEBUG
    std::cout << "addToChart():" << std::endl;
#endif

    if (stateListIdx == _chart.size()) {
        _chart.push_back(std::vector<std::unique_ptr<Elem>>());
    }

    std::unique_ptr<Elem> el(new Elem(rule, startIdx, dot));
#ifdef DEBUG
    el->op = op;
#endif

    if (!existsInStateList(el, stateListIdx)) {
#ifdef DEBUG
        std::cout << "Pushed new state in state list #" << stateListIdx << std::endl
                  << "Elem {" << std::endl
                  << "  lhs: " << symToStr[el->rule().first] << std::endl
                  << "  rhs: ";
        for (auto sym : el->rule().second) {
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

void Parser::predictor(Symbol nextSym, std::size_t k, std::size_t l) {
#ifdef DEBUG
    std::cout << "predictor for " << symToStr[nextSym] << std::endl;
#endif

    auto itPair = CFG.equal_range(nextSym);
    for (auto it = itPair.first; it != itPair.second; ++it) {
        auto rule = *it;
#ifdef DEBUG
        addToChart(rule, k, 0, k, "predictor");
#else
        addToChart(rule, k, 0, k);
#endif
        if (_nullable[nextSym]) {
#ifdef DEBUG
            addToChart(_chart[k][l]->rule(), _chart[k][l]->startIdx(), _chart[k][l]->dot() + 1, k, "predictor_nullable");
#else
            addToChart(_chart[k][l]->rule(), _chart[k][l]->startIdx(), _chart[k][l]->dot() + 1, k);
#endif
        }
    }
}

void Parser::scanner(Symbol nextSym, std::size_t k) {
#ifdef DEBUG
    std::cout << "scanner for " << symToStr[nextSym] << std::endl;
#endif

    for (const auto& el : _chart[k]) {
        if (!el->isComplete() && el->nextSymbol() == nextSym) {
#ifdef DEBUG
            addToChart(el->rule(), el->startIdx(), el->dot() + 1, k + 1, "scanner");
#else
            addToChart(el->rule(), el->startIdx(), el->dot() + 1, k + 1);
#endif
        }
    }
}

void Parser::completer(std::size_t k) {
#ifdef DEBUG
    std::cout << "completer for state list #" << k << std::endl;
#endif

    for (std::size_t j = 0; j < _chart[k].size(); ++j) {
        if (_chart[k][j]->isComplete()) {
            for (std::size_t i = 0; i < _chart[_chart[k][j]->startIdx()].size(); ++i) {
                if (!_chart[_chart[k][j]->startIdx()][i]->isComplete() &&
                    _chart[_chart[k][j]->startIdx()][i]->nextSymbol() == _chart[k][j]->rule().first) {
#ifdef DEBUG
                    addToChart(_chart[_chart[k][j]->startIdx()][i]->rule(),
                        _chart[_chart[k][j]->startIdx()][i]->startIdx(),
                        _chart[_chart[k][j]->startIdx()][i]->dot() + 1, k, "completer");
#else
                    addToChart(_chart[_chart[k][j]->startIdx()][i]->rule(),
                        _chart[_chart[k][j]->startIdx()][i]->startIdx(),
                        _chart[_chart[k][j]->startIdx()][i]->dot() + 1, k);
#endif
                }
            }
        }
    }
}

void Parser::verifyCompleteChart(const State& globalState) {
    std::vector<std::string> _procedureNames;

    for (std::size_t i = _chart.size() - 1; i >= 0; --i) {
        for (const auto& el : _chart[i]) {
            if (!el->isComplete()) continue;

            if (el->rule().first == procedure) {
                assert(globalState.getToken(el->startIdx() + 1).type == ID);
                _procedureNames.push_back(globalState.getToken(el->startIdx() + 1).value);
            }
        }
    }
}

} // namespace wlp4
