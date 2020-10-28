cmake_minimum_required(VERSION 3.13.4)

project(wlp4-llvm LANGUAGES CXX)

option(BUILD_MLIR "Build the WLP4-LLVM MLIR dialect" OFF)

if (BUILD_MLIR)
    find_package(MLIR REQUIRED CONFIG)
else()
    find_package(LLVM REQUIRED CONFIG)
endif()

if (BUILD_MLIR)
    if (${LLVM_PACKAGE_VERSION} VERSION_LESS "11.0.0")
        message(FATAL_ERROR "Please use LLVM versions 11.0.0 and above to build wlp4-llvm")
    endif()
else()
    if (${LLVM_PACKAGE_VERSION} VERSION_LESS "8.0.0")
        message(FATAL_ERROR "Please use LLVM versions 8.0.0 and above to build wlp4-llvm")
    endif()
endif()

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
if (BUILD_MLIR)
    message(STATUS "Using MLIRConfig.cmake in: ${MLIR_DIR}")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT ${LLVM_ENABLE_RTTI})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs support core native asmprinter)

add_subdirectory(third_party/fmt-7.0.3 EXCLUDE_FROM_ALL)

include_directories(third_party/fmt-7.0.3/include)
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
target_link_libraries(wlp4c ${llvm_libs} fmt::fmt)
