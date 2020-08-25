#ifndef __WLP4_LLVM_TOKENISER
#define __WLP4_LLVM_TOKENISER

// STL
#include <string>
#include <vector>

namespace wlp4 {

class State;

class Tokeniser {
public:
    Tokeniser() = default;
    ~Tokeniser() = default;

    void loadFile(const char* filename, State& state);
};

} // namespace wlp4

#endif // __WLP4_LLVM_TOKENISER
