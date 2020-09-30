#ifndef __WLP4_LLVM_AST_TERM
#define __WLP4_LLVM_AST_TERM

// STL
#include <memory>

// WLP4-LLVM
#include "astfwd.hpp"
#include "type.hpp"

namespace wlp4::ast {

class Term {
public:
    enum class Op : unsigned char { NONE, STAR, SLASH, PCT }; // FIXME: Reuse from Symbol?

    explicit Term(FactorPtr factor);
    ~Term();

    void setStarWith(TermPtr term);
    void setSlashWith(TermPtr term);
    void setPctWith(TermPtr term);

    DclType type() const;

    llvm::Value* codegen();

private:
    FactorPtr _factor;
    Op _op;
    TermPtr _leftTerm;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_TERM
