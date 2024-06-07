cmake_minimum_required(VERSION 3.22)
project(KaiSei)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")



include_directories(
        src/Frontend/include/
        src/Backend/include/
)

add_executable(KaiSei

        src/main.cpp
        
        # Frontend
        src/Frontend/define/define.cpp
        src/Frontend/define/AST_node.cpp
        src/Frontend/define/symtable.cpp
        src/Frontend/define/IR_define.cpp

        src/Frontend/lexical/lexical.cpp

        src/Frontend/parsing/AST.cpp
        src/Frontend/parsing/expr.cpp
        src/Frontend/parsing/keystmt.cpp
        src/Frontend/parsing/opt.cpp

        src/Frontend/IR/IR.cpp

        # Backend
        src/Backend/define/CFG_define.cpp
        src/Backend/define/LVA_define.cpp
        src/Backend/define/RA_define.cpp
        src/Backend/define/IA_define.cpp
        
        src/Backend/opt/CFG.cpp
        src/Backend/opt/LVA.cpp
        src/Backend/opt/RA.cpp
        src/Backend/opt/IA.cpp
)
