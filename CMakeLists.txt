cmake_minimum_required(VERSION 3.16)

project(wlp4-llvm)

find_package(LLVM REQUIRED CONFIG)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs support core)

set(HDRS
    tokeniser.hpp
    parser.hpp
    ast/base.hpp
    ast/dcl.hpp
    ast/procedure.hpp
)

set(SRCS
    driver.cpp
    tokeniser.cpp
    parser.cpp
    ast.cpp
)

add_executable(wlp4c ${HDRS} ${SRCS})
target_link_libraries(wlp4c ${llvm_libs})
