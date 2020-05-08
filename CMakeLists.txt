cmake_minimum_required(VERSION 3.16)

project(wlp4-llvm)

find_package(LLVM REQUIRED CONFIG)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/syntax.cfg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

llvm_map_components_to_libnames(llvm_libs support core)

set(HDRS
    tokeniser.hpp
    parser.hpp
    ast/baseAST.hpp
    ast/idAST.hpp
    ast/procedureAST.hpp
)

set(SRCS
    tokeniser.cpp
    parser.cpp
    ast.cpp
)

add_executable(wlp4c main.cpp ${HDRS} ${SRCS})
target_link_libraries(wlp4c ${llvm_libs})
