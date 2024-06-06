//
// Created by Simon on 2022/6/2.
//

// #include "Backend/include/define.h"

// #include <iostream>
// #include <fstream>






// void IR_node_pro::print() const {
//     std::cout << index << "\t";
//     if (ir_type == ir_forth) {
//         std::cout << "    ";
//         if (opera == "jump" || opera == "call")
//             std::cout << opera << " -> "
//                       << target.to_string(false);
//         else if (opera == "jumpe")
//             std::cout << opera << " -> "
//                       << target.to_string(false) << " if "
//                       << org_1.to_string() << " *" << register_name_str[src1.type] << (src1.type == spill ? std::to_string(tar.spill_len) : "") << "* == zero";
//         else if (opera == "jumpn")
//             std::cout << opera << " -> "
//                       << target.to_string(false) << " if "
//                       << org_1.to_string() << " *" << register_name_str[src1.type] << (src1.type == spill ? std::to_string(tar.spill_len) : "") << "* != zero";
//         else if (opera == "jumpr")
//             std::cout << opera << " -> "
//                       << target.to_string(false) << " *" << register_name_str[tar.type] << (tar.type == spill ? std::to_string(tar.spill_len) : "") << "*";
//         else if (opera == "assign" || opera == "sw" || opera == "lw" ||
//                  opera == "cast-int" || opera == "cast-float" ||
//                  opera == "not" ||
//                  opera == "alloc-static" || opera == "alloc-stack")
//             std::cout << target.to_string() << " *" << register_name_str[tar.type] << (tar.type == spill ? std::to_string(tar.spill_len) : "") << "*" << " = "
//                       << opera << ", "
//                       << org_1.to_string() << " *" << register_name_str[src1.type] << (src1.type == spill ? std::to_string(src1.spill_len) : "") << "*";
//         else
//             std::cout << target.to_string() << " *" << register_name_str[tar.type] << (tar.type == spill ? std::to_string(tar.spill_len) : "") << "*" << " = "
//                       << opera << ", "
//                       << org_1.to_string() << " *" << register_name_str[src1.type] << (src1.type == spill ? std::to_string(src1.spill_len) : "") << "*" << ", "
//                       << org_2.to_string() << " *" << register_name_str[src2.type] << (src2.type == spill ? std::to_string(src2.spill_len) : "") << "*";
//     }
//     else if (ir_type == ir_label) {
//         std::cout << target.to_string(false) << ":";
//     }
//     std::cout << (comment.empty() ? "" : "\t# " + comment)  << std::endl;
// }

// void CFG_pro_node::print() const {

//     std::cout << "str predecessor:";
//     for (const auto& mem : str_predecessor) {
//         std::cout << " " << mem;
//     }
//     std::cout << std::endl;

//     std::cout << "predecessor:";
//     for (const auto& mem : predecessor) {
//         std::cout << " " << mem;
//     }
//     std::cout << std::endl;

//     std::cout << "No." << index << ", " << name << ", " << this << std::endl;

//     for (const auto& mem : content_pro) {
//         mem->print();
//     }

//     std::cout << "str successor:";
//     for (const auto& mem : str_successor) {
//         std::cout << " " << mem;
//     }
//     std::cout << std::endl;

//     std::cout << "successor:";
//     for (const auto& mem : successor) {
//         std::cout << " " << mem;
//     }
//     std::cout << std::endl;
// }

// void CFG_pro_list::print_all(const std::vector<CFG_pro_PTR>& CFG_pro_blocks_chain) {
//     for (const auto& mem : CFG_pro_blocks_chain) {
//         mem->print();
//         std::cout << std::endl;
//     }
// }
// void CFG_pro_list::print_all(const std::vector<std::vector<CFG_pro_PTR>>& static_chain) {
//     for (const auto& mem : static_chain) {
//         std::cout << "## STATIC ## " << std::endl << std::endl;
//         print_all(mem);
//         std::cout << std::endl << std::endl;
//     }
// }

// void CFG_pro_list::print_all(const std::map<std::string, std::vector<CFG_pro_PTR>>& function_pro_chain) {
//     for (const auto& [name, blocks_pro_chain] : function_pro_chain) {
//         std::cout << "## FUNCTION ## " << name << std::endl << std::endl;
//         print_all(blocks_pro_chain);
//         std::cout << std::endl << std::endl;
//     }
// }


// void ARM::print_normal_chain(std::vector<std::shared_ptr<IR_node_pro>> normal_chain) {

//     for (int i = 0; i < normal_chain.size(); i++) {
//         normal_chain[i]->print();
//     }
// }

// void ARM::print_static_chain(std::vector<std::shared_ptr<IR_node>> static_chain) {

//     for (int i = 0; i < static_chain.size(); i++) {
//         static_chain[i]->print();
//     }
// }

// void ARM::print_all(const std::vector<ARM_node>& ARM_code){

//     for (int i = 0; i < ARM_code.size(); i++) {
//         if(ARM_code[i].type == arm_ins || ARM_code[i].type == arm_section)
//             std::cout << i << "                     " << ARM_code[i].instruction << std::endl;
//         else
//             std::cout << i << "           " << ARM_code[i].instruction << std::endl;
//     }
// }

// void ARM::dump_all(const std::vector<ARM_node>& ARM_code, const std::string& output_filename){

//     std::ofstream file;

//     //default: erase existed content in the file and then write in new content
//     file.open(output_filename);

//     if(file.fail()){
//         std::cout << "ERROR: object file doesn't exist!!!" << std::endl;
//     }
//     else{
//         for (int i = 0; i < ARM_code.size(); i++) {
//             if(ARM_code[i].type == arm_ins || ARM_code[i].type == arm_section)
//                 file << "       " << ARM_code[i].instruction << std::endl;
//             else
//                 file << ARM_code[i].instruction << std::endl;
//         }
//     }

// }