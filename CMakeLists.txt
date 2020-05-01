project(wlp4-llvm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRCS
    main.cpp
    scanner.cpp
)

add_executable(wlp4c ${SRCS})
