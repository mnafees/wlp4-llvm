#include "parser.hpp"

namespace wlp4 {

void Parser::parseTokens(Tokeniser* tokeniser) {
    while (tokeniser->hasNextToken()) {
        auto tokenPair = tokeniser->nextToken();
        switch (tokenPair.first) {
            case tok_id: break;
            case tok_num: break;
            case tok_lparen: break;
            case tok_rparen: break;
            case tok_lbrace: break;
            case tok_rbrace: break;
            case tok_return: break;
            case tok_if: break;
            case tok_else: break;
            case tok_while: break;
            case tok_println: break;
            case tok_wain: break;
            case tok_becomes: break;
            case tok_int: break;
            case tok_eq: break;
            case tok_ne: break;
            case tok_lt: break;
            case tok_gt: break;
            case tok_le: break;
            case tok_ge: break;
            case tok_plus: break;
            case tok_minus: break;
            case tok_star: break;
            case tok_slash: break;
            case tok_pct: break;
            case tok_comma: break;
            case tok_semi: break;
            case tok_new: break;
            case tok_delete: break;
            case tok_lbrack: break;
            case tok_rbrack: break;
            case tok_amp: break;
            case tok_null: break;
        }
    }
}

} // namespace wlp4
