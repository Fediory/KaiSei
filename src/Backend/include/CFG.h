/**
 * @file CFG.h
 * @author Fediory Feng
 * @brief only CFG_builder in this header
 * @date 2024/06/05
 */

#pragma once
#include "define_backend.h"

class CFG_builder
{
public:
    // input
    IR_PTR IR_head;

    // result
    std::map<std::string, std::vector<CFG_PTR>> function_chain;
    std::vector<std::string> function_name;
    std::vector<std::vector<CFG_PTR>> static_chain;

    // normal methods
    explicit CFG_builder(const IR_PTR &IR_head);
    void Generate();
    [[nodiscard]] std::map<std::string, std::vector<CFG_PTR>> get_result_function_chain() const { return function_chain; }
    [[nodiscard]] std::vector<std::string> get_result_function_name() const { return function_name; }
    [[nodiscard]] std::vector<std::vector<CFG_PTR>> get_result_static_chain() const { return static_chain; }

private:
    // middle vars, ignore
    std::vector<IR_PTR> IR_chain;
    CFG_PTR last_CFG;
    CFG_PTR now_CFG;
    int basic_block_num;
    std::vector<CFG_PTR> *now_CFG_blocks_chain_ptr;
    std::unordered_map<std::string, int> IR_maps; // label -> index
    std::unordered_map<IR_PTR, CFG_PTR> CFG_maps; // ir -> cfg

    // middle methods
    void init();
    void first_generate();
    void second_generate();
    void third_generate();
    void create_new_static(const IR_PTR &connect_IR, int ir_index);
    void create_new_function(const IR_PTR &connect_IR, int ir_index);
    void create_new_block(const IR_PTR &connect_IR, int ir_index);
    void add_to_now(const IR_PTR &now_IR);
    void add_successor_to_now(const std::string &CFG_str);
};