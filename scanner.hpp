#ifndef __WLP4_LLVM_SCANNER
#define __WLP4_LLVM_SCANNER

#include <string>
#include <vector>

enum Token {
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

class Scanner {
public:
    void scanFileForTokens(const char* name);

private:
    std::vector<std::pair<Token, std::string>> _symbols;
};

} // namespace wlp4

#endif // __WLP4_LLVM_SCANNER
