#ifndef __WLP4_LLVM_TOKENISER
#define __WLP4_LLVM_TOKENISER

#include <string>
#include <vector>

struct Token {
    int type;
    int col;
    int line;
    std::string value;
};

enum {
    tok_id = 0,
    tok_num,
    tok_lparen,
    tok_rparen,
    tok_lbrace,
    tok_rbrace,
    tok_return,
    tok_if,
    tok_else,
    tok_while,
    tok_println,
    tok_wain,
    tok_becomes,
    tok_int,
    tok_eq,
    tok_ne,
    tok_lt,
    tok_gt,
    tok_le,
    tok_ge,
    tok_plus,
    tok_minus,
    tok_star,
    tok_slash,
    tok_pct,
    tok_comma,
    tok_semi,
    tok_new,
    tok_delete,
    tok_lbrack,
    tok_rbrack,
    tok_amp,
    tok_null
};

namespace wlp4 {

class Tokeniser {
public:
    Tokeniser() : _currentTokenIdx(0) {}

    void scanFileForTokens(const char* name) noexcept(false);
    void resetTokenHead();
    bool hasNextToken() const;
    const Token& nextToken() noexcept(false);
    void backUpToken() noexcept(false);

private:
    std::vector<Token> _symbols;
    size_t _currentTokenIdx;
};

} // namespace wlp4

#endif // __WLP4_LLVM_TOKENISER
