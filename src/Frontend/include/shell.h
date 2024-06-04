/**
 * @file shell.h
 * @author Fediory Feng
 * @brief shell including in main.cpp
 * @date 2024/05/01
 */

#pragma once

#include "define.h"
#include <string>
#include <cstring>
#include <iostream>
#include <filesystem>

void shell_input(int argc, char **argv, std::string &input_filename, std::string &output_filename, std::string &debug_mode, bool &optimize, bool &to_assembly)
{
    int normal_index = 0;
    optimize = false;
    to_assembly = false;
    std::string debug_list = "shell lex parse sym opt optsym ir cfg aa reg arm";

    if (argc <= 1 || strcmp(argv[1], "--help") == 0)
    {
        std::cout << std::endl
                  << "This is compiler KaiSei!" << std::endl
                  << "Author: Fediory Feng" << std::endl
                  << "Email: yixu-nwpu@mail.nwpu.edu.cn" << std::endl
                  << std::endl
                  << std::endl
                  << "Usage: KaiSei [options] <input_name> <output_name>" << std::endl
                  << std::endl
                  << std::endl
                  << "KaiSei options: " << std::endl
                  << std::endl
                  << "--help                - Display available options." << std::endl
                  << "<input_name>          - The input file path with name." << std::endl
                  << "-o                    - If set, optimize compiler." << std::endl
                  << "-S=<output_name>      - If set, assembly to output file. Please type" << std::endl
                  << "                        output file path with name." << std::endl
                  << "--debug=<debug_mode>  - If set, then turn on the debug mode, where " << std::endl
                  << "                        debug_mode can be \"shell, lex, parse, sym, opt," << std::endl
                  << "                        optsym, ir, cfg, aa, reg, arm\". The default" << std::endl
                  << "                        debug_mode is arm, which output the arm codes. " << std::endl
                  << "                        e.g. --debug shell" << std::endl
                  << std::endl
                  << std::endl
                  << "Examples:             - ./KaiSei main.c" << std::endl
                  << "                      - ./KaiSei -o -S main.c main.S" << std::endl
                  << "                      - ./KaiSei -o -S --debug lex main.c main.S" << std::endl
                  << "                      - ./KaiSei --help" << std::endl
                  << std::endl;
    }
    else
    {
        for (int i = 1; i < argc; ++i)
        {
            std::string name(argv[i]);
            // options
            if (name.at(0) == '-')
            {
                // --xxx
                if (name.size() >= 2 && name.at(1) == '-')
                {
                    std::string slash_name = name.substr(2);
                    if (slash_name == "debug")
                    {
                        i++;
                        if (i >= argc)
                        {
                            std::cout << "Please choose a debug mode!" << std::endl;
                            return;
                        }
                        else
                        {
                            std::string debug_name(argv[i]);
                            if (debug_list.find(debug_name) == -1)
                            {
                                std::cout << "Please choose a debug mode!" << std::endl;
                                return;
                            }
                        }
                        std::string debug_name(argv[i]);
                        debug_mode = debug_name;
                        std::filesystem::path folderPath = "./debug";
                        if (!std::filesystem::exists(folderPath))
                        {
                            std::filesystem::create_directory(folderPath);
                        }
                    }
                }
                // -xxx
                else
                {
                    std::string slash_name = name.substr(1);
                    if (slash_name == "S")
                    {
                        to_assembly = true;
                    }
                    else if (slash_name == "o")
                    {
                        optimize = true;
                    }
                }
            }
            // names
            else
            {
                switch (normal_index)
                {
                case 0:
                    input_filename = name;
                    break;
                case 1:
                    output_filename = name;
                    break;
                default:
                    break;
                }
                normal_index++;
            }
        }
    }
}

void debug_shell(std::string &input_filename, std::string &output_filename, std::string &debug_mode, bool &optimize, bool &to_assembly)
{
    std::cout << "input: " << input_filename << "\noutput: " << output_filename << "\ndebug: " << debug_mode
              << "\nwhether to assembly = " << to_assembly << "\nwhether to optimize = " << optimize << std::endl;
}
