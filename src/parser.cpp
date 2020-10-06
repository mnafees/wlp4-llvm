// Self
#include "parser.hpp"

// STL
#ifdef DEBUG
#include <iostream>
#endif

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

#ifdef DEBUG
#define PRINT_FUNC std::cout << __PRETTY_FUNCTION__ << '\n'
#else
#define PRINT_FUNC
#endif

#define FETCH_RULE_RHS const auto& rhs = CFG[STATE.finalChart().at(_elemIdx)->ruleIdx()].second
#define PARSE_BEGIN(ast, sym) \
    ast##Ptr Parser::parse##ast() { \
    PRINT_FUNC; \
    gotoCorrectRule(sym, #sym##s); \
    FETCH_RULE_RHS;
#define PARSE_END }

namespace wlp4 {

void Parser::parse() {
    PRINT_FUNC;
    _elemIdx = STATE.finalChart().size() - 1;
    Symbol lhs;
    // first we need to populate the symbol table for all procedures
    for (; _elemIdx >= 0;) {
        lhs = CFG[STATE.finalChart().at(_elemIdx)->ruleIdx()].first;
        if (lhs == Symbol::procedure || lhs == Symbol::main_s) {
            const auto nameIdx = STATE.finalChart().at(_elemIdx)->startIdx() + 1;
            _currProcedureName = STATE.getToken(nameIdx).value;
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
        } else if (lhs == Symbol::params) {
            while (!parseParams()) {
                auto dcl = parseDcl();
#ifdef DEBUG
                if (dcl->type() == ast::DclType::INT) {
                    std::cout << _currProcedureName << " has declaration '" << dcl->id() << "' of type INT\n";
                } else if (dcl->type() == ast::DclType::INT_STAR) {
                    std::cout << _currProcedureName << " has declaration '" << dcl->id() << "' of type INT_STAR\n";
                }
#endif
            }
        } else {
            --_elemIdx;
        }
    }

    std::unique_ptr<ast::Procedure> proc;
    _elemIdx = STATE.finalChart().size() - 1;
    for (;;) {
        lhs = CFG[STATE.finalChart().at(_elemIdx)->ruleIdx()].first;
        if (lhs == Symbol::procedure || lhs == Symbol::main_s) {
            if (proc) {
                STATE.addProcedure(std::move(proc));
                proc = nullptr;
            }
            const auto nameIdx = STATE.finalChart().at(_elemIdx)->startIdx() + 1;
            proc = std::make_unique<ast::Procedure>(STATE.getToken(nameIdx).value);
            _currProcedureName = proc->name();
            --_elemIdx;
        } else if (lhs == Symbol::expr && !proc->returnExpr()) {
            auto expr = parseExpr();
            if (expr->type() != ast::DclType::INT) {
                throw std::runtime_error("return value in procedure "s + proc->name() + " should be of type int");
            }
            proc->setReturnExpr(std::move(expr));
        } else if (lhs == Symbol::statements) {
            while (!parseStatements()) {
                proc->addStatement(parseStatement());
            }
        } else if (lhs == Symbol::dcls) {
            auto opt = parseDcls();
            for (; opt.has_value(); opt = parseDcls()) {
                auto dcl = parseDcl(&opt.value());
#ifdef DEBUG
                std::cout << "Value of dcl '" << dcl->id() << "' is '" << dcl->value() << "'\n";
#endif
                proc->addDeclaration(std::move(dcl));
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
            while (!parseParams()) {
                proc->addParam(parseDcl());
            }
        } else {
            --_elemIdx;
        }
        if (_elemIdx < 0) {
            STATE.addProcedure(std::move(proc));
            break;
        }
    }
}

void Parser::gotoCorrectRule(Symbol lhs, const std::string& name) {
    while (_elemIdx >= 0 && CFG[STATE.finalChart()[_elemIdx]->ruleIdx()].first != lhs) {
        --_elemIdx;
    }
    if (_elemIdx < 0) {
        throw std::runtime_error("Could not find required "s + name);
    }
}

PARSE_BEGIN(Expr, Symbol::expr)
    --_elemIdx;
    ExprPtr expr(new ast::Expr(parseTerm()));
    if (rhs.size() > 1) {
        if (rhs[1] == Symbol::PLUS) {
            expr->setPlusWith(parseExpr());
        } else if (rhs[1] == Symbol::MINUS) {
            expr->setMinusWith(parseExpr());
        }
    }
    if (expr->type() == ast::DclType::INVALID) {
        throw std::runtime_error("invalid expr generated");
    }

    return expr;
PARSE_END

PARSE_BEGIN(Term, Symbol::term)
    --_elemIdx;
    auto factor = parseFactor();
    TermPtr term(new ast::Term(std::move(factor)));
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

    return term;
PARSE_END

PARSE_BEGIN(Factor, Symbol::factor)
    FactorPtr factor(new ast::Factor(_currProcedureName));
    if (rhs.size() > 1) {
        if (rhs[0] == Symbol::ID) {
            if (rhs.size() == 3) {
                // factor -> ID LPAREN RPAREN
                factor->setProcedureCall(STATE.getToken(
                    STATE.finalChart()[_elemIdx]->startIdx()).value);
                --_elemIdx;
            } else {
                // FIXME: Really bad design here!
                const auto& procName = STATE.getToken(STATE.finalChart()[_elemIdx]->startIdx()).value;
                --_elemIdx;
                factor->setValue(parseArglist());
                factor->setProcedureCall(procName);
            }
        } else {
            --_elemIdx;
            if (rhs[0] == Symbol::LPAREN) {
                // factor -> LPAREN expr RPAREN
                factor->setValue(parseExpr());
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
            factor->setValue(STATE.getToken(STATE.finalChart()[_elemIdx]->startIdx()).value);
        } else if (rhs[0] == Symbol::NUM) {
            unsigned int num = std::stoi(STATE.getToken(STATE.finalChart()[_elemIdx]->startIdx()).value);
            factor->setValue(num);
        } else if (rhs[0] == Symbol::NULL_S) {
            factor->setValue(ast::NullType());
        }
        --_elemIdx;
    }

    return factor;
PARSE_END

PARSE_BEGIN(Lvalue, Symbol::lvalue)
    LvaluePtr lvalue(new ast::Lvalue(_currProcedureName));
    if (rhs[0] == Symbol::ID) {
        lvalue->setValue(STATE.getToken(STATE.finalChart()[_elemIdx]->startIdx()).value);
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
            lvalue->setValue(parseLvalue());
        }
    }

    return lvalue;
PARSE_END

PARSE_BEGIN(Arglist, Symbol::arglist)
    ArglistPtr arglist;
    --_elemIdx;
    if (rhs.size() == 1) {
        arglist = std::make_unique<ast::Arglist>(parseExpr());
    } else {
        arglist = std::make_unique<ast::Arglist>(parseArglist());
        arglist->setExpr(parseExpr());
    }

    return arglist;
PARSE_END

bool Parser::parseStatements() {
    PRINT_FUNC;
    gotoCorrectRule(Symbol::statements, "Symbol::statements"s);
    FETCH_RULE_RHS;
    --_elemIdx;

    return rhs.empty();
}

PARSE_BEGIN(Statement, Symbol::statement)
    StatementPtr stmt;
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
            ifStmt->addFalseStatement(parseStatement());
        }
        while (!parseStatements()) {
            ifStmt->addTrueStatement(parseStatement());
        }
        ifStmt->setTest(parseTest());
        stmt = std::move(ifStmt);
    } else if (rhs[0] == Symbol::WHILE) {
        std::unique_ptr<ast::WhileStatement> whileStmt(new ast::WhileStatement);
        while (!parseStatements()) {
            whileStmt->addStatement(parseStatement());
        }
        whileStmt->setTest(parseTest());
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

    return stmt;
PARSE_END

PARSE_BEGIN(Test, Symbol::test)
    TestPtr test(new ast::Test);
    --_elemIdx;
    auto rightExpr = parseExpr();
    auto leftExpr = parseExpr();
    if (rightExpr->type() != leftExpr->type()) {
        throw std::runtime_error("invalid types being tested");
    }
    test->setRightExpr(std::move(rightExpr));
    test->setLeftExpr(std::move(leftExpr));
    test->setOp(rhs[1]);

    return test;
PARSE_END

std::optional<Symbol> Parser::parseDcls() {
    PRINT_FUNC;
    gotoCorrectRule(Symbol::dcls, "Symbol::dcls"s);
    FETCH_RULE_RHS;
    --_elemIdx;
    if (!rhs.empty()) {
        return rhs[3];
    }

    return {};
}

DclPtr Parser::parseDcl(Symbol *sym) {
    PRINT_FUNC;
    gotoCorrectRule(Symbol::dcl, "Symbol::dcl"s);
    FETCH_RULE_RHS;
    DclPtr dcl(new ast::Dcl);
    if (sym) {
        if (*sym == Symbol::NUM) {
            dcl->setValue(STATE.getToken(STATE.finalChart().at(_elemIdx)->startIdx() + 3).value);
        } else {
            dcl->setValue(STATE.getToken(STATE.finalChart().at(_elemIdx)->startIdx() + 4).value);
        }
    }
    --_elemIdx;
    if (CFG[STATE.finalChart()[_elemIdx]->ruleIdx()].second.size() == 1) {
        dcl->setId(STATE.getToken(STATE.finalChart()[_elemIdx]->startIdx() + 1).value);
        dcl->setType(ast::DclType::INT);
    } else {
        dcl->setId(STATE.getToken(STATE.finalChart()[_elemIdx]->startIdx() + 2).value);
        dcl->setType(ast::DclType::INT_STAR);
    }
    --_elemIdx;
    STATE.addDclToProc(_currProcedureName, dcl->id(), dcl->type()); // FIXME: being called twice

    return dcl;
}

bool Parser::parseParams() {
    PRINT_FUNC;
    gotoCorrectRule(Symbol::params, "Symbol::params"s);
    FETCH_RULE_RHS;
    --_elemIdx;

    return rhs.empty();
}

} // namespace wlp4
