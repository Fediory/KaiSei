/**
 * @file IR.h
 * @author Fediory Feng
 * @brief Only has IR class.
 * @date 2024/05/02
 */

#pragma once

#include "define.h"
#include <stack>

class IR
{
public:
    IR_PTR head;
    IR_PTR now_ir;
    AST_PTR AST;
    static int line_num;
    static int label_num;
    static int now_register;

    std::stack<std::string> now_continue;
    std::stack<std::string> now_break;

    explicit IR(const std::shared_ptr<AST_node> &AST_head);

    void create_empty_ir();
    void create_label(const std::string &comment, const std::string &target);
    void create_forth(const std::string &comment, const IR_tuple &target,
                      const std::string &opera, const IR_tuple &org_1 = IR_tuple(), const IR_tuple &org_2 = IR_tuple());
    void create_cast_or_assign(const std::string &comment, const IR_tuple &target, const IR_tuple &org_1);
    [[nodiscard]] IR_tuple create_cast_or_not(const std::string &comment, IR_tuple &target_and_org, const IR_tuple &sample);

    void Generate();

    void program_generate(const std::shared_ptr<AST_node> &now_AST);
    void basic_generate(const std::shared_ptr<AST_node> &now_AST);
    void function_generate(const std::shared_ptr<AST_node> &now_AST);
    void single_assign_generate(const std::shared_ptr<AST_node> &now_AST);
    void array_assign_generate(const std::shared_ptr<AST_node> &now_AST);
    void single_define_generate(const std::shared_ptr<AST_node> &now_AST);
    void array_define_generate(const std::shared_ptr<AST_node> &now_AST);
    void if_generate(const std::shared_ptr<AST_node> &now_AST);
    void while_generate(const std::shared_ptr<AST_node> &now_AST);
    void break_generate(const std::shared_ptr<AST_node> &now_AST);
    void continue_generate(const std::shared_ptr<AST_node> &now_AST);
    IR_tuple function_usage_generate(const std::shared_ptr<AST_node> &now_AST, const IR_tuple &passing_down = IR_tuple());
    IR_tuple array_usage_generate(const std::shared_ptr<AST_node> &now_AST, const IR_tuple &passing_down);
    IR_tuple expr_generate(const std::shared_ptr<AST_node> &now_AST, const IR_tuple &passing_down = IR_tuple());

    int count_array_init_block(
        const std::shared_ptr<AST_node> &init,
        const std::vector<int> &index_list,
        const IR_tuple &assign_unit, const IR_tuple &assign_target,
        int stair, int offset_base);
};