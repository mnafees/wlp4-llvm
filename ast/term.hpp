#ifndef __WLP4_LLVM_AST_TERM
#define __WLP4_LLVM_AST_TERM

// STL
#include <memory>

// WLP4-LLVM
#include "factor.hpp"

namespace wlp4::ast {

class Term {
public:
    enum class Op : unsigned char { NONE, STAR, SLASH, PCT }; // FIXME: Reuse from Symbol?

    explicit Term(std::unique_ptr<Factor> factor) : _factor(std::move(factor)), _op(Op::NONE), _leftTerm(nullptr) {}
    ~Term() = default;

    void setStarWith(std::unique_ptr<Term> term) { _leftTerm = std::move(term); _op = Op::STAR; }
    void setSlashWith(std::unique_ptr<Term> term) { _leftTerm = std::move(term); _op = Op::SLASH; }
    void setPctWith(std::unique_ptr<Term> term) { _leftTerm = std::move(term); _op = Op::PCT; }
    const auto& factor() const { return _factor; }
    const auto& leftTerm() const { return _leftTerm; }
    Op op() const { return _op; }

    llvm::Value* codegen();

private:
    std::unique_ptr<Factor> _factor;
    Op _op;
    std::unique_ptr<Term> _leftTerm;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_TERM
