//
// Created by Tastror on 2022/6/21.
//

#include "RA.h"

#include <iostream>

void RegisterAllocator::Generate() {

    init();

    // generate graph
    for (const auto& [name, block_chain] : CFG_pro_function_chain) {

        function_map_reg_var_vec[name].reserve(16);
        function_map_var_reg_map[name].clear();
        generate_global_graph(name, block_chain);

        for (const auto& single_block : block_chain) {
            generate_local_graph(name, single_block);

            // debug output
            if (debug)
                for (const auto& [n, d] : function_map_local_neighbor_form[name][single_block->name]) {
                     Debug::debug_output << "single_block: " << n << std::endl;
                    for (const auto& m : d) {
                         Debug::debug_output << m << ", ";
                    }
                     Debug::debug_output << std::endl;
                }
        }

        // debug output
        if (debug)
            for (const auto& [n, d] : function_map_neighbor_form[name]) {
                 Debug::debug_output << n << std::endl;
                for (const auto& m : d) {
                     Debug::debug_output << m << ", ";
                }
                 Debug::debug_output << std::endl;
            }

    }

    // handle graph, generate register
    for (const auto& [name, block_chain] : CFG_pro_function_chain) {

        // in graph
        int counter[20] = {0};
        handle_global_graph(name, block_chain, counter);
        for (const auto& single_block : block_chain) {
            handle_local_graph(name, single_block, counter);
        }

        // not in graph
        for (const auto& [n, s] : function_map_weight[name]) {

            if (n.substr(0, 4) == "$par") {
                int k = std::stoi(n.substr(4)) - 1;
                if (k < 4)
                    function_map_var_reg_map[name][n].type = (register_name) k;
                else {
                    function_map_var_reg_map[name][n].type = spill;
                    function_map_var_reg_map[name][n].spill_len = function_stack[name];
                    function_stack[name] += 4;
                }
            }
            else if (n == "$ra") {
                function_map_var_reg_map[name][n].type = lr;
            }
            else if (n == "$ret") {
                function_map_var_reg_map[name][n].type = a1;
            }
            else if (function_map_var_reg_map[name][n].type == no_name) {
                function_map_var_reg_map[name][n].type = a1;
            }

        }

        // debug-output
        if (debug)
            for (const auto& [n, d] : function_map_var_reg_map[name]) {
                 Debug::debug_output << n << ": *" << register_name_str[d.type] << d.spill_len << "*" << std::endl;
            }
    }

    // add register to ir_forth
    for (const auto& [name, block_chain] : CFG_pro_function_chain) {
        for (const auto& single_block : block_chain) {
            for (auto& x : single_block->content_pro) {
                generate_single_ir_pro(name, x);
            }
        }
    }

    // generate IR_pro_PTR chain
    generate_ir_pro_chain();

}

void RegisterAllocator::init() {

    for (auto& [name, block_chain] : CFG_function_chain) {

        CFG_pro_function_chain[name] = std::vector<CFG_pro_PTR>();
        function_stack[name] = 0;

        for (auto& mem : block_chain) {

            auto now_CFG = std::make_shared<CFG_pro_node>();

            now_CFG->index = mem->index;
            now_CFG->name = mem->name;
            //* now_CFG->content = mem->content; *//
            now_CFG->in_variables = mem->in_variables;
            now_CFG->out_variables = mem->out_variables;
            now_CFG->used_variables = mem->used_variables;
            now_CFG->defined_variables = mem->defined_variables;
            now_CFG->in_expressions = mem->in_expressions;
            now_CFG->out_expressions = mem->out_expressions;
            now_CFG->generated_expressions = mem->generated_expressions;
            now_CFG->killed_expressions = mem->killed_expressions;
            now_CFG->str_predecessor = mem->str_predecessor;
            now_CFG->str_successor = mem->str_successor;
            now_CFG->predecessor = mem->predecessor;
            now_CFG->successor = mem->successor;

            for (auto& r : mem->content) {

                auto now_IR = std::make_shared<IR_node_pro>();
                now_IR->index = r->index;
                now_IR->ir_type = r->ir_type;
                now_IR->next = r->next;
                now_IR->target = r->target;
                now_IR->opera = r->opera;
                now_IR->org_1 = r->org_1;
                now_IR->org_2 = r->org_2;
                now_IR->comment = r->comment;
                now_CFG->content_pro.push_back(now_IR);
            }

            CFG_pro_function_chain[name].push_back(now_CFG);
        }
    }
}


// in each CFG_pro_block
// there are defined, used, in, out

void RegisterAllocator::generate_global_graph(const std::string& name, const std::vector<CFG_pro_PTR>& block_chain) {

    for (const auto& mem : block_chain) {

        const auto& in = mem->in_variables;
        const auto& out = mem->out_variables;
        const auto& def = mem->defined_variables;
        const auto& use = mem->used_variables;

        for (const auto& var : in) {
            function_map_var_reg_map[name][var].type = no_name;
            function_map_weight[name][var] = 0;
        }
        for (const auto& var : out) {
            function_map_var_reg_map[name][var].type = no_name;
            function_map_weight[name][var] = 0;
        }
        for (const auto& var : def) {
            function_map_var_reg_map[name][var].type = no_name;
            function_map_weight[name][var] = 0;
        }
        for (const auto& var : use) {
            function_map_var_reg_map[name][var].type = no_name;
            function_map_weight[name][var] = 0;
        }

        for (const auto& var : in) {
            for (const auto& var_2 : in) {
                if (var != var_2)
                    function_map_neighbor_form[name][var].insert(var_2);
            }
        }
    }
}

void RegisterAllocator::generate_local_graph(const std::string& name, const CFG_pro_PTR& single_block) {

    std::map<std::string, std::set<std::string>> local_neighbor_form;

    for (const auto& sen : single_block->content_pro) {
        if (sen->org_1.is_name && !sen->org_1.name.empty() && sen->org_2.is_name && !sen->org_2.name.empty()) {
            local_neighbor_form[sen->org_1.name].insert(sen->org_2.name);
            local_neighbor_form[sen->org_2.name].insert(sen->org_1.name);
        }
        if (sen->opera == "sw" || sen->opera == "lw") {
            local_neighbor_form[sen->target.name].insert(sen->org_1.name);
            local_neighbor_form[sen->org_1.name].insert(sen->target.name);
        }
    }

    function_map_local_neighbor_form[name][single_block->name] = local_neighbor_form;
}

void RegisterAllocator::handle_global_graph(const std::string& name, const std::vector<CFG_pro_PTR>& block_chain, int* counter) {
    const std::map<std::string, std::set<std::string>>& global_neighbor_form = function_map_neighbor_form[name];

    // TODO: re-allocate using the priority weight, not the alphabet (in map)
    for (const auto& [n, s] : global_neighbor_form) {
        if (n == "$ra" || n == "$ret") continue;
        for (const auto& r : s) {
            counter[function_map_var_reg_map[name][r].type]++;
        }
        int i;
        for (i = v1; i <= v7; ++i) {
            if (counter[i] == 0)
                break;
        }
        if (i > v7) {
            i = spill;
            function_map_var_reg_map[name][n].spill_len = function_stack[name];
            function_stack[name] += 4;
        }
        function_map_var_reg_map[name][n].type = (register_name)i;
    }
}

void RegisterAllocator::handle_local_graph(const std::string& name, const CFG_pro_PTR& single_block, const int* counter) {
    const std::map<std::string, std::set<std::string>>& local_neighbor_form = function_map_local_neighbor_form[name][single_block->name];
    for (const auto& [n, s] : local_neighbor_form) {
        if (function_map_var_reg_map[name][n].type != no_name) continue;
        int temp[20];
        for (int i = 0; i < 20; ++i) temp[i] = counter[i];
        for (const auto& r : s) {
            temp[function_map_var_reg_map[name][r].type]++;
        }
        int i;
        for (i = a1; i <= v7; ++i) {
            if (temp[i] == 0)
                break;
        }
        if (i > v7) {
            i = spill;
            function_map_var_reg_map[name][n].spill_len = function_stack[name];
            function_stack[name] += 4;
        }
        function_map_var_reg_map[name][n].type = (register_name)i;
    }
}

void RegisterAllocator::generate_single_ir_pro(const std::string& name, const IR_pro_PTR& ir_pro) {

    if (ir_pro->ir_type == ir_forth) {
        if (ir_pro->target.is_name)
            ir_pro->tar = function_map_var_reg_map[name][ir_pro->target.name];
        if (ir_pro->org_1.is_name)
            ir_pro->src1 = function_map_var_reg_map[name][ir_pro->org_1.name];
        if (ir_pro->org_2.is_name)
            ir_pro->src2 = function_map_var_reg_map[name][ir_pro->org_2.name];
    }

}

void RegisterAllocator::generate_ir_pro_chain() {
    for (const auto& n : CFG_function_name) {
        for (const auto& b : CFG_pro_function_chain[n]) {
            for (const auto& i : b->content_pro) {
                IR_pro_normal_chain.push_back(i);
            }
        }
    }

    for (const auto& s : CFG_static_chain) {
        for (const auto& b : s) {
            for (const auto& i : b->content) {
                IR_static_chain.push_back(i);
            }
        }
    }

}