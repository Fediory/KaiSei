/**
 * @file keystmt.h
 * @author Fediory Feng
 * @brief include in keystmt.cpp with KeywordStatementAST.
 * @date 2024/05/01
 */
#pragma once
#include "AST.h"
#include "expr.h"

// examples:
// if (x == 3) x = 2;
//           ^
//       expr return
// if (x > 2) {}
//              ^
//       statement return
// return 2 + 5;
//             ^
//        expr return
// notice:
// - including ;
// - self return at ;::after or }::after
class KeywordStatementAST: public BaseAST {
public:
    TOKEN_PTR Parse();
    explicit KeywordStatementAST(const TOKEN_PTR& token_head, const Symtable& symtable): BaseAST(token_head, symtable) {}
    explicit KeywordStatementAST(const TOKEN_PTR& token_head, const std::shared_ptr<Symtable>& symtable_ptr): BaseAST(token_head, symtable_ptr) {}
    ~KeywordStatementAST() override = default;
};