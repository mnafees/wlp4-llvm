#ifndef __WLP4_LLVM_BASEAST
#define __WLP4_LLVM_BASEAST

#include <llvm/IR/Value.h>

namespace wlp4 {

class BaseAST {
public:
    virtual ~BaseAST() {}
    virtual llvm::Value* codegen() = 0;
};

} // namespace wlp4

#endif // __WLP4_LLVM_BASEAST
