/**
 * @file expr.cpp
 * @author Fediory Feng
 * @brief expr impl of ArrayUsageAST, FunctionUsageAST, ExpressionAST
 * @date 2024/05/01
 */

#include "expr.h"
#include <stack>

int assign_operator_left(const std::string &op)
{
    if (op == "begin" || op == "end")
        return 0;
    else if (op == "||")
        return 1;
    else if (op == "&&")
        return 2;
    else if (op == "==" || op == "!=")
        return 3;
    else if (op == "<" || op == ">" || op == "<=" || op == ">=")
        return 4;
    else if (op == "+" || op == "-")
        return 5;
    else if (op == "*" || op == "/" || op == "%")
        return 6;
    else if (op == "+unary" || op == "-unary" || op == "!")
        return 64;
    else
        return -1;
}

int assign_operator_right(const std::string &op)
{
    if (op == "begin" || op == "end")
        return 0;
    else if (op == "||")
        return 1;
    else if (op == "&&")
        return 2;
    else if (op == "==" || op == "!=")
        return 3;
    else if (op == "<" || op == ">" || op == "<=" || op == ">=")
        return 4;
    else if (op == "+" || op == "-")
        return 5;
    else if (op == "*" || op == "/" || op == "%")
        return 6;
    else if (op == "+unary" || op == "-unary" || op == "!")
        return 63;
    else
        return -1;
}

int compare(const std::string &op1, const std::string &op2)
{
    int a = assign_operator_left(op1), b = assign_operator_right(op2);
    return a - b;
}

TOKEN_PTR ArrayUsageAST::Parse()
{
    head->type = ArrayUsage;

    if (token_safe::type(now_token) != IDENT)
    {
        AST_safe::raise_error("in ArrayUsage, identify name is not valid", now_token);
        return now_token;
    }

    // v --- sym search & attribution --- v //
    SYM_PTR temp_sym_node = AST_safe::search_id_name(token_safe::data(now_token), symtable_ptr);
    if (temp_sym_node == nullptr)
    {
        AST_safe::raise_error("Usage without definition", now_token);
        GoNext(); // ! important
        return now_token;
    }
    head->absorb_sym_attribution(temp_sym_node);
    head->declaration_bound_sym_node = temp_sym_node;
    // ^ --- sym search & attribution--- ^ //

    AST_PTR var_name = std::make_shared<AST_node>();
    AST_node::connect_child(head, var_name);
    var_name->type = Identifier;
    var_name->data = now_token->data;
    GoNext();

    if (token_safe::data(now_token) != "[")
    {
        AST_safe::raise_error("in ArrayUsage, begin punctuation should be [[]", now_token);
        return now_token;
    }
    GoNext();

    AST_PTR index_ast = std::make_shared<AST_node>();
    AST_node::connect_child(head, index_ast);
    index_ast->type = Index;

    ExpressionAST expr(now_token, symtable_ptr);
    AST_node::connect_child(index_ast, expr.head);
    expr.head->data = "value";
    expr.head->comment = "index";
    now_token = expr.Parse();
    next_token = token_safe::next(now_token);

    if (token_safe::data(now_token) != "]")
    {
        AST_safe::raise_error("in ArrayUsage, end punctuation should be []]", now_token);
        return now_token;
    }
    GoNext();

    while (token_safe::data(now_token) == "[")
    {
        GoNext();

        ExpressionAST addi_expr(now_token, symtable_ptr);
        AST_node::connect_child(index_ast, addi_expr.head);
        addi_expr.head->data = "value";
        addi_expr.head->comment = "index";
        now_token = addi_expr.Parse();
        next_token = token_safe::next(now_token);

        if (token_safe::data(now_token) != "]")
        {
            AST_safe::raise_error("in ArrayUsage, end punctuation should be []]", now_token);
            return now_token;
        }
        GoNext();
    }

    return now_token;
}

TOKEN_PTR FunctionUsageAST::Parse()
{
    head->type = FunctionUsage;

    if (token_safe::type(now_token) != IDENT)
    {
        AST_safe::raise_error("in FunctionUsage, identify name is not valid", now_token);
        return now_token;
    }

    // v --- sym search & attribution --- v //
    SYM_PTR temp_sym_node = AST_safe::search_id_name(token_safe::data(now_token), symtable_ptr);
    head->absorb_sym_attribution(temp_sym_node);
    head->declaration_bound_sym_node = temp_sym_node;
    // ^ --- sym search & attribution--- ^ //

    AST_PTR var_name = std::make_shared<AST_node>();
    AST_node::connect_child(head, var_name);
    var_name->type = Identifier;
    var_name->data = now_token->data;
    GoNext();

    if (token_safe::data(now_token) != "(")
    {
        AST_safe::raise_error("in FunctionUsage, begin punctuation should be [(]", now_token);
        return now_token;
    }
    GoNext();

    if (token_safe::data(now_token) == ")")
    {
        GoNext();
        return now_token;
    }

    ExpressionAST expr(now_token, symtable_ptr);
    AST_node::connect_child(head, expr.head);
    expr.head->data = "value";
    expr.head->comment = "argument";
    now_token = expr.Parse();
    next_token = token_safe::next(now_token);

    while (true)
    {

        if (token_safe::data(now_token) == ")")
        {
            GoNext();
            break;
        }

        if (token_safe::data(now_token) != ",")
        {
            AST_safe::raise_error("in FunctionUsage, punctuation should be [,]", now_token);
            return now_token;
        }
        GoNext();

        ExpressionAST addi_expr(now_token, symtable_ptr);
        AST_node::connect_child(head, addi_expr.head);
        addi_expr.head->data = "value";
        addi_expr.head->comment = "argument";
        now_token = addi_expr.Parse();
        next_token = token_safe::next(now_token);
    }

    return now_token;
}

TOKEN_PTR ExpressionAST::Parse()
{
    head->type = Expression;
    head->using_attribute = true;

    std::stack<std::string> opt;
    std::stack<AST_PTR> sym;
    opt.push("begin");

    if (token_safe::data(now_token) == "+" || token_safe::data(now_token) == "-")
        now_token->data += "unary";

    bool quit = false;
    while (true)
    {

        if (now_token == nullptr)
        {
            AST_safe::raise_error("in Expression, lost ending", now_token);
            return now_token;
        }

        const std::string expression_ending[] = {";", ",", ")", "}", "]"};
        for (const auto &i : expression_ending)
            if (token_safe::data(now_token) == i)
            {
                quit = true;
                break;
            }

        if (quit)
        {

            while (true)
            {

                if (opt.size() == 1 && opt.top() == "begin")
                {
                    opt.pop();
                    break;
                }

                if (opt.empty())
                {
                    AST_safe::raise_error("in Expression, fatal error occurred", now_token);
                    return now_token;
                }

                if (sym.empty())
                {
                    AST_safe::raise_error("in Expression, number or variable is not enough", now_token);
                    return now_token;
                }
                AST_PTR k1 = sym.top();
                sym.pop();
                if (sym.empty())
                {
                    AST_safe::raise_error("in Expression, number or variable is not enough", now_token);
                    return now_token;
                }
                AST_PTR k2 = sym.top();
                sym.pop();
                AST_PTR tog = std::make_shared<AST_node>();
                tog->data = opt.top();
                opt.pop();
                tog->type = Expression;
                tog->using_attribute = true;
                AST_node::reverse_connect_child(tog, k1);
                AST_node::reverse_connect_child(tog, k2);
                sym.push(tog);
            }

            if (sym.size() == 1)
            {
                AST_node::connect_child(head, sym.top());
                sym.pop();
                break;
            }

            else
            {
                AST_safe::raise_error("in Expression, missing operators or numbers", now_token);
                return now_token;
            }
        }

        // 1 function, variables and number
        if (token_safe::type(now_token) != OPERATOR && token_safe::type(now_token) != PUNCT)
        {

            // 1.1 function or variables
            if (token_safe::type(now_token) == IDENT)
            {

                // 1.1.1 function usage [No GoNext]
                if (token_safe::data(next_token) == "(")
                {
                    FunctionUsageAST func_use(now_token, symtable_ptr);
                    sym.push(func_use.head);
                    now_token = func_use.Parse();
                    next_token = token_safe::next(now_token);
                }

                // 1.1.2 array usage [No GoNext]
                else if (token_safe::data(next_token) == "[")
                {
                    ArrayUsageAST array_use(now_token, symtable_ptr);
                    sym.push(array_use.head);
                    now_token = array_use.Parse();
                    next_token = token_safe::next(now_token);
                }

                // 1.1.3 normal variables
                else
                {
                    AST_PTR token_to_AST = std::make_shared<AST_node>();
                    token_to_AST->data = now_token->data;
                    token_to_AST->type = Identifier;
                    sym.push(token_to_AST);

                    // vvv --- sym search & attribution --- vvv //
                    SYM_PTR temp_sym_node = AST_safe::search_id_name(token_safe::data(now_token), symtable_ptr);
                    if (temp_sym_node == nullptr)
                    {
                        AST_safe::raise_error("Usage without definition", now_token);
                        GoNext(); // ! important
                        return now_token;
                    }
                    token_to_AST->absorb_sym_attribution(temp_sym_node);
                    token_to_AST->declaration_bound_sym_node = temp_sym_node;
                    // ^^^ --- sym search & attribution --- ^^^ //

                    GoNext();
                }
            }

            // 1.2 normal numbers
            else if (token_safe::type(now_token) == NUMBER)
            {
                AST_PTR token_to_AST = std::make_shared<AST_node>();
                token_to_AST->data = now_token->data;
                token_to_AST->type = Number;
                token_to_AST->IVTT = now_token->IVTT;
                token_to_AST->count_expr_ending = true;
                token_to_AST->using_attribute = true;
                sym.push(token_to_AST);
                GoNext();
            }

            // 1.3 wrong name
            else
            {
                AST_safe::raise_error("in Expression, members should only be identify name or number", now_token);
                return now_token;
            }
        }

        // 2 operators
        else
        {

            // find unary + and -
            if (token_safe::data(next_token) == "+" || token_safe::data(next_token) == "-")
                next_token->data += "unary";

            // unary+, unary-, ! should become unary (use a placeholder, which is basic_int)
            if (assign_operator_left(token_safe::data(now_token)) == 64)
            {
                AST_PTR token_to_AST = std::make_shared<AST_node>();
                token_to_AST->data = "placeholder";
                token_to_AST->count_expr_ending = true;
                token_to_AST->type = Expression;
                token_to_AST->using_attribute = true;
                sym.push(token_to_AST);
            }

            // 2.1 use ( )
            if (token_safe::data(now_token) == "(")
            {
                GoNext();

                ExpressionAST recur_expr(now_token, symtable_ptr);
                sym.push(recur_expr.head);
                recur_expr.head->data = "brace";
                now_token = recur_expr.Parse();
                next_token = token_safe::next(now_token);

                if (token_safe::data(now_token) != ")")
                {
                    AST_safe::raise_error("in Expression, missing punctuation [)]", now_token);
                    return now_token;
                }
                GoNext();
            }

            // 2.2 other operator
            else
            {
                std::string now_op = token_safe::data(now_token);

                // 2.2.1 wrong operator
                if (assign_operator_left(now_op) == -1)
                {
                    AST_safe::raise_error("in Expression, unexpected operator found", now_token);
                    return now_token;
                }

                // 2.2.2 now_token - top <= 0  means you can reduce it
                while (compare(now_op, opt.top()) <= 0)
                {
                    if (sym.empty())
                    {
                        AST_safe::raise_error("in Expression, number or variable is not enough", now_token);
                        return now_token;
                    }
                    AST_PTR k1 = sym.top();
                    sym.pop();
                    if (sym.empty())
                    {
                        AST_safe::raise_error("in Expression, number or variable is not enough", now_token);
                        return now_token;
                    }
                    AST_PTR k2 = sym.top();
                    sym.pop();
                    AST_PTR tog = std::make_shared<AST_node>();
                    tog->data = opt.top();
                    opt.pop();
                    tog->type = Expression;
                    tog->using_attribute = true;
                    AST_node::reverse_connect_child(tog, k1);
                    AST_node::reverse_connect_child(tog, k2);
                    sym.push(tog);
                }

                opt.push(now_op);
                GoNext();
            }
        }
    }
    return now_token;
}