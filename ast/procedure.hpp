#ifndef __WLP4_LLVM_AST_PROCEDURE
#define __WLP4_LLVM_AST_PROCEDURE

#include "base.hpp"
#include "dcl.hpp"

#include <memory>

namespace wlp4::ast {

class Procedure : public Base {
public:
    explicit Procedure(const std::string& name);

    llvm::Value* codegen() override;
    bool isWain() const;
    const std::string& name() const;
    void addParam(std::unique_ptr<Dcl>&& dcl) noexcept(false);
    void addDeclaration(std::unique_ptr<Dcl>&& dcl) noexcept(false);

private:
    void validateVariable(const std::string& name) noexcept(false);

    std::string _name;
    std::vector<std::unique_ptr<Dcl>> _params;
    std::vector<std::unique_ptr<Dcl>> _dcls;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_PROCEDURE
