#include "scanner.hpp"

int main(int argc, const char* argv[]) {
    using namespace wlp4;

    Scanner scanner;
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            scanner.scanFileForTokens(argv[i]);
        }
    }
}
