/**
 * @file AST_node.cpp
 * @author Fediory Feng
 * @brief AST_node and AST_safe related impl from define.h
 * @date 2024/05/01
 */

#include "define.h"
#include <iostream>
#include <fstream>

// Parsing: AST_node



void AST_node::print_all(const std::shared_ptr<AST_node> &now, int stage)
{   
    if (now == nullptr)
        return;
    for (int i = 0; i < stage; ++i)
        Debug::debug_output << "|\t";
    Debug::debug_output << AST_type_string_name[now->type];
    if (now->using_attribute)
    {
        if (!now->only_name.empty())
            Debug::debug_output << ", only_name: " << now->only_name;
        if (now->declaration_bound_sym_node != nullptr)
            Debug::debug_output << ", bound_IVTT: " << now->declaration_bound_sym_node->IVTT.to_string();
        else
            Debug::debug_output << ", IVTT: " << now->IVTT.to_string();
        if (now->is_const)
            Debug::debug_output << ", is_const";
        if (now->is_static)
            Debug::debug_output << ", is_static";
    }
    if (now->count_expr_ending)
        Debug::debug_output << ", count_expr_ending";
    Debug::debug_output << (now->data.empty() ? "" : ", " + now->data);
    Debug::debug_output << (now->comment.empty() ? "" : " (" + now->comment + ")");
    Debug::debug_output << "\n";
    print_all(now->child, stage + 1);
    print_all(now->sister, stage);
}

void AST_node::connect_child(const std::shared_ptr<AST_node> &parent, const std::shared_ptr<AST_node> &child)
{
    if (parent->child == nullptr)
    {
        parent->child = child;
        parent->last_child = child;
    }
    else
    {
        parent->last_child->sister = child;
        parent->last_child = child;
    }
}

void AST_node::reverse_connect_child(const std::shared_ptr<AST_node> &parent, const std::shared_ptr<AST_node> &child)
{
    if (parent->child == nullptr)
    {
        parent->child = child;
        parent->last_child = child;
    }
    else
    {
        child->sister = parent->child;
        parent->child = child;
    }
}

void AST_node::absorb_sym_attribution(const std::shared_ptr<symtable_node> &symtable_resource_node)
{
    using_attribute = true;
    name = symtable_resource_node->identifier_name;
    only_name = symtable_resource_node->only_name;
    IVTT = symtable_resource_node->IVTT;
    is_const = symtable_resource_node->is_const;
    is_static = symtable_resource_node->is_static;
}

void AST_node::copy(const std::shared_ptr<AST_node> &AST_resource_node)
{
    type = AST_resource_node->type;
    data = AST_resource_node->data;
    comment = AST_resource_node->comment;
    symtable_ptr = AST_resource_node->symtable_ptr;

    using_attribute = AST_resource_node->using_attribute;
    name = AST_resource_node->name;
    only_name = AST_resource_node->only_name;
    IVTT = AST_resource_node->IVTT;
    is_const = AST_resource_node->is_const;
    is_static = AST_resource_node->is_static;

    count_expr_ending = AST_resource_node->count_expr_ending;

    declaration_bound_sym_node = AST_resource_node->declaration_bound_sym_node;
}

std::shared_ptr<symtable_node> AST_safe::search_id_name(const std::string &search_name, const std::shared_ptr<symtable_node> &sym_head)
{
    if (sym_head == nullptr)
        return nullptr;
    std::shared_ptr<symtable_node> compare_now = sym_head->next;
    while (compare_now != nullptr)
    {
        if (compare_now->identifier_name == search_name)
            return compare_now;
        compare_now = compare_now->next;
    }
    return nullptr;
}

std::shared_ptr<symtable_node> AST_safe::search_id_name(const std::string &search_name, const std::shared_ptr<Symtable> &symtable_ptr, bool without_chain)
{
    std::shared_ptr<symtable_node> res = nullptr;
    if (without_chain)
        res = search_id_name(search_name, symtable_ptr->my_head);
    else
        for (int i = (int)(symtable_ptr->heads_chain.size()) - 1; i >= 0; --i)
        {
            res = search_id_name(search_name, symtable_ptr->heads_chain[i]);
            if (res != nullptr)
                return res;
        }
    return res;
}

void AST_safe::raise_error(const std::string &error_code, const std::shared_ptr<token_node> &token_node)
{
    std::cout << "ERROR: " << error_code << std::endl;
    if (token_node != nullptr)
        std::cout << "\twhere: " << token_node->data
                  << " at line " << token_node->line << ", column "
                  << token_node->column << std::endl;
    else
        std::cout << "\twhere: reach the end of the file" << std::endl;
    Safe::GlobalError = true;
}

AST_tuple AST_safe::count_child_number(const std::shared_ptr<AST_node> &now_node)
{
    if (now_node == nullptr)
        return (AST_tuple){0, false};
    AST_tuple res = {0, true};
    AST_PTR temp = now_node->child;
    while (temp != nullptr)
    {
        res.count++;
        if (!temp->count_expr_ending)
            res.judge = false;
        if (temp->declaration_bound_sym_node != nullptr)
        {
            if (temp->declaration_bound_sym_node->treat_as_constexpr)
            {
                res.judge = true;
                temp->IVTT = temp->declaration_bound_sym_node->IVTT;
            }
        }
        temp = temp->sister;
    }
    return res;
}

std::shared_ptr<symtable_node> AST_safe::search_only_name(const std::string &only_name)
{
    for (int i = (int)(Symtable::all_symtable_heads.size()) - 1; i >= 0; --i)
    {
        std::shared_ptr<symtable_node> sym_head = Symtable::all_symtable_heads[i];
        if (sym_head == nullptr)
            continue;
        std::shared_ptr<symtable_node> compare_now = sym_head->next;
        while (compare_now != nullptr)
        {
            if (compare_now->only_name == only_name)
                return compare_now;
            compare_now = compare_now->next;
        }
    }
    return nullptr;
}