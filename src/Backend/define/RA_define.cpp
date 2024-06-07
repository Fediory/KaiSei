/**
 * @file RA_define.cpp
 * @author Fediory Feng
 * @brief impl of IR_node_pro, print CFG_pro_node and CFG_pro_list
 * @date 2024/06/07
 */

#include "define_backend.h"
#include "RA.h"

#include <iostream>
#include <fstream>

void IR_node_pro::print() const
{
    if (ir_type == ir_forth)
    {
        Debug::debug_output << "    ";
        if (opera == "jump" || opera == "call")
            Debug::debug_output << opera << " -> "
                                << target.to_string(false);
        else if (opera == "jumpe")
            Debug::debug_output << opera << " -> "
                                << target.to_string(false) << " if "
                                << org_1.to_string() << " *" << register_name_str[src1.type] << (src1.type == spill ? std::to_string(tar.spill_len) : "") << "* == zero";
        else if (opera == "jumpn")
            Debug::debug_output << opera << " -> "
                                << target.to_string(false) << " if "
                                << org_1.to_string() << " *" << register_name_str[src1.type] << (src1.type == spill ? std::to_string(tar.spill_len) : "") << "* != zero";
        else if (opera == "jumpr")
            Debug::debug_output << opera << " -> "
                                << target.to_string(false) << " *" << register_name_str[tar.type] << (tar.type == spill ? std::to_string(tar.spill_len) : "") << "*";
        else if (opera == "assign" || opera == "sw" || opera == "lw" ||
                 opera == "cast-int" ||
                 opera == "not" ||
                 opera == "alloc-static" || opera == "alloc-stack")
            Debug::debug_output << target.to_string() << " *" << register_name_str[tar.type] << (tar.type == spill ? std::to_string(tar.spill_len) : "") << "*" << " = "
                                << opera << ", "
                                << org_1.to_string() << " *" << register_name_str[src1.type] << (src1.type == spill ? std::to_string(src1.spill_len) : "") << "*";
        else
            Debug::debug_output << target.to_string() << " *" << register_name_str[tar.type] << (tar.type == spill ? std::to_string(tar.spill_len) : "") << "*" << " = "
                                << opera << ", "
                                << org_1.to_string() << " *" << register_name_str[src1.type] << (src1.type == spill ? std::to_string(src1.spill_len) : "") << "*" << ", "
                                << org_2.to_string() << " *" << register_name_str[src2.type] << (src2.type == spill ? std::to_string(src2.spill_len) : "") << "*";
    }
    else if (ir_type == ir_label)
    {
        Debug::debug_output << target.to_string(false) << ":";
    }
    Debug::debug_output << (comment.empty() ? "" : "\t# " + comment) << std::endl;
}

void CFG_pro_node::print() const
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

    for (const auto &mem : content_pro)
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

void CFG_pro_list::print_all(const std::vector<CFG_pro_PTR> &CFG_pro_blocks_chain)
{
    for (const auto &mem : CFG_pro_blocks_chain)
    {
        mem->print();
        Debug::debug_output << std::endl;
    }
}
void CFG_pro_list::print_all(const std::vector<std::vector<CFG_pro_PTR>> &static_chain)
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

void CFG_pro_list::print_all(const std::map<std::string, std::vector<CFG_pro_PTR>> &function_pro_chain)
{
    for (const auto &[name, blocks_pro_chain] : function_pro_chain)
    {
        Debug::debug_output << "## FUNCTION ## " << name << std::endl
                            << std::endl;
        print_all(blocks_pro_chain);
        Debug::debug_output << std::endl
                            << std::endl;
    }
}
