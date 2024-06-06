/**
 * @file LVA_define.cpp
 * @author Fediory Feng
 * @brief only class BlockVariableFactory
 * @date 2024/06/05
 */

#pragma once
#include "define_backend.h"

#include <string>
#include <map>
#include <algorithm>

class BlockVariableFactory
{
public:
    static std::map<int, bool> CFG_change;

    void analyze_block_variables(std::vector<CFG_PTR> &cfg_block_chain);
    static void print_all(const std::map<std::string, std::vector<CFG_PTR>> &function_chain);
    static void print_block(const std::vector<std::shared_ptr<CFG_node>> &CFG_blocks_chain);

private:
    // middle vars, ignore
    static bool change;

    // middle methods
    bool detect_change();
    void calculate_defined_used(CFG_node &cfgNode);
    void calculate_activity(CFG_node &cfgNode);
    bool operator_filter1(const std::string &op);
    bool operator_filter2(const std::string &op);
};
