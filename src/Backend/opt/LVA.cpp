//
// Created by Shirone on 2022/6/9.
//
#include <map>
#include <iostream>
#include "LVA.h"

void BlockVariableFactory::calculate_defined_used(CFG_node &CFG_node)
{
    for (auto i : CFG_node.content)
    {
        if (i->opera == "sw")
        {
            // Instruction "sw" is used to put a data into where i->target points at
            // So i->target is a used variable rather than a defined one
            if (i->org_1.is_name && CFG_node.defined_variables.find(i->org_1.name) == CFG_node.defined_variables.end())
                CFG_node.used_variables.emplace(i->org_1.name);
            if (i->target.is_name && CFG_node.defined_variables.find(i->target.name) == CFG_node.defined_variables.end())
                CFG_node.used_variables.emplace(i->target.name);
        }
        else if (operator_filter1(i->opera))
        {
            // Filter operators with 1 org
            if (i->org_1.is_name && CFG_node.defined_variables.find(i->org_1.name) == CFG_node.defined_variables.end())
                CFG_node.used_variables.emplace(i->org_1.name);
            if (i->target.is_name && CFG_node.used_variables.find(i->target.name) == CFG_node.used_variables.end())
                CFG_node.defined_variables.emplace(i->target.name);
        }
        else if (operator_filter2(i->opera))
        {
            // Filter operators with 2 orgs
            if (i->org_1.is_name && CFG_node.defined_variables.find(i->org_1.name) == CFG_node.defined_variables.end())
                CFG_node.used_variables.emplace(i->org_1.name);
            if (i->org_2.is_name && CFG_node.defined_variables.find(i->org_2.name) == CFG_node.defined_variables.end())
                CFG_node.used_variables.emplace(i->org_2.name);
            if (i->target.is_name && CFG_node.used_variables.find(i->target.name) == CFG_node.used_variables.end())
                CFG_node.defined_variables.emplace(i->target.name);
        }
    }
}

void BlockVariableFactory::calculate_activity(CFG_node &CFG_node)
{
    CFG_change[CFG_node.index] = false;
    for (auto suc : CFG_node.successor)
    {
        CFG_node.out_variables.insert(suc->in_variables.begin(), suc->in_variables.end());
    }

    std::set<std::string> difference;
    std::set_difference(
        CFG_node.out_variables.begin(),
        CFG_node.out_variables.end(),
        CFG_node.defined_variables.begin(),
        CFG_node.defined_variables.end(),
        std::inserter(difference, difference.begin()));

    std::set<std::string> newIn;
    std::set_union(
        CFG_node.used_variables.begin(),
        CFG_node.used_variables.end(),
        difference.begin(),
        difference.end(),
        std::inserter(newIn, newIn.begin()));

    if (newIn != CFG_node.in_variables)
    {
        CFG_change[CFG_node.index] = true;
        CFG_node.in_variables = newIn;
    }
}

void BlockVariableFactory::analyze_block_variables(std::vector<CFG_PTR> &list)
{
    for (auto node0 : list)
    {
        BlockVariableFactory::calculate_defined_used(*node0);
    }
    while (BlockVariableFactory::detect_change())
    {
        for (auto it = list.rbegin(); it != list.rend(); it++)
        {
            BlockVariableFactory::calculate_activity(**it);
        }
    }
}
