/**
 * @file CFG.cpp
 * @author Fediory Feng
 * @brief all Control Flow Graph related impl from backend define.h
 * @date 2024/06/05
 */

#include "CFG.h"

CFG_builder::CFG_builder(const std::shared_ptr<IR_node> &IR_head)
{
    now_CFG_blocks_chain_ptr = nullptr;
    this->IR_head = IR_head;
    basic_block_num = 0;
}

void CFG_builder::Generate()
{
    init();
    first_generate();
    second_generate();
    third_generate();
}

void CFG_builder::init()
{
    IR_PTR temp = IR_head;
    if (temp != nullptr)
        temp = temp->next;
    int i = 0;
    while (temp != nullptr)
    {
        IR_chain.push_back(temp);
        if (temp->ir_type == ir_label)
        {
            auto map_member = std::make_pair(temp->target.name, i);
            IR_maps.insert(map_member);
        }
        temp = temp->next;
        ++i;
    }
}

void CFG_builder::first_generate()
{
    bool last_no_connection_with_this = false;
    bool new_block_due_to_jump = false;
    int index = 0;
    for (const auto &i : IR_chain)
    {

        if (i->ir_type == ir_label && i->target.name == "static_data")
        {
            last_no_connection_with_this = false;
            new_block_due_to_jump = false;
            create_new_static(i, index);
        }

        if (i->ir_type == ir_label && i->target.name[0] == '@')
        {
            last_no_connection_with_this = false;
            new_block_due_to_jump = false;
            create_new_function(i, index);
        }

        if (i->ir_type == ir_label || new_block_due_to_jump)
        {
            create_new_block(i, index);
            if (!last_no_connection_with_this && last_CFG != nullptr)
                last_CFG->str_successor.push_back(now_CFG->name);
            last_no_connection_with_this = false;
            new_block_due_to_jump = false;
        }

        if (i->opera == "jump" || i->opera == "jumpr")
        {
            last_no_connection_with_this = true;
            new_block_due_to_jump = true;
            add_successor_to_now(i->target.name);
        }

        if (i->opera == "jumpe" || i->opera == "jumpn")
        {
            new_block_due_to_jump = true;
            add_successor_to_now(i->target.name);
        }

        if (i->opera == "call")
        {
            new_block_due_to_jump = true;
        }

        add_to_now(i);
        index++;
    }
}

void CFG_builder::second_generate()
{
    for (const auto &[function_name, cfg_block] : function_chain)
    {
        for (const auto &mem : cfg_block)
        {
            for (const auto &i : mem->str_successor)
            {
                auto res = IR_maps.find(i);
                if (res != IR_maps.end())
                {
                    IR_PTR ir = IR_chain[res->second];
                    auto res2 = CFG_maps.find(ir);
                    if (res2 != CFG_maps.end())
                    {
                        mem->successor.push_back(res2->second);
                        res2->second->str_predecessor.push_back(mem->name);
                    }
                    else
                        CFG_safe::raise_error("CFG_maps.find(ir) can not find");
                }
            }
        }
    }
}

void CFG_builder::third_generate()
{
    for (const auto &[function_name, cfg_block] : function_chain)
    {
        for (const auto &mem : cfg_block)
        {
            for (const auto &i : mem->str_predecessor)
            {
                auto res = IR_maps.find(i);
                if (res != IR_maps.end())
                {
                    IR_PTR ir = IR_chain[res->second];
                    auto res2 = CFG_maps.find(ir);
                    if (res2 != CFG_maps.end())
                        mem->predecessor.push_back(res2->second);
                    else
                        CFG_safe::raise_error("CFG_maps.find(ir) can not find");
                }
            }
        }
    }
}

void CFG_builder::create_new_static(const IR_PTR &connect_IR, int ir_index)
{
    last_CFG = nullptr;
    now_CFG = nullptr;
    static_chain.emplace_back(std::vector<CFG_PTR>());
    now_CFG_blocks_chain_ptr = &static_chain.back();
}

void CFG_builder::create_new_function(const IR_PTR &connect_IR, int ir_index)
{
    last_CFG = nullptr;
    now_CFG = nullptr;
    function_name.push_back(connect_IR->target.name);
    function_chain[connect_IR->target.name] = std::vector<CFG_PTR>();
    now_CFG_blocks_chain_ptr = &function_chain[connect_IR->target.name];
}

void CFG_builder::create_new_block(const IR_PTR &connect_IR, int ir_index)
{
    last_CFG = now_CFG;
    now_CFG = std::make_shared<CFG_node>();
    now_CFG->index = basic_block_num;
    if (connect_IR->ir_type == ir_label)
        now_CFG->name = connect_IR->target.name;
    else
    {
        now_CFG->name = "CFG_block_" + std::to_string(basic_block_num);
        auto map_member = std::make_pair(now_CFG->name, ir_index);
        IR_maps.insert(map_member);
    }
    now_CFG_blocks_chain_ptr->push_back(now_CFG);
    basic_block_num++;
    auto map_member = std::make_pair(connect_IR, now_CFG);
    CFG_maps.insert(map_member);
}

void CFG_builder::add_to_now(const IR_PTR &now_IR)
{
    now_CFG->content.push_back(now_IR);
}

void CFG_builder::add_successor_to_now(const std::string &CFG_str)
{
    now_CFG->str_successor.push_back(CFG_str);
}