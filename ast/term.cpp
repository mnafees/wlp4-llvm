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

const auto& Term::factor() const {
    return _factor;
}

const auto& Term::leftTerm() const {
    return _leftTerm;
}

Term::Op Term::op() const {
    return _op;
}

}
