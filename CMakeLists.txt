cmake_minimum_required(VERSION 3.12)

project(wlp4-llvm)

find_package(LLVM REQUIRED CONFIG)

if (${LLVM_PACKAGE_VERSION} VERSION_LESS "8.0.0")
    message(FATAL_ERROR "Please use LLVM versions 8.0.0 and above to build wlp4-llvm")
endif()

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,readability-*,-header-filter=*)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
if (NOT ${LLVM_ENABLE_RTTI})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs support core native asmprinter)

include_directories(include)
set(WLP4C_SRCS
    wlp4c.cpp
    src/state.cpp
    src/elem.cpp
    src/tokeniser.cpp
    src/recogniser.cpp
    src/parser.cpp
    src/codegen.cpp

    src/ast/dcl.cpp
    src/ast/procedure.cpp
    src/ast/statement.cpp
    src/ast/expr.cpp
    src/ast/factor.cpp
    src/ast/arglist.cpp
    src/ast/term.cpp
    src/ast/lvalue.cpp
    src/ast/test.cpp
)

add_executable(wlp4c ${WLP4C_SRCS})
target_link_libraries(wlp4c ${llvm_libs})
