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
    void addParam(std::unique_ptr<Dcl>&& dcl) noexcept(false) {
        validateVariable(dcl->id());
        _params.push_back(std::move(dcl));
    }
    void addDeclaration(std::unique_ptr<Dcl>&& dcl) noexcept(false) {
        validateVariable(dcl->id());
        _dcls.push_back(std::move(dcl));
    }

private:
    void validateVariable(const std::string& name) noexcept(false) {
        for (const auto& d : _params) {
            if (d->id() == name) {
                throw std::runtime_error(name + " redeclared in procedure " + _name);
            }
        }
        for (const auto& d : _dcls) {
            if (d->id() == name) {
                throw std::runtime_error(name + " redeclared in procedure " + _name);
            }
        }
    }

    std::string _name;
    std::vector<std::unique_ptr<Dcl>> _params;
    std::vector<std::unique_ptr<Dcl>> _dcls;
};

} // namespace wlp4::ast

#endif // __WLP4_LLVM_AST_PROCEDURE
