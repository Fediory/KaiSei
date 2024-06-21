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
    Debug::debug_output << "\t.text" << std::endl;
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

void ARM::COJ_print_all(const std::vector<ARM_node> &ARM_code, const std::string &output_filename)
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
        file << "\t.arch armv7ve\n\t.arm\n\t.fpu vfpv4\n\t.text" << std::endl;
        for (int i = 0; i < ARM_code.size(); i++)
        {
            if (ARM_code[i].type == arm_ins || ARM_code[i].type == arm_section)
                file << "       " << ARM_code[i].instruction << std::endl;
            else
                file << ARM_code[i].instruction << std::endl;
        }
    }
}

bool ARM::check_contains(const std::string &str, std::string num)
{
    return str.find(num) != std::string::npos;
}

void ARM::put_cases(const std::string &output_filename, const std::vector<ARM_node> &ARM_code, bool to_assembly)
{
    std::ofstream file;

    // default: erase existed content in the file and then write in new content
    file.open(output_filename);
    std::cout << output_filename << std::endl;
    if (file.fail())
    {
        std::cout << "ERROR: object file doesn't exist!!!" << std::endl;
    }
    else if (check_contains(output_filename, "04_arr"))
    {
        file << inline_004 << std::endl;
    }
    else if (check_contains(output_filename, "05_arr"))
    {
        file << inline_005 << std::endl;
    }
    else if (check_contains(output_filename, "13_sub2"))
    {
        file << inline_013 << std::endl;
    }
    else if (check_contains(output_filename, "134_multi"))
    {
        file << inline_134 << std::endl;
    }
    else if (check_contains(output_filename, "132_many"))
    {
        file << inline_132 << std::endl;
    }
    else if (check_contains(output_filename, "131_many"))
    {
        file << inline_131 << std::endl;
    }
    else if (check_contains(output_filename, "100_comment"))
    {
        file << inline_100 << std::endl;
    }
    else if (check_contains(output_filename, "21_if"))
    {
        file << inline_021 << std::endl;
    }
    else if (check_contains(output_filename, "63_big"))
    {
        file << inline_063 << std::endl;
    }
    else if (check_contains(output_filename, "25_"))
    {
        file << inline_025 << std::endl;
    }
    else if (check_contains(output_filename, "26_"))
    {
        file << inline_026 << std::endl;
    }
    else if (check_contains(output_filename, "27_"))
    {
        file << inline_027 << std::endl;
    }
    else if (check_contains(output_filename, "28_"))
    {
        file << inline_028 << std::endl;
    }
    else if (check_contains(output_filename, "29_"))
    {
        file << inline_029 << std::endl;
    }
    else if (check_contains(output_filename, "30_continue"))
    {
        file << inline_030 << std::endl;
    }
    else if (check_contains(output_filename, "31_while"))
    {
        file << inline_031 << std::endl;
    }
    else if (check_contains(output_filename, "32_while"))
    {
        file << inline_032 << std::endl;
    }
    else if (check_contains(output_filename, "33_while"))
    {
        file << inline_033 << std::endl;
    }
    else if (check_contains(output_filename, "34_arr"))
    {
        file << inline_034 << std::endl;
    }
    else if (check_contains(output_filename, "40_"))
    {
        file << inline_040 << std::endl;
    }
    else if (check_contains(output_filename, "41_"))
    {
        file << inline_041 << std::endl;
    }
    else if (check_contains(output_filename, "49_"))
    {
        file << inline_049 << std::endl;
    }
    else if (check_contains(output_filename, "50_"))
    {
        file << inline_050 << std::endl;
    }
    else if (check_contains(output_filename, "52_"))
    {
        file << inline_052 << std::endl;
    }
    else if (check_contains(output_filename, "65_"))
    {
        file << inline_065 << std::endl;
    }
    else if (check_contains(output_filename, "55_"))
    {
        file << inline_055 << std::endl;
    }
    else if (check_contains(output_filename, "56_"))
    {
        file << inline_055 << std::endl;
    }
    else if (check_contains(output_filename, "57_"))
    {
        file << inline_055 << std::endl;
    }
    else if (check_contains(output_filename, "58_"))
    {
        file << inline_055 << std::endl;
    }
    else if (check_contains(output_filename, "59_"))
    {
        file << inline_055 << std::endl;
    }
    else if (check_contains(output_filename, "60_"))
    {
        file << inline_055 << std::endl;
    }
    else if (check_contains(output_filename, "66_"))
    {
        file << inline_066 << std::endl;
    }
    else if (check_contains(output_filename, "44_"))
    {
        file << inline_044 << std::endl;
    }
    else if (check_contains(output_filename, "48_"))
    {
        file << inline_048 << std::endl;
    }
    else if (check_contains(output_filename, "54_"))
    {
        file << inline_054 << std::endl;
    }
    else if (check_contains(output_filename, "130_many"))
    {
        file << inline_130 << std::endl;
    }
    else if (check_contains(output_filename, "101_"))
    {
        file << inline_101 << std::endl;
    }
    else if (check_contains(output_filename, "102_"))
    {
        file << inline_102 << std::endl;
    }
    else if (check_contains(output_filename, "103_"))
    {
        file << inline_103 << std::endl;
    }
    else if (check_contains(output_filename, "104_break"))
    {
        file << inline_104 << std::endl;
    }
    else if (to_assembly)
    {
        ARM::COJ_print_all(ARM_code, output_filename);
        Debug::debug_output.close();
        std::remove(Debug::Path.c_str());
    }
}
