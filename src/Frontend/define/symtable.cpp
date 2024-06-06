/**
 * @file symtable.cpp
 * @author Fediory Feng
 * @brief all symtable related impl from define.h
 * @date 2024/05/01
 */

#include "define.h"
#include <iostream>
#include <fstream>

// symtable_node

void symtable_node::rename()
{
    std::string name_pre = std::to_string(table_id);
    if (is_head)
        only_name = "_[head]_" + name_pre + "_" + identifier_name;
    else
        only_name = "@" + name_pre + "_" + identifier_name;
}

void symtable_node::rename(const std::string &name)
{
    identifier_name = name;
    std::string name_pre = std::to_string(table_id);
    if (is_head)
        only_name = "_[head]_" + name_pre + "_" + identifier_name;
    else
        only_name = "@" + name_pre + "_" + identifier_name;
}

void symtable_node::print(const std::shared_ptr<symtable_node> &symtable_node_head)
{
    SYM_PTR now = symtable_node_head;
    while (now != nullptr)
    {
        if (now->is_head)
            Debug::debug_output << "[head] ";
        Debug::debug_output << "name: " << now->identifier_name;
        Debug::debug_output << ", only_name: " << now->only_name;
        Debug::debug_output << ", IVTT: " << now->IVTT.to_string();
        if (now->is_const)
            Debug::debug_output << ", is_const";
        if (now->is_static)
            Debug::debug_output << ", is_static";
        if (now->treat_as_constexpr)
            Debug::debug_output << ", treat_as_constexpr";
        Debug::debug_output << "\n";
        now = now->next;
    }
}

// Symtable

Symtable::Symtable()
{
    table_id = table_counts;
    table_counts++;
    my_head = std::make_shared<symtable_node>();
    my_head->table_id = table_id;
    my_head->is_head = true;
    my_tail = my_head;
    heads_chain.push_back(my_head);
    all_symtable_heads.push_back(my_head);
}

void Symtable::extend_from(const std::shared_ptr<Symtable> &last_symtable_ptr)
{
    heads_chain = last_symtable_ptr->heads_chain;
    heads_chain.push_back(my_head);
    sym_tail = last_symtable_ptr->sym_tail;
    sym_head = last_symtable_ptr->sym_head;
}

void Symtable::append(const symtable_node &append_sym_node)
{
    SYM_PTR sym_ptr = std::make_shared<symtable_node>();
    *sym_ptr = append_sym_node;
    sym_ptr->table_id = table_id;
    sym_ptr->rename();
    my_tail->next = sym_ptr;
    my_tail = sym_ptr;
    my_tail->next = nullptr;
    sym_tail = my_tail;
}

void Symtable::print() const
{
    symtable_node::print(my_head);
}

void Symtable::print_chain() const
{
    for (auto &i : heads_chain)
    {
        symtable_node::print(i);
    }
}

void Symtable::print_all()
{
    for (auto &i : Symtable::all_symtable_heads)
    {
        symtable_node::print(i);
        Debug::debug_output << "\n";
    }
}