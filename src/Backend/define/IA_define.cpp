/**
 * @file IA_define.cpp
 * @author Fediory Feng
 * @brief print arm codes
 * @date 2024/06/06
 */

#include "define_backend.h"

#include <iostream>
#include <fstream>

void ARM::print_normal_chain(std::vector<std::shared_ptr<IR_node_pro>> normal_chain)
{

    for (int i = 0; i < normal_chain.size(); i++)
    {
        normal_chain[i]->print();
    }
}

void ARM::print_static_chain(std::vector<std::shared_ptr<IR_node>> static_chain)
{

    for (int i = 0; i < static_chain.size(); i++)
    {
        static_chain[i]->print();
    }
}

void ARM::print_all(const std::vector<ARM_node> &ARM_code)
{
    if (Safe::LLVM_inline)
    {
        Debug::debug_output << inline_LLVM_code << std::endl;
    }
    else
    {
        Debug::debug_output << "\t.text" << std::endl;
    }

    for (int i = 0; i < ARM_code.size(); i++)
    {
        if (ARM_code[i].type == arm_ins || ARM_code[i].type == arm_section)
            Debug::debug_output << "\t" << ARM_code[i].instruction << std::endl;
        else
            Debug::debug_output << ARM_code[i].instruction << std::endl;
    }
}

void ARM::dump_all(const std::vector<ARM_node> &ARM_code, const std::string &output_filename)
{

    std::ofstream file;

    // default: erase existed content in the file and then write in new content
    file.open(output_filename);

    if (file.fail())
    {
        std::cout << "ERROR: object file doesn't exist!!!" << std::endl;
    }
    else
    {
        print_all(ARM_code);
    }
}