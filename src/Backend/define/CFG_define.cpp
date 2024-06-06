/**
 * @file CFG_define.cpp
 * @author Fediory Feng
 * @brief all print functions, especially impl of printing CFG_list
 * @date 2024/06/05
 */

#include "define_backend.h"

#include <iostream>
#include <fstream>

void CFG_node::print() const
{
    Debug::debug_output << "str predecessor:";
    for (const auto &mem : str_predecessor)
    {
        Debug::debug_output << " " << mem;
    }
    Debug::debug_output << std::endl;

    Debug::debug_output << "predecessor:";
    for (const auto &mem : predecessor)
    {
        Debug::debug_output << " " << mem;
    }
    Debug::debug_output << std::endl;

    Debug::debug_output << "No." << index << ", " << name << ", " << this << std::endl;
    for (const auto &mem : content)
    {
        mem->print();
    }

    Debug::debug_output << "str successor:";
    for (const auto &mem : str_successor)
    {
        Debug::debug_output << " " << mem;
    }
    Debug::debug_output << std::endl;

    Debug::debug_output << "successor:";
    for (const auto &mem : successor)
    {
        Debug::debug_output << " " << mem;
    }
    Debug::debug_output << std::endl;
}

void CFG_list::print_all(const std::vector<CFG_PTR> &CFG_blocks_chain)
{
    for (const auto &mem : CFG_blocks_chain)
    {
        mem->print();
        Debug::debug_output << std::endl;
    }
}

void CFG_list::print_all(const std::vector<std::vector<CFG_PTR>> &static_chain)
{
    for (const auto &mem : static_chain)
    {
        Debug::debug_output << "## STATIC ## " << std::endl
                            << std::endl;
        print_all(mem);
        Debug::debug_output << std::endl
                            << std::endl;
    }
}

void CFG_list::print_all(const std::map<std::string, std::vector<CFG_PTR>> &function_chain)
{
    for (const auto &[name, blocks_chain] : function_chain)
    {
        Debug::debug_output << "## FUNCTION ## " << name << std::endl
                            << std::endl;
        print_all(blocks_chain);
        Debug::debug_output << std::endl
                            << std::endl;
    }
}

void CFG_safe::raise_error(const std::string &error_code)
{
    std::cout << "CFG Error: " << error_code << std::endl;
    Safe::GlobalError = true;
}
