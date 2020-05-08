#ifndef __WLP4_LLVM_IDAST
#define __WLP4_LLVM_IDAST

#include <string>

#include "baseAST.hpp"

namespace wlp4 {

class IdAST : public BaseAST {
public:
    IdAST(const std::string& id) : _id(id) {}

    llvm::Value* codegen() override;

private:
    std::string _id;
};

} // namespace wlp4

#endif // __WLP4_LLVM_IDAST
