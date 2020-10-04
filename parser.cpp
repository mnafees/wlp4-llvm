// Self
#include "parser.hpp"

// STL
#ifdef DEBUG
#include <iostream>
#endif
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

    _elemIdx = State::instance().finalChart().size() - 1;
    Symbol lhs;
    // first we need to populate the symbol table for all procedures
    for (; _elemIdx >= 0;) {
        lhs = CFG[State::instance().finalChart().at(_elemIdx)->ruleIdx()].first;
        if (lhs == Symbol::procedure || lhs == Symbol::main_s) {
            const auto nameIdx = State::instance().finalChart().at(_elemIdx)->startIdx() + 1;
            _currProcedureName = State::instance().getToken(nameIdx).value;
            --_elemIdx;
        } else if (lhs == Symbol::dcls) {
            for (;;) {
                if (auto opt = parseDcls(); opt.has_value()) {
                    auto dcl = parseDcl();
#ifdef DEBUG
                    if (dcl->type() == ast::DclType::INT) {
                        std::cout << _currProcedureName << " has declaration '" << dcl->id() << "' of type INT\n";
                    } else if (dcl->type() == ast::DclType::INT_STAR) {
                        std::cout << _currProcedureName << " has declaration '" << dcl->id() << "' of type INT_STAR\n";
                    }
#endif
                    State::instance().addDclToProc(_currProcedureName, dcl->id(), dcl->type());
                } else {
                    break;
                }
            }
        } else if (lhs == Symbol::dcl && _currProcedureName == "wain") {
            auto dcl2 = parseDcl();
            if (dcl2->type() != ast::DclType::INT) {
                throw std::runtime_error("the second parameter for wain() needs to be an int");
            }
            auto dcl1 = parseDcl();
            State::instance().addDclToProc(_currProcedureName, dcl1->id(), dcl1->type());
            State::instance().addDclToProc(_currProcedureName, dcl2->id(), dcl2->type());
        } else {
            --_elemIdx;
        }
    }

    std::unique_ptr<ast::Procedure> proc;
    _elemIdx = State::instance().finalChart().size() - 1;
    for (;;) {
        lhs = CFG[State::instance().finalChart().at(_elemIdx)->ruleIdx()].first;
        if (lhs == Symbol::procedure || lhs == Symbol::main_s) {
            if (proc) {
                State::instance().addProcedure(std::move(proc));
                proc = nullptr;
            }
            const auto nameIdx = State::instance().finalChart().at(_elemIdx)->startIdx() + 1;
            proc = std::make_unique<ast::Procedure>(State::instance().getToken(nameIdx).value);
            _currProcedureName = proc->name();
            --_elemIdx;
        } else if (lhs == Symbol::expr) {
            auto expr = parseExpr();
            if (expr->type() != ast::DclType::INT) {
                throw std::runtime_error("return value in procedure "s + proc->name() + " should be of type int");
            }
            proc->setReturnExpr(std::move(expr));
        } else if (lhs == Symbol::statements) {
            while (!parseStatements()) {
                proc->addStatement(std::move(parseStatement()));
            }
        } else if (lhs == Symbol::dcls) {
            for (;;) {
                if (auto opt = parseDcls(); opt.has_value()) {
                    auto dcl = parseDcl(&opt.value());
#ifdef DEBUG
                    std::cout << "Value of dcl '" << dcl->id() << "' is '" << dcl->value() << "'\n";
#endif
                    State::instance().addDclToProc(proc->name(), dcl->id(), dcl->type());
                    proc->addDeclaration(std::move(dcl));
                } else {
                    break;
                }
            }
        } else if (lhs == Symbol::dcl && proc->isWain()) {
            auto dcl2 = parseDcl();
            if (dcl2->type() != ast::DclType::INT) {
                throw std::runtime_error("the second parameter for wain() needs to be an int");
            }
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
    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
    --_elemIdx;
    std::unique_ptr<ast::Expr> expr(new ast::Expr(std::move(parseTerm())));
    if (rhs.size() > 1) {
        if (rhs[1] == Symbol::PLUS) {
            expr->setPlusWith(std::move(parseExpr()));
        } else if (rhs[1] == Symbol::MINUS) {
            expr->setMinusWith(std::move(parseExpr()));
        }
    }
    if (expr->type() == ast::DclType::INVALID) {
        throw std::runtime_error("invalid expr generated");
    }

    return std::move(expr);
}

std::unique_ptr<ast::Term> Parser::parseTerm() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    gotoCorrectRule(Symbol::term, "Symbol::term"s);
    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
    --_elemIdx;
    auto factor = parseFactor();
    std::unique_ptr<ast::Term> term(new ast::Term(std::move(factor)));
    if (rhs.size() > 1) {
        auto rightTerm = parseTerm();
        if (rhs[1] == Symbol::STAR) {
            term->setStarWith(std::move(rightTerm));
        } else if (rhs[1] == Symbol::SLASH) {
            term->setSlashWith(std::move(rightTerm));
        } else if (rhs[1] == Symbol::PCT) {
            term->setPctWith(std::move(rightTerm));
        }
        if (term->type() == ast::DclType::INVALID) {
            throw std::runtime_error("both int values expected");
        }
    }

    return std::move(term);
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
                auto lvalue = parseLvalue();
                if (lvalue->type() != ast::DclType::INT) {
                    throw std::runtime_error("int value expected after &");
                }
                factor->setValue(std::move(lvalue));
            } else if (rhs[0] == Symbol::STAR) {
                auto rightFactor = parseFactor();
                if (rightFactor->type() != ast::DclType::INT_STAR) {
                    throw std::runtime_error("int* value expected after *");
                }
                factor->setValue(std::move(rightFactor));
            } else if (rhs[0] == Symbol::NEW) {
                auto expr = parseExpr();
                if (expr->type() != ast::DclType::INT) {
                    throw std::runtime_error("int value expected after [");
                }
                factor->setValue(std::move(expr));
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
            auto factor = parseFactor();
            if (factor->type() != ast::DclType::INT_STAR) {
                throw std::runtime_error("int* value expected after *");
            }
            lvalue->setValue(std::move(factor));
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
    std::unique_ptr<ast::Statement> stmt;
    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
    --_elemIdx;
    if (rhs[0] == Symbol::lvalue) {
        std::unique_ptr<ast::LvalueStatement> lvalueStmt(new ast::LvalueStatement);
        auto expr = parseExpr();
        auto lvalue = parseLvalue();
        if (expr->type() != lvalue->type()) {
            throw std::runtime_error("assigning value of invalid type");
        }
        lvalueStmt->setExpr(std::move(expr));
        lvalueStmt->setLvalue(std::move(lvalue));
        stmt = std::move(lvalueStmt);
    } else if (rhs[0] == Symbol::IF) {
        std::unique_ptr<ast::IfStatement> ifStmt(new ast::IfStatement);
        while (!parseStatements()) {
            ifStmt->addFalseStatement(std::move(parseStatement()));
        }
        while (!parseStatements()) {
            ifStmt->addTrueStatement(std::move(parseStatement()));
        }
        ifStmt->setTest(std::move(parseTest()));
        stmt = std::move(ifStmt);
    } else if (rhs[0] == Symbol::WHILE) {
        std::unique_ptr<ast::WhileStatement> whileStmt(new ast::WhileStatement);
        while (!parseStatements()) {
            whileStmt->addStatement(std::move(parseStatement()));
        }
        whileStmt->setTest(std::move(parseTest()));
        stmt = std::move(whileStmt);
    } else if (rhs[0] == Symbol::PRINTLN) {
        std::unique_ptr<ast::PrintlnStatement> printStmt(new ast::PrintlnStatement);
        auto expr = parseExpr();
        if (expr->type() != ast::DclType::INT) {
            throw std::runtime_error("println can only accept int values");
        }
        printStmt->setExpr(std::move(expr));
        stmt = std::move(printStmt);
    } else if (rhs[0] == Symbol::DELETE) {
        std::unique_ptr<ast::DeleteStatement> delStmt(new ast::DeleteStatement);
        auto expr = parseExpr();
        if (expr->type() != ast::DclType::INT_STAR) {
            throw std::runtime_error("delete[] should be used only with int* values");
        }
        delStmt->setExpr(std::move(expr));
        stmt = std::move(delStmt);
    }

    return std::move(stmt);
}

std::unique_ptr<ast::Test> Parser::parseTest() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    gotoCorrectRule(Symbol::test, "Symbol::test"s);
    std::unique_ptr<ast::Test> test(new ast::Test);
    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
    --_elemIdx;
    auto rightExpr = parseExpr();
    auto leftExpr = parseExpr();
    if (rightExpr->type() != leftExpr->type()) {
        throw std::runtime_error("invalid types being tested");
    }
    test->setRightExpr(std::move(rightExpr));
    test->setLeftExpr(std::move(leftExpr));
    test->setOp(rhs[1]);

    return std::move(test);
}

std::optional<Symbol> Parser::parseDcls() {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    gotoCorrectRule(Symbol::dcls, "Symbol::dcls"s);
    const auto& rhs = CFG[State::instance().finalChart().at(_elemIdx)->ruleIdx()].second;
    --_elemIdx;
    if (!rhs.empty()) {
        return rhs[3];
    }

    return {};
}

std::unique_ptr<ast::Dcl> Parser::parseDcl(Symbol *sym) {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    gotoCorrectRule(Symbol::dcl, "Symbol::dcl"s);
    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
    std::unique_ptr<ast::Dcl> dcl(new ast::Dcl);
    if (sym) {
        if (*sym == Symbol::NUM) {
            dcl->setValue(State::instance().getToken(State::instance().finalChart().at(_elemIdx)->startIdx() + 3).value);
        } else {
            dcl->setValue(State::instance().getToken(State::instance().finalChart().at(_elemIdx)->startIdx() + 4).value);
        }
    }
    --_elemIdx;
    if (CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second.size() == 1) {
        dcl->setId(State::instance().getToken(State::instance().finalChart()[_elemIdx]->startIdx() + 1).value);
        dcl->setType(ast::DclType::INT);
    } else {
        dcl->setId(State::instance().getToken(State::instance().finalChart()[_elemIdx]->startIdx() + 2).value);
        dcl->setType(ast::DclType::INT_STAR);
    }
    --_elemIdx;

    return std::move(dcl);
}

} // namespace wlp4
