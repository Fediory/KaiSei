/**
 * @file AST.h
 * @author Fediory Feng
 * @brief include in AST.cpp, "baseAST" with stmt, assign, etc class.
 * @date 2024/05/01
 */
#pragma once

#include "define.h"

#include <utility>


class BaseAST {
public:
    AST_PTR head;
    TOKEN_PTR now_token;
    TOKEN_PTR next_token;
    std::shared_ptr<Symtable> symtable_ptr;
    void GoNext() {
        now_token = next_token;
        next_token = token_safe::next(now_token);
    }
    explicit BaseAST(const TOKEN_PTR& token_head, const Symtable& symtable) {
        symtable_ptr = std::make_shared<Symtable>(symtable);
        head = std::make_shared<AST_node>();
        head->symtable_ptr = symtable_ptr;
        now_token = token_head;
        next_token = token_safe::next(now_token);
    }
    explicit BaseAST(const TOKEN_PTR& token_head, const std::shared_ptr<Symtable>& symtable_ptr) {
        this->symtable_ptr = symtable_ptr;
        head = std::make_shared<AST_node>();
        head->symtable_ptr = symtable_ptr;
        now_token = token_head;
        next_token = token_safe::next(now_token);
    }
    void PassDownSymtableAttribute(const SYM_PTR& symtable_node_ptr) const {
        symtable_node temp = *symtable_ptr->my_head;
        *symtable_ptr->my_head = *symtable_node_ptr;
        symtable_ptr->my_head->table_id = temp.table_id;
        symtable_ptr->my_head->is_head = temp.is_head;
        symtable_ptr->my_head->identifier_name = temp.identifier_name;
        symtable_ptr->my_head->next = nullptr;
    }
    [[nodiscard]] SYM_PTR GetBackSymtableAttribute() const {
        return symtable_ptr->my_head;
    }
    virtual ~BaseAST() = default;
};


// examples:
// x = 3
// y = 8 * 5 + 1
//              ^
//         expr return
// notice:
// - no , ; )
// - usually end with ; , )
// - self return at expr::after
class SingleAssignmentAST: public BaseAST {
public:
    TOKEN_PTR Parse();
    explicit SingleAssignmentAST(const TOKEN_PTR& token_head, const Symtable& symtable): BaseAST(token_head, symtable) {}
    explicit SingleAssignmentAST(const TOKEN_PTR& token_head, const std::shared_ptr<Symtable>& symtable_ptr): BaseAST(token_head, symtable_ptr) {}
    ~SingleAssignmentAST() override = default;
};


// examples:
// x[2] = 3
// y[7][4] = 8 * 5 + 1
//                    ^
//               expr return
// notice:
// - no , ; )
// - usually end with ; , )
// - self return at expr::after
class ArrayAssignmentAST: public BaseAST {
public:
    TOKEN_PTR Parse();
    explicit ArrayAssignmentAST(const TOKEN_PTR& token_head, const Symtable& symtable): BaseAST(token_head, symtable) {}
    explicit ArrayAssignmentAST(const TOKEN_PTR& token_head, const std::shared_ptr<Symtable>& symtable_ptr): BaseAST(token_head, symtable_ptr) {}
    ~ArrayAssignmentAST() override = default;
};


// examples:
// x
// x = 3
// y = 8 * 5 + 1
//              ^
//         expr return
// notice:
// - no , or ;
// - usually end with , or ;
// - self return at identifyName::after or expr::after
class SingleDefinitionAST: public BaseAST {
public:
    TOKEN_PTR Parse();
    explicit SingleDefinitionAST(const TOKEN_PTR& token_head, const Symtable& symtable): BaseAST(token_head, symtable) {}
    explicit SingleDefinitionAST(const TOKEN_PTR& token_head, const std::shared_ptr<Symtable>& symtable_ptr): BaseAST(token_head, symtable_ptr) {}
    ~SingleDefinitionAST() override = default;
};


// examples:
// {}
// {8 * 5 + 1, 7}
// {{}, {2}, 1, 3}
//        ^
//   expr return
// notice:
// - self return at }::after
class ArrayInitialBlockAST: public BaseAST {
public:
    TOKEN_PTR Parse();
    explicit ArrayInitialBlockAST(const TOKEN_PTR& token_head, const Symtable& symtable): BaseAST(token_head, symtable) {}
    explicit ArrayInitialBlockAST(const TOKEN_PTR& token_head, const std::shared_ptr<Symtable>& symtable_ptr): BaseAST(token_head, symtable_ptr) {}
    ~ArrayInitialBlockAST() override = default;
};


// examples:
// a[]
// x[5] = {}
// y[6] = {8 * 5 + 1, 7}
// z[3][3] = {{}, {2}, 1, 3}
//                          ^
//                  init block return
// notice:
// - no , or ;
// - end with , or ;
// - self return at ]::after or }::after
class ArrayDefinitionAST: public BaseAST {
public:
    TOKEN_PTR Parse();
    explicit ArrayDefinitionAST(const TOKEN_PTR& token_head, const Symtable& symtable): BaseAST(token_head, symtable) {}
    explicit ArrayDefinitionAST(const TOKEN_PTR& token_head, const std::shared_ptr<Symtable>& symtable_ptr): BaseAST(token_head, symtable_ptr) {}
    ~ArrayDefinitionAST() override = default;
};


// examples:
// int x = 3, y = 4;
//          ^      ^
//         def return
// const float x[5] = {2.5, 1};
//                            ^
//                        def return
// notice:
// - including , ;
// - self return at ;::after
class DeclarationStatementAST: public BaseAST {
public:
    TOKEN_PTR Parse();
    explicit DeclarationStatementAST(const TOKEN_PTR& token_head, const Symtable& symtable): BaseAST(token_head, symtable) {}
    explicit DeclarationStatementAST(const TOKEN_PTR& token_head, const std::shared_ptr<Symtable>& symtable_ptr): BaseAST(token_head, symtable_ptr) {}
    ~DeclarationStatementAST() override = default;
};


// examples:
// 2 + 1;
//      ^
// expr return
// x = 51;
//       ^
// assign return
// { x = 5; }
//           ^
//      block return
// if (x == 3) x = 2;
// if (x > 2) {}
//              ^
//          key return
// return 2 + 5;
//              ^
//          key return
// notice:
// - including ;
// - self return at ;::after or }::after
class NormalStatementAST: public BaseAST {
public:
    TOKEN_PTR Parse();
    explicit NormalStatementAST(const TOKEN_PTR& token_head, const Symtable& symtable): BaseAST(token_head, symtable) {}
    explicit NormalStatementAST(const TOKEN_PTR& token_head, const std::shared_ptr<Symtable>& symtable_ptr): BaseAST(token_head, symtable_ptr) {}
    ~NormalStatementAST() override = default;
};


// examples:
// 5;
// 2 + 1;
// int x = 3;
//           ^
//  def statement return
// if (x > 2) {x = x - 1;}
//                        ^
//                normal stmt return
// notice:
// - including { and }
// - self return at }::after or ;::after
class StatementAST: public BaseAST {
public:
    TOKEN_PTR Parse();
    explicit StatementAST(const TOKEN_PTR& token_head, const Symtable& symtable): BaseAST(token_head, symtable) {}
    explicit StatementAST(const TOKEN_PTR& token_head, const std::shared_ptr<Symtable>& symtable_ptr): BaseAST(token_head, symtable_ptr) {}
    ~StatementAST() override = default;
};


// examples:
// { 5; 2 + 1; int x = 3; if (x > 2) {x = x - 1;} x = x + 5; return 0; }
//                       ^                       ^                     ^
//                statement return        statement return      statement return
// notice:
// - including { and }
// - self return at }::after
class BlockStatementAST: public BaseAST {
public:
    TOKEN_PTR Parse();
    explicit BlockStatementAST(const TOKEN_PTR& token_head, const Symtable& symtable): BaseAST(token_head, symtable) {}
    explicit BlockStatementAST(const TOKEN_PTR& token_head, const std::shared_ptr<Symtable>& symtable_ptr): BaseAST(token_head, symtable_ptr) {}
    ~BlockStatementAST() override = default;
};


// examples:
// int x
// float y
// notice:
// - end with , or )
// - self return at identifyName::after
class FunctionFormParamAST: public BaseAST {
public:
    TOKEN_PTR Parse();
    explicit FunctionFormParamAST(const TOKEN_PTR& token_head, const Symtable& symtable): BaseAST(token_head, symtable) {}
    explicit FunctionFormParamAST(const TOKEN_PTR& token_head, const std::shared_ptr<Symtable>& symtable_ptr): BaseAST(token_head, symtable_ptr) {}
    ~FunctionFormParamAST() override = default;
};


// examples:
// int x, float y, int z
//               ^
//      single param return
// notice:
// - no ( or )
// - end with )
// - self return at identifyName::after
class FunctionParamsAST: public BaseAST {
public:
    TOKEN_PTR Parse();
    explicit FunctionParamsAST(const TOKEN_PTR& token_head, const Symtable& symtable): BaseAST(token_head, symtable) {}
    explicit FunctionParamsAST(const TOKEN_PTR& token_head, const std::shared_ptr<Symtable>& symtable_ptr): BaseAST(token_head, symtable_ptr) {}
    ~FunctionParamsAST() override = default;
};


// examples:
// void func(int a, float b) { 3 + 1; return; }
//                         ^                   ^
//                     param return       block return
// notice:
// - no ; at the end, {} is in block item
// - self return at }::after
class FunctionDefinitionAST: public BaseAST {
public:
    TOKEN_PTR Parse();
    explicit FunctionDefinitionAST(const TOKEN_PTR& token_head, const Symtable& symtable): BaseAST(token_head, symtable) {}
    explicit FunctionDefinitionAST(const TOKEN_PTR& token_head, const std::shared_ptr<Symtable>& symtable_ptr): BaseAST(token_head, symtable_ptr) {}
    ~FunctionDefinitionAST() override = default;
};


// the program itself
class ProgramAST: public BaseAST {
public:
    TOKEN_PTR Parse();
    explicit ProgramAST(const TOKEN_PTR& token_head, const Symtable& symtable): BaseAST(token_head, symtable) {}
    explicit ProgramAST(const TOKEN_PTR& token_head, const std::shared_ptr<Symtable>& symtable_ptr): BaseAST(token_head, symtable_ptr) {}
    ~ProgramAST() override = default;
};