cmake_minimum_required(VERSION 3.16)

project(wlp4-llvm)

find_package(LLVM REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs support core)

set(WLP4BASE_SRCS
    state.cpp
    elem.cpp
    tokeniser.cpp
    recogniser.cpp
)

set(WLP4C_SRCS
    wlp4c.cpp
    parser.cpp
    codegen.cpp

    ast/dcl.cpp
    ast/procedure.cpp
    ast/statement.cpp
)

add_library(wlp4base SHARED ${WLP4BASE_SRCS})

add_executable(wlp4check wlp4check.cpp)
add_executable(wlp4c ${WLP4C_SRCS})
target_link_libraries(wlp4check wlp4base ${llvm_libs}) # We do not need llvm linkage here
target_link_libraries(wlp4c wlp4base ${llvm_libs})
