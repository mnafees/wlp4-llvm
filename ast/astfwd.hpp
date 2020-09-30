#ifndef __WLP4_LLVM_AST_ASTFWD
#define __WLP4_LLVM_AST_ASTFWD

// STL
#include <memory>

// LLVM
#include "llvm/IR/Value.h"

namespace wlp4::ast {

class Arglist;
class Dcl;
class Expr;
class Factor;
class Lvalue;
class Procedure;
class Statement;
class Term;
class Test;

using ArglistPtr = std::unique_ptr<Arglist>;
using DclPtr = std::unique_ptr<Dcl>;
using ExprPtr = std::unique_ptr<Expr>;
using FactorPtr = std::unique_ptr<Factor>;
using LvaluePtr = std::unique_ptr<Lvalue>;
using ProcedurePtr = std::unique_ptr<Procedure>;
using StatementPtr = std::unique_ptr<Statement>;
using TermPtr = std::unique_ptr<Term>;
using TestPtr = std::unique_ptr<Test>;

} // namespace wlp4::ast


#endif // __WLP4_LLVM_AST_ASTFWD
