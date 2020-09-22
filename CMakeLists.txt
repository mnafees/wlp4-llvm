cmake_minimum_required(VERSION 3.12)

project(wlp4-llvm)

find_package(LLVM REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,readability-*,-header-filter=*)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs support core)

set(WLP4C_SRCS
    wlp4c.cpp
    state.cpp
    elem.cpp
    tokeniser.cpp
    recogniser.cpp
    parser.cpp
    codegen.cpp

    ast/dcl.cpp
    ast/procedure.cpp
    ast/statement.cpp
    ast/expr.cpp
    ast/factor.cpp
    ast/arglist.cpp
    ast/term.cpp
    ast/lvalue.cpp
)

add_executable(wlp4c ${WLP4C_SRCS})
target_link_libraries(wlp4c ${llvm_libs})
