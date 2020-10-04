// STL
#include <iostream>

// LLVM
#include "llvm/Support/CommandLine.h"

// WLP4-LLVM
#include "parser.hpp"
#include "recogniser.hpp"
#include "state.hpp"
#include "tokeniser.hpp"

using namespace llvm;

cl::opt<std::string> OutputFilename("o", cl::desc("Specify output filename"), cl::value_desc("filename"));

int main(int argc, const char* argv[]) {
    cl::SetVersionPrinter([](raw_ostream& os) {
        os << "wlp4c 1.0" << '\n'
           << "Copyright (C) 2020 Mohammed Nafees" << '\n';
    });
    cl::ParseCommandLineOptions(argc, argv);

    try {
        using namespace wlp4;
        auto& state = State::instance();
        state.setFilename(argv[1]);
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

        state.initLLVMCodegen();
        for (const auto& proc : State::instance().procedures()) {
            proc->codegen();
        }
        state.dumpObjectFile();
        // using namespace std::string_literals;
        // const auto compileCmd = "clang "s + State::instance().filename() + ".o -o "s + State::instance().filename() + ".exe"s;
        // std::system(compileCmd.c_str());
    } catch (const std::exception& e) {
        std::cerr << "wlp4c: " << e.what() << '\n'
                  << "compilation terminated." << '\n';
        return -1;
    }

    return 0;
}
