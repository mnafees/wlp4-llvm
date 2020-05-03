#include "tokeniser.hpp"
#include "parser.hpp"

#include <iostream>

int main(int argc, const char* argv[]) {
    try {
        if (argc == 1) {
            throw std::runtime_error("no input files");
        }

        wlp4::Tokeniser tokeniser;
        for (int i = 1; i < argc; ++i) {
            tokeniser.scanFileForTokens(argv[i]);
        }
        wlp4::Parser parser;
        parser.parseTokens(&tokeniser);
    } catch (std::exception& e) {
        std::cerr << "wlp4c: " << e.what() << std::endl
                  << "compilation terminated." << std::endl;
        return -1;
    }

    return 0;
}
