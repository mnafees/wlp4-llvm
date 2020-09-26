// Self
#include "term.hpp"

// WLP4-LLVM
#include "factor.hpp"

namespace wlp4::ast {

Term::Term(std::unique_ptr<Factor> factor) :
    _factor(std::move(factor)),
    _op(Op::NONE),
    _leftTerm(nullptr) {}

void Term::setStarWith(std::unique_ptr<Term> term) {
    _leftTerm = std::move(term);
    _op = Op::STAR;
}

void Term::setSlashWith(std::unique_ptr<Term> term) {
    _leftTerm = std::move(term);
    _op = Op::SLASH;
}

void Term::setPctWith(std::unique_ptr<Term> term) {
    _leftTerm = std::move(term);
    _op = Op::PCT;
}

DclType Term::type() const {
    if (_op == Op::NONE) {
        // The type of a term deriving factor is the same as the type of the derived factor
        return _factor->type();
    }

    // The type of a term directly deriving anything other than just factor is int
    return DclType::INT;
}

}
