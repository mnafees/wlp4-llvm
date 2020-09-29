// STL
#include <iostream>

// WLP4-LLVM
#include "parser.hpp"
#include "recogniser.hpp"
#include "state.hpp"
#include "tokeniser.hpp"

int main(int argc, const char* argv[]) {
    try {
        if (argc == 1) {
            throw std::runtime_error("no input file");
        }

        using namespace wlp4;

        State::instance().setFilename(argv[1]);
        // All of the below operations simply populate data in the State singeton object
        Tokeniser().tokenise();
        Recogniser().recognise();
#ifdef DEBUG
        State::instance().printFinalChart();
#endif
        Parser().parse();

        State::instance().initLLVMCodegen();
        for (const auto& proc : State::instance().procedures()) {
            proc->codegen();
        }
        State::instance().dumpObjectFile();
    } catch (std::exception& e) {
        std::cerr << "wlp4c: " << e.what() << std::endl
                  << "compilation terminated." << std::endl;
        return -1;
    }

    return 0;
}
