// STL
#include <iostream>

// WLP4-LLVM
#include "parser.hpp"
#include "recogniser.hpp"
#include "state.hpp"
#include "tokeniser.hpp"

static void initLLVMCodegen();

int main(int argc, const char* argv[]) {
    try {
        if (argc == 1) {
            throw std::runtime_error("no input file");
        }

        wlp4::Tokeniser tokeniser;
        tokeniser.tokenise();

        wlp4::Recogniser recogniser;
        recogniser.recognise();

        wlp4::Parser parser;
        parser.parse();

        initLLVMCodegen();
        for (const auto& proc : wlp4::State::instance().procedures()) {
            proc->codegen();
        }
    } catch (std::exception& e) {
        std::cerr << "wlp4c: " << e.what() << std::endl
                  << "compilation terminated." << std::endl;
        return -1;
    }

    return 0;
}
