project(wlp4-llvm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRCS
    tokeniser.cpp
    parser.cpp
)

add_executable(wlp4c main.cpp ${SRCS})
