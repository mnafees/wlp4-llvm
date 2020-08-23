// STL
#include <iostream>
#include <vector>

// WLP4-LLVM
#include "ast/procedure.hpp"
#include "parser.hpp"
#include "tokeniser.hpp"

int main(int argc, const char* argv[]) {
    try {
        if (argc == 1) {
            throw std::runtime_error("no input file");
        }

        wlp4::Tokeniser tokeniser;
        tokeniser.scanFileForTokens(argv[1]);

        wlp4::Parser parser;
        auto procedures = parser.parse(tokeniser);
    } catch (std::exception& e) {
        std::cerr << "wlp4c: " << e.what() << std::endl
                  << "compilation terminated." << std::endl;
        return -1;
    }

    return 0;
}
