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

namespace wlp4 {

void Parser::parse() {
#ifdef DEBUG
    std::cout << __FUNCTION__ << '\n';
#endif

    ast::Procedure* proc = nullptr;
    _elemIdx = State::instance().finalChart().size() - 1;
    for (; _elemIdx >= 0;) {
        const auto& elem = State::instance().finalChart()[_elemIdx];
#ifdef DEBUG
        std::cout << symToStr[CFG[elem->ruleIdx()].first] << std::endl;
#endif
        if (CFG[elem->ruleIdx()].first == Symbol::procedure || CFG[elem->ruleIdx()].first == Symbol::main_s) {
            for (auto sym : CFG[elem->ruleIdx()].second) {
                if (!isTerminal(sym)) {
                    _symbolStack.push(sym);
                }
            }
            proc = new ast::Procedure(State::instance().getToken(elem->startIdx() + 1).value);
            --_elemIdx;
        } else if (!_symbolStack.empty()) {
            auto sym = CFG[elem->ruleIdx()].first;
            if (sym == Symbol::expr) {
                proc->setReturnExpr(std::move(parseExpr()));
            } else if (sym == Symbol::statements) {
                while (!parseStatements()) {
                    proc->addStatement(std::move(parseStatement()));
                }
            } else if (sym == Symbol::dcls) {
                while (!parseDcls()) {
                    auto dcl = parseDcl();
                    State::instance().addDclToProc(proc->name(), dcl->id(), dcl->type());
                    proc->addDeclaration(std::move(dcl));
                }
            } else if (sym == Symbol::dcl) {
                auto dcl2 = parseDcl();
                auto dcl1 = parseDcl();
                proc->addParam(std::move(dcl1));
                proc->addParam(std::move(dcl2));
            } else if (sym == Symbol::params) {

            }
        }
        if (_symbolStack.empty()) {
            State::instance().addProcedure(std::unique_ptr<ast::Procedure>(proc));
            proc = nullptr;
        }
    }
}

std::unique_ptr<ast::Expr> Parser::parseExpr() {
#ifdef DEBUG
    std::cout << __FUNCTION__ << '\n';
#endif

    if (_symbolStack.top() != Symbol::expr) {
        throw std::runtime_error("Symbol::expr expected to parse");
    } else if (CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].first != Symbol::expr) {
        throw std::runtime_error("Expected lhs Symbol::expr not found");
    }

    ast::Expr* expr = nullptr;
    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
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
#ifdef DEBUG
    std::cout << __FUNCTION__ << '\n';
#endif

    if (_symbolStack.top() != Symbol::term) {
        throw std::runtime_error("Symbol::term expected to parse");
    } else if (CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].first != Symbol::term) {
        throw std::runtime_error("Expected lhs Symbol::term not found");
    }

    ast::Term* term = nullptr;
    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
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
#ifdef DEBUG
    std::cout << __FUNCTION__ << '\n';
#endif

    if (_symbolStack.top() != Symbol::factor) {
        throw std::runtime_error("Symbol::factor expected to parse");
    } else if (CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].first != Symbol::factor) {
        throw std::runtime_error("Expected lhs Symbol::factor not found");
    }

    std::unique_ptr<ast::Factor> factor(new ast::Factor(_currProcedureName));
    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
    if (rhs.size() > 1) {
        if (rhs[0] == Symbol::ID && rhs.size() == 3) {
            // factor -> ID LPAREN RPAREN
            factor->setProcedureCall(State::instance().getToken(
                State::instance().finalChart()[_elemIdx]->startIdx()).value);
            --_elemIdx;
        } else {
            for (std::size_t i = 0; i < rhs.size(); ++i) {
                if (!isTerminal(rhs[i])) {
                    _symbolStack.push(rhs[i]);
                }
            }
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
            } else if (rhs[0] == Symbol::ID) { // factor -> ID LPAREN arglist RPAREN
                // FIXME: Really bad design here!
                const auto& procName = State::instance().getToken(State::instance().finalChart()[_elemIdx]->startIdx()).value;
                factor->setValue(std::move(parseArglist()));
                factor->setProcedureCall(procName);
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
    _symbolStack.pop();

    return std::move(factor);
}

std::unique_ptr<ast::Lvalue> Parser::parseLvalue() {
#ifdef DEBUG
    std::cout << __FUNCTION__ << '\n';
#endif

    if (_symbolStack.top() != Symbol::lvalue) {
        throw std::runtime_error("Symbol::lvalue expected to parse");
    } else if (CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].first != Symbol::lvalue) {
        throw std::runtime_error("Expected lhs Symbol::lvalue not found");
    }

    std::unique_ptr<ast::Lvalue> lvalue(new ast::Lvalue(_currProcedureName));
    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
    if (rhs[0] == Symbol::ID) {
        lvalue->setValue(State::instance().getToken(State::instance().finalChart()[_elemIdx]->startIdx()).value);
    } else {
        _symbolStack.push(rhs[1]);
        if (rhs[0] == Symbol::STAR) {
            lvalue->setValue(std::move(parseFactor()));
        } else if (rhs[0] == Symbol::LPAREN) {
            lvalue->setValue(std::move(parseLvalue()));
        }
    }
    --_elemIdx;
    _symbolStack.pop();

    return std::move(lvalue);
}

std::unique_ptr<ast::Arglist> Parser::parseArglist() {
#ifdef DEBUG
    std::cout << __FUNCTION__ << '\n';
#endif

    if (_symbolStack.top() != Symbol::arglist) {
        throw std::runtime_error("Symbol::arglist expected to parse");
    } else if (CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].first != Symbol::arglist) {
        throw std::runtime_error("Expected lhs Symbol::arglist not found");
    }

    ast::Arglist* arglist = nullptr;
    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
    for (std::size_t i = 0; i < rhs.size(); ++i) {
        if (!isTerminal(rhs[i])) {
            _symbolStack.push(rhs[i]);
        }
    }
    --_elemIdx;
    if (rhs.size() == 1) {
        arglist = new ast::Arglist(std::move(parseExpr()));
    } else {
        arglist = new ast::Arglist(std::move(parseArglist()));
        arglist->setExpr(std::move(parseExpr()));
    }
    _symbolStack.pop();

    return std::unique_ptr<ast::Arglist>(arglist);
}

bool Parser::parseStatements() {
#ifdef DEBUG
    std::cout << __FUNCTION__ << '\n';
#endif

    if (_symbolStack.top() != Symbol::statements) {
        throw std::runtime_error("Symbol::statements expected to parse");
    } else if (CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].first != Symbol::statements) {
        throw std::runtime_error("Expected lhs Symbol::statements not found");
    }

    _symbolStack.pop();
    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
    bool emptyStmt = true;
    if (!rhs.empty()) {
        emptyStmt = false;
        for (std::size_t i = 0; i < rhs.size(); ++i) {
            if (!isTerminal(rhs[i])) {
                _symbolStack.push(rhs[i]);
            }
        }
    }

    return emptyStmt;
}

std::unique_ptr<ast::Statement> Parser::parseStatement() {
#ifdef DEBUG
    std::cout << __FUNCTION__ << '\n';
#endif

    if (_symbolStack.top() != Symbol::statement) {
        throw std::runtime_error("Symbol::statement expected to parse");
    } else if (CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].first != Symbol::statement) {
        throw std::runtime_error("Expected lhs Symbol::statement not found");
    }

    ast::Statement* stmt = nullptr;
    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
    for (std::size_t i = 0; i < rhs.size(); ++i) {
        if (!isTerminal(rhs[i])) {
            _symbolStack.push(rhs[i]);
        }
    }
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
    --_elemIdx;
    _symbolStack.pop();

    return std::unique_ptr<ast::Statement>(stmt);
}

std::unique_ptr<ast::Test> Parser::parseTest() {
#ifdef DEBUG
    std::cout << __FUNCTION__ << '\n';
#endif

    if (_symbolStack.top() != Symbol::test) {
        throw std::runtime_error("Symbol::test expected to parse");
    } else if (CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].first != Symbol::test) {
        throw std::runtime_error("Expected lhs Symbol::test not found");
    }

    auto test = new ast::Test;
    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
    for (std::size_t i = 0; i < rhs.size(); ++i) {
        if (!isTerminal(rhs[i])) {
            _symbolStack.push(rhs[i]);
        }
    }
    test->setRightExpr(std::move(parseExpr()));
    test->setLeftExpr(std::move(parseExpr()));
    test->setOp(rhs[1]);

    --_elemIdx;
    _symbolStack.pop();

    return std::unique_ptr<ast::Test>(test);
}

bool Parser::parseDcls() {
#ifdef DEBUG
    std::cout << __FUNCTION__ << '\n';
#endif

    if (_symbolStack.top() != Symbol::dcls) {
        throw std::runtime_error("Symbol::dcls expected to parse");
    } else if (CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].first != Symbol::dcls) {
        throw std::runtime_error("Expected lhs Symbol::dcls not found");
    }

    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
    bool emptyDcls = true;
    if (!rhs.empty()) {
        emptyDcls = false;
        for (std::size_t i = 0; i < rhs.size(); ++i) {
            if (!isTerminal(rhs[i])) {
                _symbolStack.push(rhs[i]);
            }
        }
    }
    --_elemIdx;
    _symbolStack.pop();

    return emptyDcls;
}

std::unique_ptr<ast::Dcl> Parser::parseDcl() {
#ifdef DEBUG
    std::cout << __FUNCTION__ << '\n';
#endif

    if (_symbolStack.top() != Symbol::dcl) {
        throw std::runtime_error("Symbol::dcl expected to parse");
    } else if (CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].first != Symbol::dcl) {
        throw std::runtime_error("Expected lhs Symbol::dcl not found");
    }

    const auto& rhs = CFG[State::instance().finalChart()[_elemIdx]->ruleIdx()].second;
    auto dcl = new ast::Dcl;
    dcl->setId(State::instance().getToken(State::instance().finalChart()[_elemIdx]->startIdx()).value);

    return std::unique_ptr<ast::Dcl>(dcl);
}

} // namespace wlp4
