#ifndef __WLP4_LLVM_STATE
#define __WLP4_LLVM_STATE

// STL
#include <list>
#include <memory>
#include <vector>

// WLP4-LLVM
#include "elem.hpp"
#include "token.hpp"

namespace wlp4 {

class State {
public:
    explicit State(const char* name);
    ~State() = default;

    const std::string& filename() const;

    void addToken(Token token);
    const Token& getToken(std::size_t idx) const;
    std::size_t numTokens() const;

    void addToFinalChart(std::unique_ptr<Elem> elem);

private:
    std::string _filename;
    std::vector<Token> _tokens;
    std::vector<std::unique_ptr<Elem>> _chart;
};

} // namespace wlp4

#endif // __WLP4_LLVM_STATE
