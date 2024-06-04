cmake_minimum_required(VERSION 3.22)
project(KaiSei)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

include_directories(

        src/Frontend/include/
        
)

add_executable(KaiSei

        src/main.cpp

        src/Frontend/define/define.cpp
        src/Frontend/lexical/lexical.cpp
)
