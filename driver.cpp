#include "tokeniser.hpp"
#include "parser.hpp"
#include "ast/procedure.hpp"

#include <iostream>
#include <vector>

int main(int argc, const char* argv[]) {
    try {
        if (argc == 1) {
            throw std::runtime_error("no input file");
        }

        std::unique_ptr<wlp4::Tokeniser> tokeniser(new wlp4::Tokeniser);
        tokeniser->scanFileForTokens(argv[1]);
        std::unique_ptr<wlp4::Parser> parser(new wlp4::Parser(tokeniser));
        std::vector<std::unique_ptr<wlp4::ast::Procedure>> procedures;
        parser->parseTokens(procedures);
    } catch (std::exception& e) {
        std::cerr << "wlp4c: " << e.what() << std::endl
                  << "compilation terminated." << std::endl;
        return -1;
    }

    return 0;
}
