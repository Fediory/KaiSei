# Compiler KaiSei

玩具用编译器项目：KaiSei（取自かいせい · [JP] 解构）

项目包含7200行代码，616行注释，1011行空白，采用蛇形命名法+驼峰命名法

### 学习历程

- 【4月】编译器初期学习，包括词法、语法等

- 【5月初】使用模板完成前端AST树，并实现部分IR

- 【5月底】舍弃原本模板，自行开发KaiSei

- 【6.3】自行实现语义分析

- 【6.7】前端、IR、控制流图完成

- 【6.15】后端ARMv7 TARGET完成

- 【6.22】Debug完成，可通过71个测例（可确保90%测例在IR端均能正常通过）

  ![AC](.\fig\AC.png)

### 项目目录

```cmd
|-- Backend
|   |-- define
|   |   |-- CFG_define.cpp
|   |   |-- IA_define.cpp
|   |   |-- LVA_define.cpp
|   |   `-- RA_define.cpp
|   |-- include
|   |   |-- CFG.h
|   |   |-- IA.h
|   |   |-- LVA.h
|   |   |-- RA.h
|   |   `-- define_backend.h
|   `-- opt
|       |-- CFG.cpp
|       |-- IA.cpp
|       |-- LVA.cpp
|       `-- RA.cpp
|-- Frontend
|   |-- IR
|   |   `-- IR.cpp
|   |-- define
|   |   |-- AST_node.cpp
|   |   |-- IR_define.cpp
|   |   |-- define.cpp
|   |   `-- symtable.cpp
|   |-- include
|   |   |-- AST.h
|   |   |-- IR.h
|   |   |-- define.h
|   |   |-- expr.h
|   |   |-- keystmt.h
|   |   |-- lexical.h
|   |   |-- opt.h
|   |   `-- shell.h
|   |-- lexical
|   |   `-- lexical.cpp
|   `-- parsing
|       |-- AST.cpp
|       |-- expr.cpp
|       |-- keystmt.cpp
|       `-- opt.cpp
`-- main.cpp

10 directories, 32 files
```

- Lexical：词法分析
- Parsing：语法+语义（AST树+递归下降分析法）
- Optimize：AST树优化常数合并与传播 + 代数化简与强度消减
- IR：生成自定义IR
- CFG：生成控制流图
- LVA：活跃变量分析，无效变量删除
- RA：寄存器分配（图着色）
- IA：指令指派+生成ARM32

### 使用方法

- make clean：重新编译生成可执行文件
- make start：第一次编译此文件
- make all：重新编译此文件
- make help（./KaiSei help）：查看如何使用此编译器 

![help](.\fig\help.png)

### Debug流程

1. 将需要debug的文件放入debug文件夹，并命名为test.c
2. 输入make dbg
3. 在debug文件夹下可以按顺序生成该文件的各阶段输出
4. 根据输出检查错误