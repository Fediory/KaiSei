//
// Created by Tastror on 2022/6/21.
//

#pragma once

#include "define_backend.h"

class RegisterAllocator
{
public:
    std::map<std::string, std::vector<CFG_PTR>> CFG_function_chain;
    std::vector<std::string> CFG_function_name;
    std::vector<std::vector<CFG_PTR>> CFG_static_chain;
    std::map<std::string, std::vector<CFG_pro_PTR>> CFG_pro_function_chain;
    std::vector<IR_PTR> IR_static_chain;
    std::vector<IR_pro_PTR> IR_pro_normal_chain;
    explicit RegisterAllocator(
        std::map<std::string, std::vector<CFG_PTR>> CFG_function_chain,
        std::vector<std::vector<CFG_PTR>> CFG_static_chain,
        std::vector<std::string> CFG_function_name,
        bool debug) : CFG_function_chain(std::move(CFG_function_chain)),
                      CFG_static_chain(std::move(CFG_static_chain)),
                      CFG_function_name(std::move(CFG_function_name)),
                      debug(debug) {}
    void Generate();
    void init();
    void generate_global_graph(const std::string &name, const std::vector<CFG_pro_PTR> &block_chain);
    void generate_local_graph(const std::string &name, const CFG_pro_PTR &single_block);
    void handle_global_graph(const std::string &name, const std::vector<CFG_pro_PTR> &block_chain, int *counter);
    void handle_local_graph(const std::string &name, const CFG_pro_PTR &single_block, const int *counter);
    void generate_single_ir_pro(const std::string &name, const IR_pro_PTR &ir_pro);
    void generate_ir_pro_chain();
    [[nodiscard]] std::map<std::string, std::vector<CFG_pro_PTR>> get_result_pro_function_chain() const { return CFG_pro_function_chain; };
    [[nodiscard]] std::vector<IR_PTR> get_result_IR_static_chain() const { return IR_static_chain; };
    [[nodiscard]] std::vector<IR_pro_PTR> get_result_IR_pro_normal_chain() const { return IR_pro_normal_chain; };

private:
    bool debug;
    std::map<std::string, int> function_stack;
    std::map<std::string, std::map<std::string, int>> function_map_weight;
    std::map<std::string, std::map<std::string, std::set<std::string>>> function_map_neighbor_form;
    std::map<std::string, std::map<std::string, std::map<std::string, std::set<std::string>>>> function_map_local_neighbor_form;
    std::map<std::string, std::vector<std::string>> function_map_reg_var_vec;
    std::map<std::string, std::unordered_map<std::string, register_id>> function_map_var_reg_map;
};