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
#include "ast/lvalue.hpp"
#include "ast/arglist.hpp"
#include "ast/statement.hpp"
#include "ast/test.hpp"
#include "ast/dcl.hpp"
#include "ast/type.hpp"

using namespace std::string_literals;

namespace wlp4 {

void Parser::parse() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    std::unique_ptr<ast::Procedure> proc(nullptr);
    _elemIdx = State::instance().finalChart().size() - 1;
    Symbol lhs;
    for (;;) {
        lhs = CFG[State::instance().finalChart().at(_elemIdx)->ruleIdx()].first;
        if (lhs == Symbol::procedure || lhs == Symbol::main_s) {
            if (proc) {
                State::instance().addProcedure(std::move(proc));
                proc = nullptr;
            }
            const auto nameIdx = State::instance().finalChart().at(_elemIdx)->startIdx() + 1;
            proc = std::make_unique<ast::Procedure>(std::move(State::instance().getToken(nameIdx).value));
            _currProcedureName = proc->name();
            --_elemIdx;
        } else if (lhs == Symbol::expr) {
            proc->setReturnExpr(std::move(parseExpr()));
        } else if (lhs == Symbol::statements) {
            while (!parseStatements()) {
                proc->addStatement(std::move(parseStatement()));
            }
        } else if (lhs == Symbol::dcls) {
            for (;;) {
                if (auto [cond, opt] = parseDcls(); !cond) {
                    auto dcl = parseDcl();
                    if (opt.has_value()) {
                        dcl->setValue(opt.value());
                    }
                    State::instance().addDclToProc(proc->name(), dcl->id(), dcl->type());
                    proc->addDeclaration(std::move(dcl));
                } else {
                    break;
                }
            }
        } else if (lhs == Symbol::dcl && proc->isWain()) {
            auto dcl2 = parseDcl();
            auto dcl1 = parseDcl();
            proc->addParam(std::move(dcl1));
            proc->addParam(std::move(dcl2));
        } else if (lhs == Symbol::params) {

        }
        if (_elemIdx < 0) {
            State::instance().addProcedure(std::move(proc));
            break;
        }
    }
}

void Parser::gotoCorrectRule(Symbol lhs, const std::string& name) {
    while (_elemIdx >= 0 && CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].first != lhs) {
        --_elemIdx;
    }
    if (_elemIdx < 0) {
        throw std::runtime_error("Could not find required "s + name);
    }
}

std::unique_ptr<ast::Expr> Parser::parseExpr() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    gotoCorrectRule(Symbol::expr, "Symbol::expr"s);
    ast::Expr* expr = nullptr;
    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
    --_elemIdx;
    expr = new ast::Expr(std::move(parseTerm()));
    if (rhs.size() > 1) {
        if (rhs[1] == Symbol::PLUS) {
            expr->setPlusWith(std::move(parseExpr()));
        } else if (rhs[1] == Symbol::MINUS) {
            expr->setMinusWith(std::move(parseExpr()));
        }
    }

    return std::unique_ptr<ast::Expr>(expr);
}

std::unique_ptr<ast::Term> Parser::parseTerm() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    gotoCorrectRule(Symbol::term, "Symbol::term"s);
    ast::Term* term = nullptr;
    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
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

    return std::unique_ptr<ast::Term>(term);
}

std::unique_ptr<ast::Factor> Parser::parseFactor() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    gotoCorrectRule(Symbol::factor, "Symbol::factor"s);
    std::unique_ptr<ast::Factor> factor(new ast::Factor(_currProcedureName));
    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
    if (rhs.size() > 1) {
        if (rhs[0] == Symbol::ID) {
            if (rhs.size() == 3) {
                // factor -> ID LPAREN RPAREN
                factor->setProcedureCall(State::instance().getToken(
                    State::instance().finalChart()[_elemIdx]->startIdx()).value);
                --_elemIdx;
            } else {
                // FIXME: Really bad design here!
                const auto& procName = State::instance().getToken(State::instance().finalChart()[_elemIdx]->startIdx()).value;
                --_elemIdx;
                factor->setValue(std::move(parseArglist()));
                factor->setProcedureCall(procName);
            }
        } else {
            --_elemIdx;
            if (rhs[0] == Symbol::LPAREN) {
                // factor -> LPAREN expr RPAREN
                factor->setValue(std::move(parseExpr()));
                factor->setParenExpr();
            } else if (rhs[0] == Symbol::AMP) {
                factor->setValue(std::move(parseLvalue()));
            } else if (rhs[0] == Symbol::STAR) {
                factor->setValue(std::move(parseFactor()));
            } else if (rhs[0] == Symbol::NEW) {
                factor->setValue(std::move(parseExpr()));
            }
        }
    } else {
        if (rhs[0] == Symbol::ID) {
            factor->setValue(State::instance().getToken(State::instance().finalChart()[_elemIdx]->startIdx()).value);
        } else if (rhs[0] == Symbol::NUM) {
            unsigned int num = std::stoi(State::instance().getToken(State::instance().finalChart()[_elemIdx]->startIdx()).value);
            factor->setValue(num);
        } else if (rhs[0] == Symbol::NULL_S) {
            factor->setValue(ast::NullType());
        }
        --_elemIdx;
    }

    return std::move(factor);
}

std::unique_ptr<ast::Lvalue> Parser::parseLvalue() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    gotoCorrectRule(Symbol::lvalue, "Symbol::lvalue"s);
    std::unique_ptr<ast::Lvalue> lvalue(new ast::Lvalue(_currProcedureName));
    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
    if (rhs[0] == Symbol::ID) {
        lvalue->setValue(State::instance().getToken(State::instance().finalChart()[_elemIdx]->startIdx()).value);
        --_elemIdx;
    } else {
        --_elemIdx;
        if (rhs[0] == Symbol::STAR) {
            lvalue->setValue(std::move(parseFactor()));
        } else if (rhs[0] == Symbol::LPAREN) {
            lvalue->setValue(std::move(parseLvalue()));
        }
    }

    return std::move(lvalue);
}

std::unique_ptr<ast::Arglist> Parser::parseArglist() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    gotoCorrectRule(Symbol::arglist, "Symbol::arglist"s);
    ast::Arglist* arglist = nullptr;
    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
    --_elemIdx;
    if (rhs.size() == 1) {
        arglist = new ast::Arglist(std::move(parseExpr()));
    } else {
        arglist = new ast::Arglist(std::move(parseArglist()));
        arglist->setExpr(std::move(parseExpr()));
    }

    return std::unique_ptr<ast::Arglist>(arglist);
}

bool Parser::parseStatements() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    gotoCorrectRule(Symbol::statements, "Symbol::statements"s);
    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
    --_elemIdx;

    return rhs.empty();
}

std::unique_ptr<ast::Statement> Parser::parseStatement() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    gotoCorrectRule(Symbol::statement, "Symbol::statement"s);
    ast::Statement* stmt = nullptr;
    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
    --_elemIdx;
    if (rhs[0] == Symbol::lvalue) {
        ast::LvalueStatement* lvalueStmt = new ast::LvalueStatement;
        lvalueStmt->setExpr(std::move(parseExpr()));
        lvalueStmt->setLvalue(std::move(parseLvalue()));
        stmt = lvalueStmt;
    } else if (rhs[0] == Symbol::IF) {
        auto ifStmt = new ast::IfStatement;
        while (!parseStatements()) {
            ifStmt->addFalseStatement(std::move(parseStatement()));
        }
        while (!parseStatements()) {
            ifStmt->addTrueStatement(std::move(parseStatement()));
        }
        ifStmt->setTest(std::move(parseTest()));
        stmt = ifStmt;
    } else if (rhs[0] == Symbol::WHILE) {
        auto whileStmt = new ast::WhileStatement;
        while (!parseStatements()) {
            whileStmt->addStatement(std::move(parseStatement()));
        }
        whileStmt->setTest(std::move(parseTest()));
        stmt = whileStmt;
    } else if (rhs[0] == Symbol::PRINTLN) {
        ast::PrintlnStatement* printStmt = new ast::PrintlnStatement;
        printStmt->setExpr(std::move(parseExpr()));
        stmt = printStmt;
    } else if (rhs[0] == Symbol::DELETE) {
        ast::DeleteStatement* delStmt = new ast::DeleteStatement;
        delStmt->setExpr(std::move(parseExpr()));
        stmt = delStmt;
    }

    return std::unique_ptr<ast::Statement>(stmt);
}

std::unique_ptr<ast::Test> Parser::parseTest() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    gotoCorrectRule(Symbol::test, "Symbol::test"s);
    auto test = new ast::Test;
    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
    --_elemIdx;
    test->setRightExpr(std::move(parseExpr()));
    test->setLeftExpr(std::move(parseExpr()));
    test->setOp(rhs[1]);

    return std::unique_ptr<ast::Test>(test);
}

std::pair<bool, std::optional<std::string>> Parser::parseDcls() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    gotoCorrectRule(Symbol::dcls, "Symbol::dcls"s);
    const auto& elem = State::instance().finalChart().at(_elemIdx);
    const auto& rhs = CFG[elem->ruleIdx()].second;
    --_elemIdx;
    if (rhs.empty()) {
        return { rhs.empty(), {} };
    } else if (rhs[3] == Symbol::NUM) {
        return { rhs.empty(), State::instance().getToken(elem->startIdx() + 3).value };
    }

    return { rhs.empty(), State::instance().getToken(elem->startIdx() + 4).value };
}

std::unique_ptr<ast::Dcl> Parser::parseDcl() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    gotoCorrectRule(Symbol::dcl, "Symbol::dcl"s);
    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
    auto dcl = new ast::Dcl;
    --_elemIdx;
    if (CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second.size() == 1) {
        dcl->setId(State::instance().getToken(State::instance().finalChart()[_elemIdx]->startIdx() + 1).value);
        dcl->setType(ast::DclType::INT);
    } else {
        dcl->setId(State::instance().getToken(State::instance().finalChart()[_elemIdx]->startIdx() + 2).value);
        dcl->setType(ast::DclType::INT_STAR);
    }
    --_elemIdx;

    return std::unique_ptr<ast::Dcl>(dcl);
}

} // namespace wlp4
