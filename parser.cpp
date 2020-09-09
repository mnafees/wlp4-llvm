// Self
#include "parser.hpp"

// STL
#include <array>
#include <fstream>
#include <sstream>
#include <stack>
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
const std::vector<std::pair<Symbol, std::vector<Symbol>>> CFG = {
    { Symbol::start, { Symbol::procedures } },
    { Symbol::procedures, { Symbol::procedure, Symbol::procedures } },
    { Symbol::procedures, { Symbol::main_s } },
    { Symbol::procedure, { Symbol::INT, Symbol::ID, Symbol::LPAREN, Symbol::params, Symbol::RPAREN,
        Symbol::LBRACE, Symbol::dcls, Symbol::statements, Symbol::RETURN, Symbol::expr, Symbol::SEMI,
        Symbol::RBRACE } },
    { Symbol::main_s, { Symbol::INT, Symbol::WAIN, Symbol::LPAREN, Symbol::dcl, Symbol::COMMA,
        Symbol::dcl, Symbol::RPAREN, Symbol::LBRACE, Symbol::dcls, Symbol::statements, Symbol::RETURN,
        Symbol::expr, Symbol::SEMI, Symbol::RBRACE } },
    { Symbol::params, {} },
    { Symbol::params, { Symbol::paramlist } },
    { Symbol::paramlist, { Symbol::dcl } },
    { Symbol::paramlist, { Symbol::dcl, Symbol::COMMA, Symbol::paramlist } },
    { Symbol::type, { Symbol::INT } },
    { Symbol::type, { Symbol::INT, Symbol::STAR } },
    { Symbol::dcls, {} },
    { Symbol::dcls, { Symbol::dcls, Symbol::dcl, Symbol::BECOMES, Symbol::NUM, Symbol::SEMI } },
    { Symbol::dcls, { Symbol::dcls, Symbol::dcl, Symbol::BECOMES, Symbol::NULL_S, Symbol::SEMI } },
    { Symbol::dcl, { Symbol::type, Symbol::ID } },
    { Symbol::statements, {} },
    { Symbol::statements, { Symbol::statements, Symbol::statement } },
    { Symbol::statement, { Symbol::lvalue, Symbol::BECOMES, Symbol::expr, Symbol::SEMI } },
    { Symbol::statement, { Symbol::IF, Symbol::LPAREN, Symbol::test, Symbol::RPAREN, Symbol::LBRACE,
        Symbol::statements, Symbol::RBRACE, Symbol::ELSE, Symbol::LBRACE, Symbol::statements,
        Symbol::RBRACE } },
    { Symbol::statement, { Symbol::WHILE, Symbol::LPAREN, Symbol::test, Symbol::RPAREN, Symbol::LBRACE,
        Symbol::statements, Symbol::RBRACE } },
    { Symbol::statement, { Symbol::PRINTLN, Symbol::LPAREN, Symbol::expr, Symbol::RPAREN, Symbol::SEMI } },
    { Symbol::statement, { Symbol::DELETE, Symbol::LBRACK, Symbol::RBRACK, Symbol::expr, Symbol::SEMI } },
    { Symbol::test, { Symbol::expr, Symbol::EQ, Symbol::expr } },
    { Symbol::test, { Symbol::expr, Symbol::NE, Symbol::expr } },
    { Symbol::test, { Symbol::expr, Symbol::LT, Symbol::expr } },
    { Symbol::test, { Symbol::expr, Symbol::LE, Symbol::expr } },
    { Symbol::test, { Symbol::expr, Symbol::GE, Symbol::expr } },
    { Symbol::test, { Symbol::expr, Symbol::GT, Symbol::expr } },
    { Symbol::expr, { Symbol::term } },
    { Symbol::expr, { Symbol::expr, Symbol::PLUS, Symbol::term } },
    { Symbol::expr, { Symbol::expr, Symbol::MINUS, Symbol::term } },
    { Symbol::term, { Symbol::factor } },
    { Symbol::term, { Symbol::term, Symbol::STAR, Symbol::factor } },
    { Symbol::term, { Symbol::term, Symbol::SLASH, Symbol::factor } },
    { Symbol::term, { Symbol::term, Symbol::PCT, Symbol::factor } },
    { Symbol::factor, { Symbol::ID } },
    { Symbol::factor, { Symbol::NUM } },
    { Symbol::factor, { Symbol::NULL_S } },
    { Symbol::factor, { Symbol::LPAREN, Symbol::expr, Symbol::RPAREN } },
    { Symbol::factor, { Symbol::AMP, Symbol::lvalue } },
    { Symbol::factor, { Symbol::STAR, Symbol::factor } },
    { Symbol::factor, { Symbol::NEW, Symbol::INT, Symbol::LBRACK, Symbol::expr, Symbol::RBRACK } },
    { Symbol::factor, { Symbol::ID, Symbol::LPAREN, Symbol::RPAREN } },
    { Symbol::factor, { Symbol::ID, Symbol::LPAREN, Symbol::arglist, Symbol::RPAREN } },
    { Symbol::arglist, { Symbol::expr } },
    { Symbol::arglist, { Symbol::expr, Symbol::COMMA, Symbol::arglist } },
    { Symbol::lvalue, { Symbol::ID } },
    { Symbol::lvalue, { Symbol::STAR, Symbol::factor } },
    { Symbol::lvalue, { Symbol::LPAREN, Symbol::lvalue, Symbol::RPAREN } }
};

#ifdef DEBUG
std::map<Symbol, std::string> symToStr = {
    { Symbol::ID, "ID" }, { Symbol::NUM, "NUM" }, { Symbol::LPAREN, "LPAREN" }, { Symbol::RPAREN, "RPAREN" },
    { Symbol::LBRACE, "LBRACE" }, { Symbol::RBRACE, "RBRACE" }, { Symbol::RETURN, "RETURN" }, { Symbol::IF, "IF" },
    { Symbol::ELSE, "ELSE" }, { Symbol::WHILE, "WHILE" }, { Symbol::PRINTLN, "PRINTLN" }, { Symbol::WAIN, "WAIN" },
    { Symbol::BECOMES, "BECOMES" }, { Symbol::INT, "INT" }, { Symbol::EQ, "EQ" }, { Symbol::NE, "NE" },
    { Symbol::LT, "LT" }, { Symbol::GT, "GT" }, { Symbol::LE, "LE" }, { Symbol::GE, "GE" }, { Symbol::PLUS, "PLUS" },
    { Symbol::MINUS, "MINUS" }, { Symbol::STAR, "STAR" }, { Symbol::SLASH, "SLASH" }, { Symbol::PCT, "PCT" },
    { Symbol::COMMA, "COMMA" }, { Symbol::SEMI, "SEMI" }, { Symbol::NEW, "NEW" }, { Symbol::DELETE, "DELETE" },
    { Symbol::LBRACK, "LBRACK" }, { Symbol::RBRACK, "RBRACK" }, { Symbol::AMP, "AMP" }, { Symbol::NULL_S, "NULL" },
    { Symbol::procedures, "procedures" }, { Symbol::procedure, "procedure" }, { Symbol::main_s, "main" },
    { Symbol::params, "params" }, { Symbol::paramlist, "paramlist" }, { Symbol::type, "type" }, { Symbol::dcl, "dcl" },
    { Symbol::dcls, "dcls" }, { Symbol::statements, "statements" }, { Symbol::lvalue, "lvalue" }, { Symbol::expr, "expr" },
    { Symbol::test, "test" }, { Symbol::statement, "statement" }, { Symbol::term, "term" }, { Symbol::factor, "factor" },
    { Symbol::start, "start" }, { Symbol::arglist, "arglist" }
};
#endif

Elem::Elem(std::size_t ruleIdx, std::size_t startIdx, std::size_t dot)
    : _ruleIdx(ruleIdx), _startIdx(startIdx), _dot(dot) {}

std::size_t Elem::ruleIdx() const {
    return _ruleIdx;
}

std::size_t Elem::startIdx() const {
    return _startIdx;
}

std::size_t Elem::dot() const {
    return _dot;
}

Symbol Elem::nextSymbol() const {
    return CFG[_ruleIdx].second[_dot];
}

bool Elem::isComplete() const {
    return _dot == CFG[_ruleIdx].second.size();
}

void Parser::parse(State& globalState) {
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
        populateProcedureNames(globalState);
        cleanupChart();
        // generateAST(globalState);
    } else {
        throw std::runtime_error("Invalid WLP4 code");
    }
}

void Parser::setupNullableRules() {
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

bool Parser::existsInStateList(const std::unique_ptr<Elem>& el, std::size_t stateListIdx) {
    for (const auto& s : _chart[stateListIdx]) {
        if (s->ruleIdx() == el->ruleIdx() && s->startIdx() == el->startIdx() && s->dot() == el->dot()) {
            return true;
        }
    }

    return false;
}

#ifdef DEBUG
void Parser::addToChart(std::size_t ruleIdx, std::size_t startIdx, std::size_t dot, std::size_t stateListIdx,
                        const std::string& op) {
#else
void Parser::addToChart(std::size_t ruleIdx, std::size_t startIdx, std::size_t dot, std::size_t stateListIdx) {
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

void Parser::predictor(Symbol nextSym, std::size_t k, std::size_t l) {
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

void Parser::scanner(Symbol nextSym, std::size_t k) {
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

void Parser::completer(std::size_t k) {
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

void Parser::populateProcedureNames(const State& globalState) {
    for (std::size_t i = _chart.size() - 1; i > 0;) {
        for (const auto& el : _chart[i]) {
            if (CFG[el->ruleIdx()].first == Symbol::procedure || CFG[el->ruleIdx()].first == Symbol::main_s) {
                _procedureNames.emplace_front(globalState.getToken(el->startIdx() + 1).value);
                i = el->startIdx();
                break;
            }
        }
    }
}

void Parser::cleanupChart() {
    // Iterate and keep only complete elems and elem lists
    for (auto it = _chart.begin(); it != _chart.end();) {
        for (auto elit = it->begin(); elit != it->end();) {
            if (!(*elit)->isComplete()) {
                elit = it->erase(elit);
            } else {
                ++elit;
            }
        }
        if (it->empty()) {
            it = _chart.erase(it);
        } else {
            ++it;
        }
    }

#ifdef DEBUG
    std::cout << "Cleaned up chart is as follows:" << std::endl;
    for (std::size_t i = 0; i < _chart.size(); ++i) {
        std::cout << "Elem list #" << i << ": " << std::endl;
        for (const auto& el : _chart[i]) {
            std::cout << "  Elem {" << std::endl
                      << "    lhs: " << symToStr[CFG[el->ruleIdx()].first] << std::endl
                      << "    rhs: ";
            for (auto sym : CFG[el->ruleIdx()].second) {
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

void Parser::generateAST(State& globalState) {
    std::stack<Symbol> nonTerminals;
    ast::Procedure* proc = nullptr;
    for (auto i = _chart.size() - 1; i > 0;) {
        for (const auto& el : _chart[i]) {
            if (CFG[el->ruleIdx()].first == Symbol::procedure || CFG[el->ruleIdx()].first == Symbol::main_s) {
                for (const auto& sym : CFG[el->ruleIdx()].second) {
                    if (!isTerminal(sym)) {
                        nonTerminals.push(sym);
                    }
                }
                proc = new ast::Procedure(globalState.getToken(el->startIdx() + 1).value);
            } else if (!nonTerminals.empty()) {
                auto sym = nonTerminals.top();
                if (sym == Symbol::expr) {
                    auto [j, parsedExpr] = parseExpr(i);
                    proc->setReturnExpr(std::move(parsedExpr));
                    i = j;
                } else if (sym == Symbol::statements) {

                } else if (sym == Symbol::dcls) {

                } else if (sym == Symbol::dcl) {
                    assert(proc->isWain());
                } else if (sym == Symbol::params) {
                    assert(!proc->isWain());
                } else {
                    // this should never occur
                    throw std::runtime_error("Non-terminal stack seems to be corrupted");
                }
                nonTerminals.pop();
            }
        }
        if (nonTerminals.empty() && proc) {
            globalState.addProcedure(std::unique_ptr<ast::Procedure>(proc));
            proc = nullptr;
        }
    }
}

std::pair<std::size_t, std::unique_ptr<ast::Expr>> Parser::parseExpr(std::size_t currElemListIdx) {
    assert(currElemListIdx > 0 && currElemListIdx < _chart.size());

    auto i = currElemListIdx;
    ast::Expr* parsedExpr = nullptr;
    for (; i > 0; --i) {
        for (const auto& el : _chart[i]) {

        }
    }

    return std::make_pair(i, std::unique_ptr<ast::Expr>(parsedExpr));
}

} // namespace wlp4
