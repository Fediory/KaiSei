/**
 * @file LVA_define.cpp
 * @author Fediory Feng
 * @brief impl of BlockVariableFactory
 * @date 2024/06/05
 */

#include "define_backend.h"
#include "LVA.h"

#include <map>
#include <algorithm>
#include <iostream>
#include <fstream>

std::map<int, bool> BlockVariableFactory::CFG_change;
bool BlockVariableFactory::change = true;

bool BlockVariableFactory::detect_change()
{
    if (CFG_change.empty())
    {
        return true;
    }
    else
    {
        return std::any_of(CFG_change.begin(), CFG_change.end(), [](const std::pair<int, bool> &item) -> auto
                           { return item.second; });
    }
}

bool BlockVariableFactory::operator_filter1(const std::string &op)
{
    const std::vector<std::string> operats = {"alloca", "cast-int", "assign", "lw"};
    return std::any_of(operats.begin(), operats.end(), [&op](const std::string &str)
                       { return str == op; });
}

bool BlockVariableFactory::operator_filter2(const std::string &op)
{
    const std::vector<std::string> operats = {
        "add", "addf", "sub", "subf", "mul", "mulf", "div", "divf", "mod",
        "gr", "le", "not", "eq", "neq", "gre", "lee", "sll", "slr"};
    return std::any_of(operats.begin(), operats.end(), [&op](const std::string &str)
                       { return str == op; });
}

void BlockVariableFactory::print_all(const std::map<std::string, std::vector<CFG_PTR>> &function_chain)
{
    for (const auto &[name, blocks_chain] : function_chain)
    {
        Debug::debug_output << "## FUNCTION ## " << name << std::endl
                            << std::endl;
        BlockVariableFactory::print_block(blocks_chain);
        Debug::debug_output << std::endl
                            << std::endl;
    }
}

void BlockVariableFactory::print_block(const std::vector<std::shared_ptr<CFG_node>> &CFG_blocks_chain)
{
    for (const auto &item : CFG_blocks_chain)
    {
        item->print();
        Debug::debug_output << "Defined:" << std::endl;
        for (const auto &i : item->defined_variables)
            Debug::debug_output << i << " ";
        Debug::debug_output << std::endl;
        Debug::debug_output << "Used:" << std::endl;
        for (const auto &i : item->used_variables)
            Debug::debug_output << i << " ";
        Debug::debug_output << std::endl;
        Debug::debug_output << "In:" << std::endl;
        for (const auto &i : item->in_variables)
            Debug::debug_output << i << " ";
        Debug::debug_output << std::endl;
        Debug::debug_output << "Out:" << std::endl;
        for (const auto &i : item->out_variables)
            Debug::debug_output << i << " ";
        Debug::debug_output << std::endl
                            << std::endl;
    }
}

void LVA_safe::raise_error(const std::string &error_code)
{
    std::cout << "LVA ERROR: " << error_code << std::endl;
    Safe::GlobalError = true;
}