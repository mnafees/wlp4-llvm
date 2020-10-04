#pragma once
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

} // namespace wlp4::ast

using ArglistPtr = std::unique_ptr<wlp4::ast::Arglist>;
using DclPtr = std::unique_ptr<wlp4::ast::Dcl>;
using ExprPtr = std::unique_ptr<wlp4::ast::Expr>;
using FactorPtr = std::unique_ptr<wlp4::ast::Factor>;
using LvaluePtr = std::unique_ptr<wlp4::ast::Lvalue>;
using ProcedurePtr = std::unique_ptr<wlp4::ast::Procedure>;
using StatementPtr = std::unique_ptr<wlp4::ast::Statement>;
using TermPtr = std::unique_ptr<wlp4::ast::Term>;
using TestPtr = std::unique_ptr<wlp4::ast::Test>;

#endif // __WLP4_LLVM_AST_ASTFWD
