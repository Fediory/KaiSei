/**
 * @file opt.h
 * @author Fediory Feng
 * @brief impl of optimise AST values.
 * @date 2024/05/02
 */

#include "opt.h"
#include <iostream>
#include <set>

std::set<std::string> count;

void AST_optimize_safe::raise_error(const std::string &error_code)
{
    std::cout << "AST Optimize ERROR: " << error_code << std::endl;
    Safe::GlobalError = true;
}

identify_value_type_tuple Optimize_Useful::implicit_conversion(const identify_value_type_tuple &a, const identify_value_type_tuple &b)
{

    identify_value_type_tuple res;

    // make F(a) <= F(b), F() is a map from identify_value_type_tuple types to integer
    const identify_value_type_tuple *ap = &a, *bp = &b;
    int F_a = (int)a.self_is_pointer() * 10 + (int)a.self_basic_type();
    int F_b = (int)b.self_is_pointer() * 10 + (int)b.self_basic_type();
    if (F_a > F_b)
    {
        ap = &b;
        bp = &a;
    }

    if (ap->self_is_pointer() == bp->self_is_pointer() && ap->self_basic_type() == bp->self_basic_type())
    {
        res.reset_and_parse_from_basic_type(ap->self_basic_type(), ap->self_is_pointer());
        return res;
    }
    else if (!ap->self_is_pointer() &&
             (ap->self_basic_type() == basic_any || ap->self_basic_type() == basic_unused))
    {
        res.reset_and_parse_from_basic_type(bp->self_basic_type(), bp->self_is_pointer());
        return res;
    }
    else if (!ap->self_is_pointer() && ap->self_basic_type() == basic_int)
    {
        res.reset_and_parse_from_basic_type(bp->self_basic_type(), bp->self_is_pointer());
        return res;
    }
    else if (ap->self_is_pointer() &&
             (ap->self_basic_type() == basic_any || ap->self_basic_type() == basic_unused))
    {
        res.reset_and_parse_from_basic_type(bp->self_basic_type(), bp->self_is_pointer());
        return res;
    }
    else
    {
        AST_optimize_safe::raise_error(
            "cannot implicitly converse " + a.to_string() + " and " + b.to_string());
        return res;
    }
}

AST_PTR Optimize_Useful::get_last_child(const AST_PTR &statement_ast)
{
    if (statement_ast == nullptr)
        return nullptr;
    AST_PTR now = statement_ast->child, res = nullptr;
    while (now != nullptr)
    {
        res = now;
        now = now->sister;
    }
    return res;
}

identify_value_type_tuple Optimize_Useful::binary_operator_implicit_conversion(
    const std::string &binary_operator,
    const identify_value_type_tuple &a,
    const identify_value_type_tuple &b)
{

    if (
        binary_operator == "==" || binary_operator == "!=" || binary_operator == ">=" || binary_operator == "<=" ||
        binary_operator == ">" || binary_operator == "<" || binary_operator == "!" || binary_operator == "&&" || binary_operator == "||")
    {
        identify_value_type_tuple res;
        res.reset_and_parse_from_string("int");
        return res;
    }
    else if (binary_operator == "%")
    {
        if (!a.self_is_pointer() && a.self_basic_type() != basic_int)
        {
            AST_optimize_safe::raise_error("cannot mod " + a.to_string() + " and " + b.to_string());
        }
        identify_value_type_tuple res;
        res.reset_and_parse_from_string("int");
        return res;
    }
    else
    {
        return Optimize_Useful::implicit_conversion(a, b);
    }
}

// contains many meddle error which need to be thrown by binary_operator_implicit_conversion()
// waiting for refactor  TBD...
identify_value_type_tuple Optimize_Useful::calculate(
    const std::string &binary_operator,
    const identify_value_type_tuple &a,
    const identify_value_type_tuple &b)
{

    identify_value_type_tuple res = binary_operator_implicit_conversion(binary_operator, a, b);
    if (Safe::GlobalError)
        return res;

    if (res.self_basic_type() == basic_any)
        AST_optimize_safe::raise_error("cannot calculate " + a.to_string() + " and " + b.to_string());

    if (binary_operator == "+unary")
    {
        res = b;
    }

    else if (binary_operator == "-unary")
    {
        if (res.self_is_pointer() || res.self_basic_type() == basic_int)
            res.reset_and_assign_as_int(-b.self_get_int_value());
        else
            AST_optimize_safe::raise_error("cannot use unary minus before " + b.to_string());
    }

    if (binary_operator == "!")
    {
        if (res.self_is_pointer() || res.self_basic_type() == basic_int)
            res.reset_and_assign_as_int((int)!b.self_get_int_value());
        else
            AST_optimize_safe::raise_error("cannot plus " + a.to_string() + " and " + b.to_string());
    }

    else if (binary_operator == "+")
    {
        identify_value_type_tuple temp_a = a, temp_b = b;
        if (res.self_is_pointer())
        {
            if (a.self_is_pointer() && !b.self_is_pointer() && b.self_basic_type() == basic_int)
                res.reset_and_assign_as_int(
                    temp_a.self_get_int_value() + temp_b.self_get_int_value() * 4);
            else
                AST_optimize_safe::raise_error("cannot plus " + a.to_string() + " and " + b.to_string());
        }
        else if (res.self_basic_type() == basic_int)
        {
            temp_a.self_change_to_int();
            temp_b.self_change_to_int();
            res.reset_and_assign_as_int(
                temp_a.self_get_int_value() + temp_b.self_get_int_value());
        }
        else
        {
            AST_optimize_safe::raise_error("cannot plus " + a.to_string() + " and " + b.to_string());
        }
    }

    else if (binary_operator == "-")
    {
        identify_value_type_tuple temp_a = a, temp_b = b;
        if (res.self_is_pointer())
        {
            if (a.self_is_pointer() && !b.self_is_pointer() && b.self_basic_type() == basic_int)
                res.reset_and_assign_as_int(
                    temp_a.self_get_int_value() - temp_b.self_get_int_value() * 4);
            else
                AST_optimize_safe::raise_error("cannot minus " + a.to_string() + " and " + b.to_string());
        }
        else if (res.self_basic_type() == basic_int)
        {
            temp_a.self_change_to_int();
            temp_b.self_change_to_int();
            res.reset_and_assign_as_int(
                temp_a.self_get_int_value() - temp_b.self_get_int_value());
        }
        else
        {
            AST_optimize_safe::raise_error("cannot minus " + a.to_string() + " and " + b.to_string());
        }
    }

    else if (binary_operator == "*")
    {
        identify_value_type_tuple temp_a = a, temp_b = b;
        if (res.self_is_pointer())
        {
            AST_optimize_safe::raise_error("cannot multiply " + a.to_string() + " and " + b.to_string());
        }
        else if (res.self_basic_type() == basic_int)
        {
            temp_a.self_change_to_int();
            temp_b.self_change_to_int();
            res.reset_and_assign_as_int(
                temp_a.self_get_int_value() * temp_b.self_get_int_value());
        }
        else
        {
            AST_optimize_safe::raise_error("cannot multiply " + a.to_string() + " and " + b.to_string());
        }
    }

    else if (binary_operator == "/")
    {
        identify_value_type_tuple temp_a = a, temp_b = b;
        if (res.self_is_pointer())
        {
            AST_optimize_safe::raise_error("cannot divide " + a.to_string() + " and " + b.to_string());
        }
        else if (res.self_basic_type() == basic_int)
        {
            temp_a.self_change_to_int();
            temp_b.self_change_to_int();
            res.reset_and_assign_as_int(
                temp_a.self_get_int_value() / temp_b.self_get_int_value());
        }
        else
        {
            AST_optimize_safe::raise_error("cannot divide " + a.to_string() + " and " + b.to_string());
        }
    }

    else if (binary_operator == "%")
    {
        identify_value_type_tuple temp_a = a, temp_b = b;
        if (res.self_is_pointer())
        {
            AST_optimize_safe::raise_error("cannot mod " + a.to_string() + " and " + b.to_string());
        }
        else if (res.self_basic_type() == basic_int)
        {
            temp_a.self_change_to_int();
            temp_b.self_change_to_int();
            res.reset_and_assign_as_int(
                temp_a.self_get_int_value() % temp_b.self_get_int_value());
        }
        else
        {
            AST_optimize_safe::raise_error("cannot mod " + a.to_string() + " and " + b.to_string());
        }
    }

    else if (binary_operator == "&&")
    {
        identify_value_type_tuple temp_a = a, temp_b = b;
        if (res.self_is_pointer() || res.self_basic_type() == basic_int)
        {
            temp_a.self_change_to_int();
            temp_b.self_change_to_int();
            res.reset_and_assign_as_int(
                temp_a.self_get_int_value() && temp_b.self_get_int_value());
        }
        else
        {
            AST_optimize_safe::raise_error("cannot calculate " + a.to_string() + " and " + b.to_string());
        }
    }

    else if (binary_operator == "||")
    {
        identify_value_type_tuple temp_a = a, temp_b = b;
        if (res.self_is_pointer() || res.self_basic_type() == basic_int)
        {
            temp_a.self_change_to_int();
            temp_b.self_change_to_int();
            res.reset_and_assign_as_int(
                temp_a.self_get_int_value() || temp_b.self_get_int_value());
        }
        else
        {
            AST_optimize_safe::raise_error("cannot calculate " + a.to_string() + " and " + b.to_string());
        }
    }

    else if (binary_operator == "==")
    {
        identify_value_type_tuple temp_a = a, temp_b = b;
        if (res.self_is_pointer() || res.self_basic_type() == basic_int)
        {
            temp_a.self_change_to_int();
            temp_b.self_change_to_int();
            res.reset_and_assign_as_int(
                temp_a.self_get_int_value() == temp_b.self_get_int_value());
        }
        else
        {
            AST_optimize_safe::raise_error("cannot calculate " + a.to_string() + " and " + b.to_string());
        }
    }

    else if (binary_operator == "!=")
    {
        identify_value_type_tuple temp_a = a, temp_b = b;
        if (res.self_is_pointer() || res.self_basic_type() == basic_int)
        {
            temp_a.self_change_to_int();
            temp_b.self_change_to_int();
            res.reset_and_assign_as_int(
                temp_a.self_get_int_value() != temp_b.self_get_int_value());
        }
        else
        {
            AST_optimize_safe::raise_error("cannot calculate " + a.to_string() + " and " + b.to_string());
        }
    }

    else if (binary_operator == ">=")
    {
        identify_value_type_tuple temp_a = a, temp_b = b;
        if (res.self_is_pointer() || res.self_basic_type() == basic_int)
        {
            temp_a.self_change_to_int();
            temp_b.self_change_to_int();
            res.reset_and_assign_as_int(
                temp_a.self_get_int_value() >= temp_b.self_get_int_value());
        }
        else
        {
            AST_optimize_safe::raise_error("cannot calculate " + a.to_string() + " and " + b.to_string());
        }
    }

    else if (binary_operator == "<=")
    {
        identify_value_type_tuple temp_a = a, temp_b = b;
        if (res.self_is_pointer() || res.self_basic_type() == basic_int)
        {
            temp_a.self_change_to_int();
            temp_b.self_change_to_int();
            res.reset_and_assign_as_int(
                temp_a.self_get_int_value() <= temp_b.self_get_int_value());
        }
        else
        {
            AST_optimize_safe::raise_error("cannot calculate " + a.to_string() + " and " + b.to_string());
        }
    }

    else if (binary_operator == ">")
    {
        identify_value_type_tuple temp_a = a, temp_b = b;
        if (res.self_is_pointer() || res.self_basic_type() == basic_int)
        {
            temp_a.self_change_to_int();
            temp_b.self_change_to_int();
            res.reset_and_assign_as_int(
                temp_a.self_get_int_value() > temp_b.self_get_int_value());
        }
        else
        {
            AST_optimize_safe::raise_error("cannot calculate " + a.to_string() + " and " + b.to_string());
        }
    }

    else if (binary_operator == "<")
    {
        identify_value_type_tuple temp_a = a, temp_b = b;
        if (res.self_is_pointer() || res.self_basic_type() == basic_int)
        {
            temp_a.self_change_to_int();
            temp_b.self_change_to_int();
            res.reset_and_assign_as_int(
                temp_a.self_get_int_value() < temp_b.self_get_int_value());
        }
        else
        {
            AST_optimize_safe::raise_error("cannot calculate " + a.to_string() + " and " + b.to_string());
        }
    }

    return res;
}

void Front::Optimiser::optimize_single(const AST_PTR &now, const AST_PTR &parent)
{
    if (now == nullptr)
        return;
    optimize_single(now->child, now);
    
    if (now->type == DeclarationStatement && now->data == "const")
    {
        AST_PTR child = now->child;

        // declaration conversion
        while (child != nullptr && child->type == SingleDefinition)
        {
            if (child->last_child->count_expr_ending)
            {
                if (child->IVTT.self_basic_type() == basic_int || child->IVTT.self_is_pointer())
                {
                    child->IVTT = child->last_child->IVTT;
                    child->IVTT.self_change_to_int();
                }
                child->declaration_bound_sym_node->treat_as_constexpr = true;
                child->declaration_bound_sym_node->IVTT = child->IVTT;
            }
            child = child->sister;
        }
    }
    
    if (now->type == Expression)
    {
        AST_tuple res = AST_safe::count_child_number(now);
        if (res.judge)
            now->count_expr_ending = true;
        if (res.count == 1)
        {
            now->IVTT.reset_and_parse_from_basic_type(now->child->IVTT.self_basic_type(),
                                                      now->child->IVTT.self_is_pointer());
            if (res.judge)
                now->IVTT = now->child->IVTT;
        }
        if (res.count == 2)
        {
            // std::cout <<  now->child->IVTT.self_get_int_value() << std::endl;
            identify_value_type_tuple temp = Optimize_Useful::binary_operator_implicit_conversion(
                now->data, now->child->IVTT, now->last_child->IVTT);
            now->IVTT.reset_and_parse_from_basic_type(temp.self_basic_type(), temp.self_is_pointer());
            if (res.judge){
                now->IVTT = Optimize_Useful::calculate(now->data, now->child->IVTT, now->last_child->IVTT);
            }
        }

        // vvv --- ! important --- vvv //
        // let shared_ptr destroy all the child tree node.
        if (res.judge)
            now->child = now->last_child = nullptr;
        // ^^^ --- ! important --- ^^^ //
    }

    // destroy redundant statement
    if (now->type == Statement || now->type == NormalStatement || now->type == DeclarationStatement)
    {
        if (now->child != nullptr)
            if (
                now->child->type == SingleDefinition || now->child->type == ArrayDefinition ||
                now->child->type == SingleAssignment || now->child->type == ArrayAssignment ||
                now->child->type == NormalStatement || now->child->type == BlockStatement ||
                now->child->type == KeywordStatement)
            {
                now->last_child = Optimize_Useful::get_last_child(now);

                // warning: now->parent->last_child has not changed !!!
                now->copy(now->child);
                AST_PTR temp = now->child;
                now->last_child->sister = now->sister;
                now->child = temp->child;
                now->sister = temp->sister;
                now->last_child = temp->last_child;
            }
    }

    if (now->type == ArrayDefinition && count.find(now->only_name) == count.end())
    {

        count.insert(now->only_name);

        AST_PTR temp = now->child->sister;
        if (temp->type != Index)
        {
            AST_optimize_safe::raise_error("temp->type != Index, AST need to be fixed");
            return;
        }
        temp = temp->child;
        while (temp != nullptr)
        {
            if (temp->count_expr_ending)
            {
                temp->IVTT.self_change_to_int();
                now->IVTT.array_add(temp->IVTT.self_get_int_value());
                now->declaration_bound_sym_node->IVTT.array_add(temp->IVTT.self_get_int_value());
            }
            else
            {
                AST_optimize_safe::raise_error("array index definition parameter is not a const-expression");
                now->IVTT.array_add(0);
                now->declaration_bound_sym_node->IVTT.array_add(0);
                break;
            }
            temp = temp->sister;
        }
    }

    if (now->type == FunctionFormParam && now->last_child->type == Index && count.find(now->only_name) == count.end())
    {

        count.insert(now->only_name);

        AST_PTR temp = now->child->sister;
        if (temp->type != Index)
        {
            AST_optimize_safe::raise_error("temp->type != Index, AST need to be fixed");
            return;
        }
        temp = temp->child;
        while (temp != nullptr)
        {
            if (temp->count_expr_ending)
            {
                temp->IVTT.self_change_to_int();
                now->IVTT.array_add(temp->IVTT.self_get_int_value());
                now->declaration_bound_sym_node->IVTT.array_add(temp->IVTT.self_get_int_value());
            }
            else
            {
                now->IVTT.array_add(0);
                now->declaration_bound_sym_node->IVTT.array_add(0);
                break;
            }
            temp = temp->sister;
        }
    }

    optimize_single(now->sister, parent);
}

void Front::Optimiser::Optimize(const AST_PTR &source_AST_head)
{
    Front::Optimiser::optimize_single(source_AST_head, nullptr);
}
