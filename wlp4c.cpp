// STL
#include <cstdlib>
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
#ifdef DEBUG
        std::cout << "===================================" << '\n'
                  << "======== TOKENISATION BEGIN =======" << '\n'
                  << "===================================" << '\n';
#endif
        Tokeniser().tokenise();
#ifdef DEBUG
        std::cout << "===================================" << '\n'
                  << "======== TOKENISATION END =========" << '\n'
                  << "===================================" << '\n'
                  << "===================================" << '\n'
                  << "======== RECOGNISER BEGIN =========" << '\n'
                  << "===================================" << '\n';
#endif
        Recogniser().recognise();
#ifdef DEBUG
        std::cout << "===================================" << '\n'
                  << "======== RECOGNISER END ===========" << '\n'
                  << "===================================" << '\n';
        State::instance().printFinalChart();
        std::cout << "===================================" << '\n'
                  << "======== PARSER BEGIN =============" << '\n'
                  << "===================================" << '\n';
#endif
        Parser().parse();
#ifdef DEBUG
        std::cout << "===================================" << '\n'
                  << "======== PARSER END ===============" << '\n'
                  << "===================================" << '\n';
#endif

        State::instance().initLLVMCodegen();
        for (const auto& proc : State::instance().procedures()) {
            proc->codegen();
        }
        State::instance().dumpObjectFile();
        // using namespace std::string_literals;
        // const auto compileCmd = "clang "s + State::instance().filename() + ".o -o "s + State::instance().filename() + ".exe"s;
        // std::system(compileCmd.c_str());
    } catch (std::exception& e) {
        std::cerr << "wlp4c: " << e.what() << '\n'
                  << "compilation terminated." << '\n';
        return -1;
    }

    return 0;
}
