#ifndef __WLP4_LLVM_AST_PROCEDURE
#define __WLP4_LLVM_AST_PROCEDURE

#include "base.hpp"
#include "dcl.hpp"

#include <memory>

namespace wlp4::ast {

class Procedure : public Base {
public:
    Procedure(const std::string& name) : _name(name) {}

    llvm::Value* codegen() override;
    bool isWain() const { return _name == "wain"; }
    const std::string& name() const { return _name; }
    void addParam(std::unique_ptr<Dcl>&& dcl) {
        _params.push_back(std::move(dcl));
    }

private:
    std::string _name;
    std::vector<std::unique_ptr<Dcl>> _params;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_PROCEDURE
