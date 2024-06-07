//
// Created by Simon on 2022/6/2.
//

#pragma once

#include "define.h"

#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <ostream>

//-------------------------basic define----------------------------//

struct BackExpression
{
    static std::set<BackExpression> AllExpressions;
    std::string opera;
    std::string org_1, org_2;
    int index{};

    explicit BackExpression(const IR_node &node)
    {
        opera = node.opera;
        org_1 = node.org_1.name;
        org_2 = node.org_2.name;
        index = node.index;
    }

    BackExpression() = default;

    friend bool operator==(const BackExpression &first, const BackExpression &second)
    {
        return (first.opera == second.opera && first.org_1 == second.org_1 && first.org_2 == second.org_2);
    }

    friend bool operator<(const BackExpression &first, const BackExpression &second)
    {
        if (first.opera + first.org_1 + first.org_2 < second.opera + second.org_1 + second.org_2)
            return true;
        return false;
    }

    BackExpression &operator=(const BackExpression &target) = default;

    friend std::ostream &operator<<(std::ostream &os, const BackExpression &expression)
    {
        os << "opera: " << expression.opera << " org_1: " << expression.org_1 << " org_2: " << expression.org_2
           << " index: " << expression.index;
        return os;
    }
};

#define EXP_PTR std::shared_ptr<BackExpression>

//-------------------------CFG----------------------------//

struct CFG_node
{
    int index = -1;
    std::string name;

    std::vector<IR_PTR> content;

    // For ActivityAnalysis
    std::set<std::string> in_variables;
    std::set<std::string> out_variables;
    std::set<std::string> used_variables;
    std::set<std::string> defined_variables;

    // For ExpressionAnalysis
    std::set<BackExpression> in_expressions;
    std::set<BackExpression> out_expressions;
    std::set<BackExpression> generated_expressions;
    std::set<BackExpression> killed_expressions;

    // save linear index of other basic block in the CFG
    std::vector<std::string> str_predecessor;
    std::vector<std::string> str_successor;
    std::vector<std::shared_ptr<CFG_node>> predecessor;
    std::vector<std::shared_ptr<CFG_node>> successor;

    virtual void print() const;
};

#define CFG_PTR std::shared_ptr<CFG_node>

namespace CFG_list
{
    void print_all(const std::vector<CFG_PTR> &CFG_blocks_chain);
    void print_all(const std::vector<std::vector<CFG_PTR>> &static_chain);
    void print_all(const std::map<std::string, std::vector<CFG_PTR>> &function_chain);
}

namespace CFG_safe
{
    void raise_error(const std::string &error_code);
}

//-------------------------LVA----------------------------//

namespace LVA_safe
{
    void raise_error(const std::string &error_code);
}

//-------------------------RA----------------------------//

enum register_name
{
    a1,
    a2,
    a3,
    a4,
    v1,
    v2,
    v3,
    v4,
    v5,
    v6,
    v7,
    v8,
    ip,
    sp,
    lr,
    pc,
    spill,
    no_name
};

struct register_id
{
    register_name type = no_name;
    int spill_len = 0;
};

const std::string register_name_str[] = {
    "a1", "a2", "a3", "a4",
    "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8",
    "ip", "sp", "lr", "pc",
    "spill",
    "no_name"};

struct IR_node_pro : public IR_node
{
    register_id tar;
    register_id src1;
    register_id src2;

    void print() const override;
};

#define IR_pro_PTR std::shared_ptr<IR_node_pro>

struct CFG_pro_node : public CFG_node
{
    std::vector<IR_pro_PTR> content_pro;
    void print() const override;
};

#define CFG_pro_PTR std::shared_ptr<CFG_pro_node>

namespace CFG_pro_list
{
    void print_all(const std::vector<CFG_pro_PTR> &CFG_pro_blocks_chain);
    void print_all(const std::vector<std::vector<CFG_pro_PTR>> &static_chain);
    void print_all(const std::map<std::string, std::vector<CFG_pro_PTR>> &function_pro_chain);
}

//-------------------------IA&ARM----------------------------//

enum arm_type
{
    arm_global_label,
    arm_func_label,
    arm_block_label,
    arm_ins,
    arm_section
};

struct ARM_node
{
    int index = -1;
    arm_type type = arm_ins;
    std::string instruction;
    std::string comment;
};

namespace ARM
{
    void print_normal_chain(std::vector<IR_pro_PTR> normal_chain);
    void print_static_chain(std::vector<IR_PTR> static_chain);
    void print_all(const std::vector<ARM_node> &ARM_code);
    void dump_all(const std::vector<ARM_node> &ARM_code, const std::string &output_filename);
};
