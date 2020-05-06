cmake_minimum_required(VERSION 3.16)

project(wlp4-llvm)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/syntax.cfg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(SRCS
    tokeniser.cpp
    parser.cpp
)

add_executable(wlp4c main.cpp ${SRCS})
