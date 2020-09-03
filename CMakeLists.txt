cmake_minimum_required(VERSION 3.16)

project(wlp4-llvm)

find_package(LLVM REQUIRED CONFIG)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs support core)

set(HDRS
    state.hpp
    token.hpp
    tokeniser.hpp
    parser.hpp

    ast/base.hpp
    ast/dcl.hpp
    ast/procedure.hpp
    ast/statement.hpp
    ast/test.hpp
    ast/expr.hpp
)

set(SRCS
    driver.cpp
    state.cpp
    tokeniser.cpp
    parser.cpp
    codegen.cpp

    ast/dcl.cpp
    ast/procedure.cpp
    ast/statement.cpp
)

set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-flto")

add_executable(wlp4c ${HDRS} ${SRCS})
target_link_libraries(wlp4c ${llvm_libs})
target_compile_options(wlp4c PUBLIC "-flto")
