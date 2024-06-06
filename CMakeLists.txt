cmake_minimum_required(VERSION 3.22)
project(KaiSei)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")


# find_package(OpenCV REQUIRED)
# message(STATUS "include path: ${OpenCV_INCLUDE_DIRS}")


include_directories(
        src/Frontend/include/
        # ${OpenCV_INCLUDE_DIRS}
)

add_executable(KaiSei

        src/main.cpp

        src/Frontend/define/define.cpp
        src/Frontend/define/AST_node.cpp
        src/Frontend/define/symtable.cpp

        src/Frontend/lexical/lexical.cpp

        src/Frontend/parsing/AST.cpp
        src/Frontend/parsing/expr.cpp
        src/Frontend/parsing/keystmt.cpp
        src/Frontend/parsing/opt.cpp

)
