/**
 * @file IA_define.cpp
 * @author Fediory Feng
 * @brief impl of InstructionAllocator code generations
 * @date 2024/06/06
 */

#include "IA.h"

#include <iostream>

void InstructionAllocator::Generate()
{
    code_section_generate();
    data_section_generate();
}

void InstructionAllocator::data_section_generate()
{

    if (ir_static_chain.size() != 0)
    {
        ARM_node now_ARM;

        now_ARM.type = arm_section;
        now_ARM.instruction = ".data";
        ARM_chain.push_back(now_ARM);

        now_ARM.type = arm_section;
        now_ARM.instruction = ".align\t2";
        ARM_chain.push_back(now_ARM);
    }

    for (const auto &it : ir_static_chain)
    {

        if (it->ir_type == ir_label)
        {
            global_generate(it);
        }
    }
}

void InstructionAllocator::code_section_generate()
{

    ARM_node now_ARM;

    for (const auto &it : ir_pro_normal_chain)
    {
        if (it->ir_type == ir_label && it->target.name.at(0) == '@')
        {

            now_ARM.type = arm_section;
            now_ARM.instruction = ".align\t1";
            ARM_chain.push_back(now_ARM);

            int line_pos = it->target.name.find('_');
            int len = it->target.name.length();

            now_ARM.type = arm_section;
            now_ARM.instruction = ".global\t" + it->target.name.substr(line_pos + 1, len - line_pos - 1);
            ARM_chain.push_back(now_ARM);

            function_generate(it);
        }
    }
}

void InstructionAllocator::global_generate(const std::shared_ptr<IR_node> &now_IR)
{
    bool isInitialized = false;
    auto iter = now_IR->next;

    while (iter != nullptr)
    {
        if (iter->ir_type == ir_label)
            break;
        if (iter->opera == "assign")
        {
            isInitialized = true;
            break;
        }
        iter = iter->next;
    }

    if (isInitialized)
        initialized_generate(now_IR);
    else
        uninitialized_generate(now_IR);
}

void InstructionAllocator::initialized_generate(const std::shared_ptr<IR_node> &now_IR)
{

    ARM_node now_ARM;
    auto iter = now_IR->next;

    // global array
    if (iter->opera == "alloc-static")
    {
        now_ARM.type = arm_global_label;
        now_ARM.instruction = iter->target.name.erase(0, 3) + ":";
        ARM_chain.push_back(now_ARM);

        auto it = iter->next;
        while (it != nullptr && it->index < ir_static_chain.size())
        {
            if (it->opera == "assign")
            {
                now_ARM.type = arm_ins;
                if (it->org_1.IVTT.self_type().represent_type == basic_int)
                    now_ARM.instruction = ".word    " + std::to_string(it->org_1.IVTT.self_get_int_value());
                ARM_chain.push_back(now_ARM);
                Debug::debug_output << it->index << std::endl;
            }
            it = it->next;
        }
    }
    // global int
    else
    {
        now_ARM.type = arm_global_label;
        now_ARM.instruction = iter->target.name.erase(0, 3) + ":";
        ARM_chain.push_back(now_ARM);

        now_ARM.type = arm_ins;
        if (iter->org_1.IVTT.self_type().represent_type == basic_int)
            now_ARM.instruction = ".word    " + std::to_string(iter->org_1.IVTT.self_get_int_value());
        ARM_chain.push_back(now_ARM);
    }
}

void InstructionAllocator::uninitialized_generate(const std::shared_ptr<IR_node> &now_IR)
{
    ARM_node now_ARM;

    auto it = now_IR->next;
    //.comm	c,8,4
    now_ARM.type = arm_ins;
    now_ARM.instruction = ".comm    " + it->target.name.erase(0, 3) + ", " + std::to_string(it->org_1.IVTT.self_get_int_value()) + ", 4";
    ARM_chain.push_back(now_ARM);
}

void InstructionAllocator::function_generate(const std::shared_ptr<IR_node_pro> &now_IR_pro)
{

    // judge current function is Leaf or not
    bool isLeaf = true;

    for (int i = now_IR_pro->index + 1 - ir_pro_normal_chain[0]->index; i < ir_pro_normal_chain.size(); i++)
    {

        if (ir_pro_normal_chain[i]->ir_type == ir_forth && ir_pro_normal_chain[i]->opera == "jumpr")
        {
            break;
        }

        if (ir_pro_normal_chain[i]->ir_type == ir_forth && ir_pro_normal_chain[i]->opera == "call")
        {
            isLeaf = false;
            break;
        }
    }

    function_label_generate(now_IR_pro);

    function_entry_generate(now_IR_pro, isLeaf);

    // content of function
    for (int i = now_IR_pro->index + 1 - ir_pro_normal_chain[0]->index; i < ir_pro_normal_chain.size(); i++)
    {

        if (ir_pro_normal_chain[i]->ir_type == ir_forth && (ir_pro_normal_chain[i]->opera == "jumpr"))
        {
            function_exit_generate(ir_pro_normal_chain[i], isLeaf);
            break;
        }

        if (ir_pro_normal_chain[i]->ir_type == ir_label)
        {
            block_label_generate(ir_pro_normal_chain[i]);
        }

        if (ir_pro_normal_chain[i]->ir_type == ir_forth && (ir_pro_normal_chain[i]->opera == "lee" || ir_pro_normal_chain[i]->opera == "le" || ir_pro_normal_chain[i]->opera == "gr" || ir_pro_normal_chain[i]->opera == "eq"))
        {
            compare_generate(ir_pro_normal_chain[i]);
        }

        if (ir_pro_normal_chain[i]->ir_type == ir_forth && ir_pro_normal_chain[i]->opera == "call")
        {
            call_generate(ir_pro_normal_chain[i]);
        }

        if (ir_pro_normal_chain[i]->ir_type == ir_forth && (ir_pro_normal_chain[i]->opera == "jump"))
        {
            jump_generate(ir_pro_normal_chain[i]);
        }

        if (ir_pro_normal_chain[i]->ir_type == ir_forth && ir_pro_normal_chain[i]->opera == "assign")
        {
            assign_generate(ir_pro_normal_chain[i]);
        }

        if (ir_pro_normal_chain[i]->ir_type == ir_forth && (ir_pro_normal_chain[i]->opera == "sll" || ir_pro_normal_chain[i]->opera == "slr"))
        {
            shift_generate(ir_pro_normal_chain[i]);
        }

        if (ir_pro_normal_chain[i]->ir_type == ir_forth && (ir_pro_normal_chain[i]->opera == "add" || ir_pro_normal_chain[i]->opera == "sub" || ir_pro_normal_chain[i]->opera == "mul" || ir_pro_normal_chain[i]->opera == "div" || ir_pro_normal_chain[i]->opera == "mod"))
        {
            arithmetic_generate(ir_pro_normal_chain[i]);
        }

        if (ir_pro_normal_chain[i]->ir_type == ir_forth && (ir_pro_normal_chain[i]->opera == "not"))
        {
            logical_generate(ir_pro_normal_chain[i]);
        }

        if (ir_pro_normal_chain[i]->ir_type == ir_forth && ir_pro_normal_chain[i]->opera == "lw")
        {
            load_generate(ir_pro_normal_chain[i]);
        }

        if (ir_pro_normal_chain[i]->ir_type == ir_forth && ir_pro_normal_chain[i]->opera == "sw")
        {
            store_generate(ir_pro_normal_chain[i]);
        }

        if (ir_pro_normal_chain[i]->ir_type == ir_forth && ir_pro_normal_chain[i]->opera == "alloc-stack")
        {
            alloc_generate(ir_pro_normal_chain[i]);
        }
    }
}

void InstructionAllocator::function_entry_generate(const std::shared_ptr<IR_node_pro> &now_IR_pro, bool isLeaf)
{

    ARM_node now_ARM;

    if (!isLeaf)
    {
        // FIXME
        now_ARM.type = arm_ins;
        now_ARM.instruction = "push    {r7, lr}";
        ARM_chain.push_back(now_ARM);

        now_ARM.type = arm_ins;
        now_ARM.instruction = "add     r7, sp, #0";
        ARM_chain.push_back(now_ARM);
    }

    // TODO: if more than 4 params are received, the rest will be saved on stack
}

void InstructionAllocator::function_exit_generate(const std::shared_ptr<IR_node_pro> &now_IR_pro, bool isLeaf)
{

    // TODO: if more than 4 params are received, the rest will be saved on stack
    ARM_node now_ARM;

    if (isLeaf)
    {
        now_ARM.type = arm_ins;
        now_ARM.instruction = "bx      lr";
        ARM_chain.push_back(now_ARM);
    }
    else
    {
        now_ARM.type = arm_ins;
        now_ARM.instruction = "pop     {r7, pc}";
        ARM_chain.push_back(now_ARM);
    }
}

void InstructionAllocator::function_label_generate(const std::shared_ptr<IR_node_pro> &now_IR_pro)
{

    ARM_node now_ARM;
    int line_pos = now_IR_pro->target.name.find('_');
    int len = now_IR_pro->target.name.length();

    // set label
    // only save function name since '@' means comment in ARM,
    // for example: @0_main -> main:
    now_ARM.type = arm_func_label;
    now_ARM.instruction = now_IR_pro->target.name.substr(line_pos + 1, len - line_pos - 1) + ":";
    ARM_chain.push_back(now_ARM);
}

void InstructionAllocator::block_label_generate(const std::shared_ptr<IR_node_pro> &now_IR_pro)
{

    ARM_node now_ARM;

    //'.' is added since it means block entry in ARM
    now_ARM.type = arm_block_label;
    now_ARM.instruction = "." + now_IR_pro->target.name + ":";
    ARM_chain.push_back(now_ARM);
}

void InstructionAllocator::call_generate(const std::shared_ptr<IR_node_pro> &now_IR_pro)
{

    ARM_node now_ARM;
    int line_pos = now_IR_pro->target.name.find('_');
    int len = now_IR_pro->target.name.length();

    now_ARM.type = arm_ins;
    std::string call_string = "bl     ";
    now_ARM.instruction = call_string + " " + now_IR_pro->target.name.substr(line_pos + 1, len - line_pos - 1);
    ARM_chain.push_back(now_ARM);
}

void InstructionAllocator::jump_generate(const std::shared_ptr<IR_node_pro> &now_IR_pro)
{
    ARM_node now_ARM;

    now_ARM.type = arm_ins;
    std::string jump_string = "b       ";
    now_ARM.instruction = jump_string + "." + now_IR_pro->target.name;
    ARM_chain.push_back(now_ARM);
}

void InstructionAllocator::compare_generate(const std::shared_ptr<IR_node_pro> &now_IR_pro)
{
    ARM_node now_ARM;

    now_ARM.type = arm_ins;
    std::string operand2_str;

    if (now_IR_pro->org_2.is_name)
        operand2_str = register_name_str[now_IR_pro->src2.type];
    else if (now_IR_pro->org_2.IVTT.self_type().represent_type == basic_int)
        operand2_str = "#" + std::to_string(now_IR_pro->org_2.IVTT.self_get_int_value());

    now_ARM.instruction = "cmp     " + register_name_str[now_IR_pro->src1.type] + ", " + operand2_str;
    ARM_chain.push_back(now_ARM);

    auto next_IR = now_IR_pro->next;
    now_ARM.type = arm_ins;

    std::string jump_str = "bne     ";
    if (now_IR_pro->opera == "le")      // <
        jump_str = "bge     ";          // >=
    else if (now_IR_pro->opera == "eq") // ==
        jump_str = "bne     ";          // !=
    else if (now_IR_pro->opera == "gr") // >
        jump_str = "ble     ";          // <=

    now_ARM.instruction = jump_str + "." + next_IR->target.name;
    ARM_chain.push_back(now_ARM);
}

void InstructionAllocator::assign_generate(const std::shared_ptr<IR_node_pro> &now_IR_pro)
{

    ARM_node now_ARM;

    if (now_IR_pro->org_1.is_name)
    {
        now_ARM.type = arm_ins;
        now_ARM.instruction = "mov     " + register_name_str[now_IR_pro->tar.type] + ", " + register_name_str[now_IR_pro->src1.type];
        ARM_chain.push_back(now_ARM);
    }
    else
    {
        if (now_IR_pro->org_1.IVTT.self_get_int_value() >= 0)
        {
            now_ARM.type = arm_ins;
            now_ARM.instruction = "movs    " + register_name_str[now_IR_pro->tar.type] + ", #" + std::to_string(now_IR_pro->org_1.IVTT.self_get_int_value());
            ARM_chain.push_back(now_ARM);
        }
        else if (now_IR_pro->org_1.IVTT.self_get_int_value() == -1)
        {
            now_ARM.type = arm_ins;
            now_ARM.instruction = "mov     " + register_name_str[now_IR_pro->tar.type] + ", #" + std::to_string(now_IR_pro->org_1.IVTT.self_get_int_value());
            ARM_chain.push_back(now_ARM);
        }
        else
        {
            now_ARM.type = arm_ins;
            now_ARM.instruction = "mvn     " + register_name_str[now_IR_pro->tar.type] + ", #" + std::to_string(-now_IR_pro->org_1.IVTT.self_get_int_value() - 1);
            ARM_chain.push_back(now_ARM);
        }
    }
}

void InstructionAllocator::shift_generate(const std::shared_ptr<IR_node_pro> &now_IR_pro)
{
    ARM_node now_ARM;
    std::string opera_str;

    if (now_IR_pro->opera == "sll")
        opera_str = "lsl     ";
    else if (now_IR_pro->opera == "slr")
        opera_str = "lsr     ";

    now_ARM.type = arm_ins;
    now_ARM.instruction = opera_str + register_name_str[now_IR_pro->tar.type] + ", " + register_name_str[now_IR_pro->src1.type] + ", #" + std::to_string(now_IR_pro->org_2.IVTT.self_get_int_value());
    ARM_chain.push_back(now_ARM);
}

void InstructionAllocator::arithmetic_generate(const std::shared_ptr<IR_node_pro> &now_IR_pro)
{

    ARM_node now_ARM;
    if (!now_IR_pro->org_1.name.empty() && now_IR_pro->org_1.name.substr(1, 1) == "0")
    {
        now_ARM.type = arm_ins;
        now_ARM.instruction = "movw    " + register_name_str[now_IR_pro->src1.type] + ", " + "#:lower16:" + now_IR_pro->org_1.name.erase(0, 3);
        ARM_chain.push_back(now_ARM);

        now_ARM.type = arm_ins;
        now_ARM.instruction = "movt    " + register_name_str[now_IR_pro->src1.type] + ", " + "#:upper16:" + now_IR_pro->org_1.name;
        ARM_chain.push_back(now_ARM);

        if (now_IR_pro->org_1.IVTT.self_type().represent_type != basic_any)
        {
            now_ARM.type = arm_ins;
            now_ARM.instruction = "ldr     " + register_name_str[now_IR_pro->src1.type] + ", [" + register_name_str[now_IR_pro->src1.type] + "]";
            ARM_chain.push_back(now_ARM);
        }
    }

    if (!now_IR_pro->org_2.name.empty() && now_IR_pro->org_2.name.substr(1, 1) == "0")
    {
        now_ARM.type = arm_ins;
        now_ARM.instruction = "movw    " + register_name_str[now_IR_pro->src2.type] + ", " + "#:lower16:" + now_IR_pro->org_2.name.erase(0, 3);
        ARM_chain.push_back(now_ARM);

        now_ARM.type = arm_ins;
        now_ARM.instruction = "movt    " + register_name_str[now_IR_pro->src2.type] + ", " + "#:upper16:" + now_IR_pro->org_2.name;
        ARM_chain.push_back(now_ARM);

        if (now_IR_pro->org_2.IVTT.self_type().represent_type != basic_any)
        {
            now_ARM.type = arm_ins;
            now_ARM.instruction = "ldr     " + register_name_str[now_IR_pro->src2.type] + ", [" + register_name_str[now_IR_pro->src2.type] + "]";
            ARM_chain.push_back(now_ARM);
        }
    }

    now_ARM.type = arm_ins;
    std::string operand2_str;

    if (now_IR_pro->org_2.is_name)
        operand2_str = register_name_str[now_IR_pro->src2.type];
    else if (now_IR_pro->org_2.IVTT.self_type().represent_type == basic_int)
        operand2_str = "#" + std::to_string(now_IR_pro->org_2.IVTT.self_get_int_value());

    std::string opera_str;
    if (now_IR_pro->opera == "div")
        opera_str = "sdiv"; // signed division
    else
        opera_str = now_IR_pro->opera;

    now_ARM.instruction = opera_str + "     " + register_name_str[now_IR_pro->tar.type] + ", " + register_name_str[now_IR_pro->src1.type] + ", " + operand2_str;
    ARM_chain.push_back(now_ARM);
}

void InstructionAllocator::logical_generate(const std::shared_ptr<IR_node_pro> &now_IR_pro)
{
    // FIXME
    ARM_node now_ARM;
    std::string opera_str;

    if (now_IR_pro->opera == "not")
        opera_str = "mvn    ";
    else if (now_IR_pro->opera == "lee")
        opera_str = "le     ";

    now_ARM.type = arm_ins;
    now_ARM.instruction = opera_str + register_name_str[now_IR_pro->tar.type] + ", " + register_name_str[now_IR_pro->src1.type];
    ARM_chain.push_back(now_ARM);
}

void InstructionAllocator::load_generate(const std::shared_ptr<IR_node_pro> &now_IR_pro)
{
    ARM_node now_ARM;

    now_ARM.type = arm_ins;
    now_ARM.instruction = "ldr     " + register_name_str[now_IR_pro->tar.type] + ", [" + register_name_str[now_IR_pro->src1.type] + "]";
    ARM_chain.push_back(now_ARM);
}

void InstructionAllocator::store_generate(const std::shared_ptr<IR_node_pro> &now_IR_pro)
{
    ARM_node now_ARM;

    now_ARM.type = arm_ins;
    now_ARM.instruction = "str     " + register_name_str[now_IR_pro->src1.type] + ", [" + register_name_str[now_IR_pro->tar.type] + "]";
    ARM_chain.push_back(now_ARM);
}

void InstructionAllocator::alloc_generate(const std::shared_ptr<IR_node_pro> &now_IR_pro)
{
    ARM_node now_ARM;

    now_ARM.type = arm_ins;
    std::string len = std::to_string(now_IR_pro->org_1.IVTT.self_get_int_value());
    if(!(register_name_str[now_IR_pro->tar.type] == "no_name")){
        now_ARM.instruction = "sub    sp, sp, #" + len;
        ARM_chain.push_back(now_ARM);

        now_ARM.type = arm_ins;
        now_ARM.instruction = "movs    " + register_name_str[now_IR_pro->tar.type] + ", sp";
        ARM_chain.push_back(now_ARM);
    }
}
