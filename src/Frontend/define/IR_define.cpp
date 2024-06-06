/**
 * @file IR_define.cpp
 * @author Fediory Feng
 * @brief IR_tuple and IR_node and IR_safe impl from define.h
 * @date 2024/05/02
 */

#include "define.h"
#include <iostream>
#include <fstream>

std::string IR_tuple::to_string(bool attribute) const
{
    if (attribute)
        if (is_name)
            return IVTT.to_string() + "" + name;
        else
            return IVTT.to_string();
    else if (is_name)
        return name;
    else
        return IVTT.to_string();
}

IR_tuple::IR_tuple()
{
    is_name = true;
}

IR_tuple::IR_tuple(const std::string &str, basic_type type)
{
    is_name = true;
    this->name = str;
    IVTT.reset_and_parse_from_basic_type(type);
}

IR_tuple::IR_tuple(int int_num)
{
    is_name = false;
    IVTT.reset_and_assign_as_int(int_num);
}

IR_tuple::IR_tuple(basic_type pointer_represent_type)
{
    is_name = false;
    IVTT.reset_and_parse_from_basic_type(pointer_represent_type, true);
}

void IR_node::print() const
{
    if (ir_type == ir_forth)
    {
        Debug::debug_output << "    ";
        if (opera == "jump" || opera == "jumpr" || opera == "call")
            Debug::debug_output << opera << " -> "
                      << target.to_string(false);
        else if (opera == "jumpe")
            Debug::debug_output << opera << " -> "
                      << target.to_string(false) << " if "
                      << org_1.to_string() << " == zero";
        else if (opera == "jumpn")
            Debug::debug_output << opera << " -> "
                      << target.to_string(false) << " if "
                      << org_1.to_string() << " != zero";
        else if (opera == "assign" || opera == "sw" || opera == "lw" ||
                 opera == "cast-int" ||
                 opera == "not" ||
                 opera == "alloc-static" || opera == "alloc-stack")
            Debug::debug_output << target.to_string() << " = "
                      << opera << ",\t"
                      << org_1.to_string();
        else
            Debug::debug_output << target.to_string() << " = "
                      << opera << ",\t"
                      << org_1.to_string() << ",\t"
                      << org_2.to_string();
    }
    else if (ir_type == ir_label)
    {
        Debug::debug_output << target.to_string(false) << ":";
    }
    Debug::debug_output << (comment.empty() ? "" : "\t# " + comment) << std::endl;
}

void IR_node::print_all(const std::shared_ptr<IR_node> &IR_head)
{
    std::shared_ptr<IR_node> now = IR_head;
    if (now == nullptr)
        return;
    now = now->next;
    while (now != nullptr)
    {
        now->print();
        now = now->next;
    }
}

void IR_safe::raise_error(const std::string &error_code)
{
    std::cout << "IR ERROR: " << error_code << std::endl;
    Safe::GlobalError = true;
}