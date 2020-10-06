// STL
#include <filesystem>
#include <iostream>

// LLVM
#include "llvm/Support/CommandLine.h"

// WLP4-LLVM
#include "parser.hpp"
#include "recogniser.hpp"
#include "state.hpp"
#include "tokeniser.hpp"

using namespace llvm;
namespace fs = std::filesystem;

cl::OptionCategory Category("WLP4-LLVM Options");
cl::opt<std::string> InputFile(cl::Positional, cl::desc("file"), cl::Required);
cl::opt<std::string> OutputFilename("o", cl::desc("Output filename"), cl::value_desc("filename"), cl::cat(Category));
cl::opt<std::string> Compiler("compiler", cl::desc("C compiler"), cl::cat(Category), cl::Required);

int main(int argc, const char* argv[]) {
    cl::SetVersionPrinter([](raw_ostream& os) {
        os << "wlp4c 1.0" << '\n'
           << "Copyright (C) 2020 Mohammed Nafees" << '\n';
    });
    cl::HideUnrelatedOptions(Category);
    cl::ParseCommandLineOptions(argc, argv);

    try {
        using namespace wlp4;

        auto& state = State::instance();
        if (!fs::exists(fs::path(InputFile.c_str()))) {
            throw std::runtime_error("input file does not exist");
        }
        state.setInputFilePath(InputFile.c_str());
        state.setOutputFilePath(OutputFilename.c_str());
        state.setCompilerPath(Compiler.c_str());
        // All of the below operations simply populate data in the State singeton object
        Tokeniser().tokenise();
        Recogniser().recognise();
        Parser().parse();

        state.initLLVMCodegen();
        for (const auto& proc : State::instance().procedures()) {
            proc->codegen();
        }
        state.compile();
    } catch (const std::exception& e) {
        std::cerr << "wlp4c: " << e.what() << '\n'
                  << "compilation terminated." << '\n';
        return -1;
    }

    return 0;
}
