// Self
#include "ast/term.hpp"

#ifdef DEBUG
// STL
#include <iostream>
#endif

// WLP4-LLVM
#include "ast/factor.hpp"

namespace wlp4::ast {

Term::Term(FactorPtr factor) :
    _factor(std::move(factor)),
    _op(Op::NONE),
    _leftTerm(nullptr) {}

Term::~Term() {}

void Term::setStarWith(TermPtr term) {
    _leftTerm = std::move(term);
    _op = Op::STAR;
}

void Term::setSlashWith(TermPtr term) {
    _leftTerm = std::move(term);
    _op = Op::SLASH;
}

void Term::setPctWith(TermPtr term) {
    _leftTerm = std::move(term);
    _op = Op::PCT;
}

DclType Term::type() const {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << '\n';
#endif

    if (_op == Op::NONE) {
        // The type of a term deriving factor is the same as the type of the derived factor
        return _factor->type();
    } else if (_leftTerm->type() != DclType::INT || _factor->type() != DclType::INT) {
        // The type of a term directly deriving anything other than just factor must have the
        // derived term and factor to both be int
        return DclType::INVALID;
    }

    // The type of a term directly deriving anything other than just factor is int
    return DclType::INT;
}

}
