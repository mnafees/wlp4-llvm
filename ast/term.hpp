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

    explicit Term(std::unique_ptr<Factor> factor);

    void setStarWith(std::unique_ptr<Term> term);
    void setSlashWith(std::unique_ptr<Term> term);
    void setPctWith(std::unique_ptr<Term> term);

    DclType type() const;

    llvm::Value* codegen();

private:
    std::unique_ptr<Factor> _factor;
    Op _op;
    std::unique_ptr<Term> _leftTerm;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_TERM
