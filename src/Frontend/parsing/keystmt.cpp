/**
 * @file keystmt.cpp
 * @author Fediory Feng
 * @brief impl of KeywordStatementAST
 * @date 2024/05/01
 */

#include "keystmt.h"

TOKEN_PTR KeywordStatementAST::Parse()
{
    head->type = KeywordStatement;

    if (token_safe::data(now_token) == "if")
    {
        head->data = "if";
        GoNext();

        if (token_safe::data(now_token) != "(")
        {
            AST_safe::raise_error("in KeywordStatement, if condition statement should begin with [(]", now_token);
            return now_token;
        }
        GoNext();

        ExpressionAST expr(now_token, symtable_ptr);
        AST_node::connect_child(head, expr.head);
        expr.head->data = "condition";
        now_token = expr.Parse();
        next_token = token_safe::next(now_token);

        if (token_safe::data(now_token) != ")")
        {
            AST_safe::raise_error("in KeywordStatement, if condition statement should end with [)]", now_token);
            return now_token;
        }
        GoNext();

        StatementAST stmt(now_token, symtable_ptr);
        AST_node::connect_child(head, stmt.head);
        now_token = stmt.Parse();
        next_token = token_safe::next(now_token);

        if (token_safe::data(now_token) == "else")
        {
            head->data = "if-else";
            GoNext();

            StatementAST else_stmt(now_token, symtable_ptr);
            AST_node::connect_child(head, else_stmt.head);
            now_token = else_stmt.Parse();
            next_token = token_safe::next(now_token);
        }
    }

    else if (token_safe::data(now_token) == "while")
    {
        head->data = "while";
        GoNext();

        if (token_safe::data(now_token) != "(")
        {
            AST_safe::raise_error("in KeywordStatement, while condition statement should begin with [(]", now_token);
            return now_token;
        }
        GoNext();

        ExpressionAST expr(now_token, symtable_ptr);
        AST_node::connect_child(head, expr.head);
        expr.head->data = "condition";
        now_token = expr.Parse();
        next_token = token_safe::next(now_token);

        if (token_safe::data(now_token) != ")")
        {
            AST_safe::raise_error("in KeywordStatement, while condition statement should end with [)]", now_token);
            return now_token;
        }
        GoNext();

        StatementAST stmt(now_token, symtable_ptr);
        AST_node::connect_child(head, stmt.head);
        now_token = stmt.Parse();
        next_token = token_safe::next(now_token);
    }

    else if (token_safe::data(now_token) == "break")
    {
        head->data = "break";
        GoNext();

        if (token_safe::data(now_token) != ";")
        {
            AST_safe::raise_error("in KeywordStatement, break should end with [;]", now_token);
            return now_token;
        }
        GoNext();
    }

    else if (token_safe::data(now_token) == "continue")
    {
        head->data = "continue";
        GoNext();

        if (token_safe::data(now_token) != ";")
        {
            AST_safe::raise_error("in KeywordStatement, continue should end with [;]", now_token);
            return now_token;
        }
        GoNext();
    }

    else if (token_safe::data(now_token) == "return")
    {
        head->data = "return";
        GoNext();

        ExpressionAST expr(now_token, symtable_ptr);
        AST_node::connect_child(head, expr.head);
        expr.head->data = "value";
        now_token = expr.Parse();
        next_token = token_safe::next(now_token);

        if (token_safe::data(now_token) != ";")
        {
            AST_safe::raise_error("in KeywordStatement, return should end with [;]", now_token);
            return now_token;
        }
        GoNext();
    }
    else
    {
        AST_safe::raise_error("in KeywordStatement, begin with wrong sign", now_token);
        return now_token;
    }

    return now_token;
}