#ifndef __WLP4_LLVM_AST_DCL
#define __WLP4_LLVM_AST_DCL

#include <string>

#include "base.hpp"

namespace wlp4::ast {

enum Type { INT, INT_STAR };

class Dcl : public Base {
public:
    Dcl(Type type, const std::string& id) : _type(type), _id(id) {}

    llvm::Value* codegen() override;

private:
    Type _type;
    std::string _id;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_DCL
