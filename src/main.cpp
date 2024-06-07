/**
 * @file main.cpp
 * @author Fediory Feng
 * @brief main compiler of KaiSei
 * @date 2024/05/01
 */

#include "shell.h"
#include "lexical.h"
#include "AST.h"
#include "opt.h"
#include "IR.h"
#include "CFG.h"
#include "LVA.h"
#include "RA.h"
#include "IA.h"

#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>

int main(int argc, char **argv)
{

    //-------------------------Shell----------------------------//

    // right input: ./KaiSei [options] <input_name> <output_name>

    std::string input_filename, output_filename, debug_mode;
    bool to_assembly, optimize;
    shell_input(argc, argv, input_filename, output_filename, debug_mode, optimize, to_assembly);
    if (debug_mode == "shell")
        debug_shell(input_filename, output_filename, debug_mode, optimize, to_assembly);
    if (input_filename.empty())
        return 0;
    if (Safe::GlobalError)
        return 0;

    //-------------------------Frontend----------------------------//

    // Lexical Analyze
    Lexical program_file(input_filename);
    program_file.Lexicalize();
    const TOKEN_PTR &token_head = program_file.head;
    if (debug_mode == "lex")
    {
        token_node::print_all(token_head);
        Debug::debug_out(Debug::lex_path);
        return 0;
    }

    if (Safe::GlobalError)
        return 0;

    //
    // AST && Symbol_Table
    Symtable symtable;
    ProgramAST program(token_head, symtable);
    program.Parse();
    const AST_PTR &AST_head = program.head;

    if (debug_mode == "parse")
    {
        AST_node::print_all(AST_head, 0);
        Debug::debug_out(Debug::AST_path);
        return 0;
    }

    if (debug_mode == "sym")
    {
        Symtable::print_all();
        Debug::debug_out(Debug::sym_path);
        return 0;
    }

    if (Safe::GlobalError)
        return 0;

    //
    // Semantic Check && Frontend Optimize
    Front::Optimiser::Optimize(AST_head);
    const AST_PTR &optimized_AST_head = AST_head;

    if (debug_mode == "opt")
    {
        AST_node::print_all(optimized_AST_head, 0);
        Debug::debug_out(Debug::AST_path);
        return 0;
    }

    if (debug_mode == "optsym")
    {
        Symtable::print_all();
        Debug::debug_out(Debug::sym_path);
        return 0;
    }

    if (Safe::GlobalError)
        return 0;

    //
    // IR Generation
    IR ir(optimized_AST_head);
    ir.Generate();
    const IR_PTR &IR_head = ir.head;
    if (debug_mode == "ir")
    {
        IR_node::print_all(IR_head);
        Debug::debug_out(Debug::IR_path);
        return 0;
    }

    if (Safe::GlobalError)
        return 0;

    //-------------------------Frontend----------------------------//

    // Control Flow Graph
    CFG_builder cfg_builder(IR_head);
    cfg_builder.Generate();
    auto cfg_mul_function_chain = cfg_builder.get_result_function_chain();
    auto cfg_mul_static_chain = cfg_builder.get_result_static_chain();
    auto cfg_function_name = cfg_builder.get_result_function_name();
    if (debug_mode == "cfg")
    {
        CFG_list::print_all(cfg_mul_static_chain);
        CFG_list::print_all(cfg_mul_function_chain);
        Debug::debug_out(Debug::CFG_path);
        return 0;
    }

    if (Safe::GlobalError)
        return 0;

    //
    // Live Variable Analysis
    BlockVariableFactory block_variable_factory;
    for (auto &[name, mul_block_chain] : cfg_mul_function_chain)
        block_variable_factory.analyze_block_variables(mul_block_chain);

    if (debug_mode == "lva")
    {
        BlockVariableFactory::print_all(cfg_mul_function_chain);
        Debug::debug_out(Debug::LVA_path);
    }

    if (Safe::GlobalError)
        return 0;

    //
    // Register Allocation
    RegisterAllocator register_allocator(
        cfg_mul_function_chain,
        cfg_mul_static_chain,
        cfg_function_name,
        debug_mode == "ra");
    register_allocator.Generate();
    auto ir_pro_normal_chain = register_allocator.get_result_IR_pro_normal_chain();
    auto ir_static_chain = register_allocator.get_result_IR_static_chain();
    if (debug_mode == "ra")
    {
        CFG_pro_list::print_all(register_allocator.get_result_pro_function_chain());
        Debug::debug_out(Debug::RA_path);
    }

    if (Safe::GlobalError)
        return 0;

    // Instruction Allocation
    InstructionAllocator InsAllo(ir_pro_normal_chain, ir_static_chain);
    InsAllo.Generate();
    const auto &ARM_code = InsAllo.get_result_ARM_code();
    if (debug_mode == "arm")
    {
        Debug::debug_output << "********************** static chain ***********************" << std::endl;
        ARM::print_static_chain(ir_static_chain);
        Debug::debug_output << "********************** normal chain ***********************" << std::endl;
        ARM::print_normal_chain(ir_pro_normal_chain);
        Debug::debug_output << "**********************  arm chain   ***********************" << std::endl;
        ARM::print_all(ARM_code);
        Debug::debug_out(Debug::IA_path);
        return 0;
    }

    if (Safe::GlobalError)
        return 0;

    // Dump arm32 code to .s file
    if (to_assembly){
        ARM::dump_all(ARM_code, output_filename);
        Debug::debug_out(Debug::ARM_path);
    }
        

    return 0;
}
