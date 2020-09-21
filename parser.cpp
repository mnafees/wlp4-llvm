// Self
#include "parser.hpp"

// STL
#include <iostream>
#include <map>

// WLP4-LLVM
#include "state.hpp"
#include "ast/expr.hpp"
#include "ast/factor.hpp"
#include "ast/procedure.hpp"
#include "ast/term.hpp"

namespace wlp4 {

Parser::Parser(State& state) : _state(state) {}

void Parser::parse() {
    ast::Procedure* proc = nullptr;
    _elemIdx = _state.finalChart().size() - 1;
    for (; _elemIdx >= 0;) {
        const auto& elem = _state.finalChart()[_elemIdx];
#ifdef DEBUG
        std::cout << symToStr[CFG[elem->ruleIdx()].first] << std::endl;
#endif
        if (CFG[elem->ruleIdx()].first == Symbol::procedure || CFG[elem->ruleIdx()].first == Symbol::main_s) {
            for (auto sym : CFG[elem->ruleIdx()].second) {
                if (!isTerminal(sym)) {
                    _symbolStack.push(sym);
                }
            }
            proc = new ast::Procedure(_state.getToken(elem->startIdx() + 1).value);
            --_elemIdx;
        } else if (!_symbolStack.empty()) {
            auto sym = CFG[elem->ruleIdx()].first;
            if (sym == Symbol::expr) {
                proc->setReturnExpr(std::move(parseExpr()));
            }
        }
        if (_symbolStack.empty()) {
            _state.addProcedure(std::unique_ptr<ast::Procedure>(proc));
            proc = nullptr;
        }
    }
}

std::unique_ptr<ast::Expr> Parser::parseExpr() {
    if (_symbolStack.top() != Symbol::expr) {
        throw std::runtime_error("Symbol::expr expected to parse");
    } else if (CFG[_state.finalChart()[_elemIdx]->ruleIdx()].first != Symbol::expr) {
        throw std::runtime_error("Expected lhs Symbol::expr not found");
    }

    ast::Expr* expr = nullptr;
    const auto& rhs = CFG[_state.finalChart()[_elemIdx]->ruleIdx()].second;
    for (std::size_t i = 0; i < rhs.size(); ++i) {
        if (!isTerminal(rhs[i])) {
            _symbolStack.push(rhs[i]);
        }
    }
    --_elemIdx;
    expr = new ast::Expr(std::move(parseTerm()));
    if (rhs.size() > 1) {
        if (rhs[1] == Symbol::PLUS) {
            expr->setPlusWith(std::move(parseExpr()));
        } else if (rhs[1] == Symbol::MINUS) {
            expr->setMinusWith(std::move(parseExpr()));
        }
    }
    _symbolStack.pop();

    return std::unique_ptr<ast::Expr>(expr);
}

std::unique_ptr<ast::Term> Parser::parseTerm() {
    if (_symbolStack.top() != Symbol::term) {
        throw std::runtime_error("Symbol::term expected to parse");
    } else if (CFG[_state.finalChart()[_elemIdx]->ruleIdx()].first != Symbol::term) {
        throw std::runtime_error("Expected lhs Symbol::term not found");
    }

    ast::Term* term = nullptr;
    const auto& rhs = CFG[_state.finalChart()[_elemIdx]->ruleIdx()].second;
    for (std::size_t i = 0; i < rhs.size(); ++i) {
        if (!isTerminal(rhs[i])) {
            _symbolStack.push(rhs[i]);
        }
    }
    --_elemIdx;
    term = new ast::Term(std::move(parseFactor()));
    if (rhs.size() > 1) {
        if (rhs[1] == Symbol::STAR) {
            term->setStarWith(std::move(parseTerm()));
        } else if (rhs[1] == Symbol::SLASH) {
            term->setSlashWith(std::move(parseTerm()));
        } else if (rhs[1] == Symbol::PCT) {
            term->setPctWith(std::move(parseTerm()));
        }
    }
    _symbolStack.pop();

    return std::unique_ptr<ast::Term>(term);
}

std::unique_ptr<ast::Factor> Parser::parseFactor() {
    if (_symbolStack.top() != Symbol::factor) {
        throw std::runtime_error("Symbol::factor expected to parse");
    } else if (CFG[_state.finalChart()[_elemIdx]->ruleIdx()].first != Symbol::factor) {
        throw std::runtime_error("Expected lhs Symbol::factor not found");
    }

    ast::Factor* factor = nullptr;
    const auto& rhs = CFG[_state.finalChart()[_elemIdx]->ruleIdx()].second;
    if (rhs.size() > 1) {
        for (std::size_t i = 0; i < rhs.size(); ++i) {
            if (!isTerminal(rhs[i])) {
                _symbolStack.push(rhs[i]);
            }
        }

    } else {
        if (rhs[0] == Symbol::ID) {
            factor = new ast::Factor(_state.getToken(_state.finalChart()[_elemIdx]->startIdx()).value);
        } else if (rhs[0] == Symbol::NUM) {

        } else if (rhs[0] == Symbol::NULL_S) {

        }
    }
    _symbolStack.pop();

    return std::unique_ptr<ast::Factor>(factor);
}

} // namespace wlp4
