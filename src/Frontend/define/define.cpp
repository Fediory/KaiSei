/**
 * @file define.cpp
 * @author Fediory Feng
 * @brief basic definations with safe, literal, type, identify values
 * @date 2024/05/01
 */

#include "define.h"
#include <iostream>

bool Safe::GlobalError = false;

std::string literal_value_storage::to_string() const
{
    if (!literal_used)
        return "NaN";
    if (literal_type == literal_int)
        return std::to_string(literal_value.int_value);
    return "";
}

bool literal_value_storage::used() const
{
    return literal_used;
}

void literal_value_storage::make_unused()
{
    literal_used = false;
}

void literal_value_storage::change_to_int()
{
    literal_type = literal_int;
}


void literal_value_storage::cast_and_assign(int x)
{
    literal_used = true;
    if (literal_type == literal_int)
        literal_value.int_value = (int)x;
}

enum literal_class literal_value_storage::get_literal_type() const
{
    return literal_type;
}

int literal_value_storage::get_int_value() const
{
    return literal_value.int_value;
}

std::string type_storage::to_string() const
{
    return "[" + basic_type_string_name[represent_type] +
           (is_pointer ? "*" + (pointer_nest_num != 0 ? "<" + std::to_string(pointer_nest_num) + ">" : "") : "") + "]";
}

bool type_storage::used() const
{
    return represent_type != basic_unused;
}

bool type_storage::self_check() const
{
    if (represent_type == basic_function && !is_pointer)
        return false;
    if (pointer_nest_num < 0)
        return false;
    return true;
}

void type_storage::make_unused()
{
    represent_type = basic_unused;
    is_pointer = false;
    pointer_nest_num = 0;
}

void type_storage::add_nest()
{
    pointer_nest_num++;
}

int type_storage::nest_num() const
{
    return pointer_nest_num;
}

std::string value_and_type_storage::to_string() const
{
    if (storage_value.used())
        return storage_type.to_string() + " " + storage_value.to_string();
    return storage_type.to_string();
}

bool value_and_type_storage::used() const
{
    return storage_type.used();
}

void value_and_type_storage::make_unused()
{
    storage_type.make_unused();
    storage_value.make_unused();
}

void value_and_type_storage::change_to_int()
{
    storage_type.represent_type = basic_int;
    storage_value.change_to_int();
}

void value_and_type_storage::change_to_pointer()
{
    storage_type.is_pointer = true;
    storage_value.change_to_int();
}

void value_and_type_storage::add_nest()
{
    storage_type.add_nest();
}

int value_and_type_storage::nest_num() const
{
    return storage_type.nest_num();
}

int value_and_type_storage::get_int_value() const
{
    return storage_value.get_int_value();
}


void value_and_type_storage::parse_from_string(const std::string &str)
{
    make_unused();
    if (str == "int")
    {
        storage_type.represent_type = basic_int;
        storage_value.change_to_int();
    }
    else if (str == "int*")
    {
        storage_type.is_pointer = true;
        storage_type.represent_type = basic_int;
        storage_value.change_to_int();
    }
    else if (str == "function" || str == "function*")
    {
        storage_type.is_pointer = true;
        storage_type.represent_type = basic_function;
        storage_value.change_to_int();
    }
    else if (str == "void")
    {
        storage_type.represent_type = basic_void;
    }
}

void value_and_type_storage::parse_from_basic_type(basic_type type, bool is_pointer)
{
    make_unused();
    storage_type.represent_type = type;
    if (is_pointer)
    {
        storage_value.change_to_int();
    }
    else
    {
        storage_value.change_to_int();
    }
}

std::string identify_value_type_tuple::to_string() const
{
    std::string res = self_storage.to_string();
    if (return_storage.used())
        res += " return(" + return_storage.to_string() + ")";
    if (!additional_storage_vector.empty())
        res += " para";
    for (const auto &i : additional_storage_vector)
        res += "(" + i.to_string() + ")";
    if (!array_length.empty())
        res += " len";
    for (const auto &i : array_length)
        res += "(" + std::to_string(i) + ")";
    return res;
}

bool identify_value_type_tuple::used() const
{
    return self_storage.used();
}

void identify_value_type_tuple::make_unused()
{
    self_storage.make_unused();
    return_storage.make_unused();
    std::vector<identify_value_type_tuple>().swap(additional_storage_vector);
}

void identify_value_type_tuple::reset_and_assign_as_int(int x)
{
    make_unused();
    self_storage.change_to_int();
    self_storage.storage_value.cast_and_assign(x);
}


void identify_value_type_tuple::reset_and_parse_from_basic_type(basic_type type, bool is_pointer)
{
    make_unused();
    self_storage.parse_from_basic_type(type, is_pointer);
}

void identify_value_type_tuple::reset_and_parse_from_string(const std::string &str)
{
    make_unused();
    self_storage.parse_from_string(str);
}

type_storage identify_value_type_tuple::self_type() const
{
    return self_storage.storage_type;
}

bool identify_value_type_tuple::self_is_pointer() const
{
    return self_storage.storage_type.is_pointer;
}

basic_type identify_value_type_tuple::self_basic_type() const
{
    return self_storage.storage_type.represent_type;
}

void identify_value_type_tuple::self_change_to_int()
{
    self_storage.change_to_int();
}


void identify_value_type_tuple::self_change_to_pointer()
{
    self_storage.change_to_pointer();
}

void identify_value_type_tuple::self_add_nest()
{
    self_storage.add_nest();
}

int identify_value_type_tuple::self_get_int_value() const
{
    return self_storage.get_int_value();
}

int identify_value_type_tuple::self_nest_num() const
{
    return self_storage.nest_num();
}

void identify_value_type_tuple::return_storage_parse_from_string(const std::string &str)
{
    return_storage.parse_from_string(str);
}

type_storage identify_value_type_tuple::return_type() const
{
    return return_storage.storage_type;
}

bool identify_value_type_tuple::return_is_pointer() const
{
    return return_storage.storage_type.is_pointer;
}

basic_type identify_value_type_tuple::return_basic_type() const
{
    return return_storage.storage_type.represent_type;
}

void identify_value_type_tuple::additional_storage_push(const identify_value_type_tuple &IVTT)
{
    additional_storage_vector.push_back(IVTT);
}

void identify_value_type_tuple::additional_storage_push(const std::string &str)
{
    identify_value_type_tuple temp;
    temp.return_storage_parse_from_string(str);
    additional_storage_vector.push_back(temp);
};

void identify_value_type_tuple::additional_storage_pop()
{
    if (additional_storage_vector.empty())
        return;
    additional_storage_vector.pop_back();
};

int identify_value_type_tuple::parameter_num() const
{
    return (int)additional_storage_vector.size();
}

void identify_value_type_tuple::array_add(const int &n)
{
    array_length.push_back(n);
}

// // Parsing: symtable_node

// void symtable_node::rename() {
//     std::string name_pre = std::to_string(table_id);
//     if (is_head) only_name = "_[head]_" + name_pre + "_" + identifier_name;
//     else only_name = "@" + name_pre + "_" + identifier_name;
// }

// void symtable_node::rename(const std::string& name) {
//     identifier_name = name;
//     std::string name_pre = std::to_string(table_id);
//     if (is_head) only_name = "_[head]_" + name_pre + "_" + identifier_name;
//     else only_name = "@" + name_pre + "_" + identifier_name;
// }

// void symtable_node::print(const std::shared_ptr<symtable_node>& symtable_node_head) {
//     SYM_PTR now = symtable_node_head;
//     while (now != nullptr) {
//         if (now->is_head) std::cout << "[head] ";
//         std::cout << "name: " << now->identifier_name;
//         std::cout << ", only_name: " << now->only_name;
//         std::cout << ", IVTT: " << now->IVTT.to_string();
//         if (now->is_const) std::cout << ", is_const";
//         if (now->is_static) std::cout << ", is_static";
//         if (now->treat_as_constexpr) std::cout << ", treat_as_constexpr";
//         std::cout << std::endl;
//         now = now->next;
//     }
// }

// // Parsing: Symtable

// Symtable::Symtable() {
//     table_id = table_counts;
//     table_counts++;
//     my_head = std::make_shared<symtable_node>();
//     my_head->table_id = table_id;
//     my_head->is_head = true;
//     my_tail = my_head;
//     heads_chain.push_back(my_head);
//     all_symtable_heads.push_back(my_head);
// }

// void Symtable::extend_from(const std::shared_ptr<Symtable>& last_symtable_ptr) {
//     heads_chain = last_symtable_ptr->heads_chain;
//     heads_chain.push_back(my_head);
//     sym_tail = last_symtable_ptr->sym_tail;
//     sym_head = last_symtable_ptr->sym_head;
// }

// void Symtable::append(const symtable_node& append_sym_node) {
//     SYM_PTR sym_ptr = std::make_shared<symtable_node>();
//     *sym_ptr = append_sym_node;
//     sym_ptr->table_id = table_id;
//     sym_ptr->rename();
//     my_tail->next = sym_ptr;
//     my_tail = sym_ptr;
//     my_tail->next = nullptr;
//     sym_tail = my_tail;
// }

// void Symtable::print() const {
//     symtable_node::print(my_head);
// }

// void Symtable::print_chain() const {
//     for (auto& i : heads_chain) {
//         symtable_node::print(i);
//     }
// }

// void Symtable::print_all() {
//     for (auto& i : Symtable::all_symtable_heads) {
//         symtable_node::print(i);
//         std::cout << std::endl;
//     }
// }

// // Parsing: AST_node

// void AST_node::print_all(const std::shared_ptr<AST_node>& now, int stage) {
//     if (now == nullptr) return;
//     for (int i = 0; i < stage; ++i)
//         std::cout << "    ";
//     std::cout << AST_type_string_name[now->type];
//     if (now->using_attribute) {
//         if (!now->only_name.empty())
//             std::cout << ", only_name: " << now->only_name;
//         if (now->declaration_bound_sym_node != nullptr)
//             std::cout << ", bound_IVTT: " << now->declaration_bound_sym_node->IVTT.to_string();
//         else
//             std::cout << ", IVTT: " << now->IVTT.to_string();
//         if (now->is_const) std::cout << ", is_const";
//         if (now->is_static) std::cout << ", is_static";
//     }
//     if (now->count_expr_ending) std::cout << ", count_expr_ending";
//     std::cout << (now->data.empty() ? "" : ", " + now->data);
//     std::cout << (now->comment.empty() ? "" : " (" + now->comment + ")");
//     // std::cout << "    #" << now << ", ch-" << now->child << ", si-" << now->sister << ", lc-" << now->last_child;
//     std::cout << std::endl;
//     print_all(now->child, stage + 1);
//     print_all(now->sister, stage);
// }

// void AST_node::print_all(const std::shared_ptr<AST_node>& AST_head) {
//     print_all(AST_head, 0);
// }

// void AST_node::connect_child(const std::shared_ptr<AST_node>& parent, const std::shared_ptr<AST_node>& child) {
//     if (parent->child == nullptr) {
//         parent->child = child;
//         parent->last_child = child;
//         // child->parent = parent;
//     } else {
//         parent->last_child->sister = child;
//         parent->last_child = child;
//         // child->parent = parent;
//     }
// }

// void AST_node::reverse_connect_child(const std::shared_ptr<AST_node>& parent, const std::shared_ptr<AST_node>& child) {
//     if (parent->child == nullptr) {
//         parent->child = child;
//         parent->last_child = child;
//         // child->parent = parent;
//     } else {
//         child->sister = parent->child;
//         parent->child = child;
//         // child->parent = parent;
//     }
// }

// void AST_node::absorb_sym_attribution(const std::shared_ptr<symtable_node>& symtable_resource_node) {
//     using_attribute = true;
//     name = symtable_resource_node->identifier_name;
//     only_name = symtable_resource_node->only_name;
//     IVTT = symtable_resource_node->IVTT;
//     is_const = symtable_resource_node->is_const;
//     is_static = symtable_resource_node->is_static;
// }

// void  AST_node::copy(const std::shared_ptr<AST_node>& AST_resource_node) {
//     type = AST_resource_node->type;
//     data = AST_resource_node->data;
//     comment = AST_resource_node->comment;
//     symtable_ptr = AST_resource_node->symtable_ptr;

//     using_attribute = AST_resource_node->using_attribute;
//     name = AST_resource_node->name;
//     only_name = AST_resource_node->only_name;
//     IVTT = AST_resource_node->IVTT;
//     is_const = AST_resource_node->is_const;
//     is_static = AST_resource_node->is_static;

//     count_expr_ending = AST_resource_node->count_expr_ending;

//     declaration_bound_sym_node = AST_resource_node->declaration_bound_sym_node;
// }

// std::shared_ptr<symtable_node> AST_safe::search_id_name(const std::string& search_name, const std::shared_ptr<symtable_node>& sym_head) {
//     if (sym_head == nullptr) return nullptr;
//     std::shared_ptr<symtable_node> compare_now = sym_head->next;
//     while (compare_now != nullptr) {
//         if (compare_now->identifier_name == search_name)
//             return compare_now;
//         compare_now = compare_now->next;
//     }
//     return nullptr;
// }

// std::shared_ptr<symtable_node> AST_safe::search_id_name(const std::string& search_name, const std::shared_ptr<Symtable>& symtable_ptr, bool without_chain) {
//     std::shared_ptr<symtable_node> res = nullptr;
//     if (without_chain)
//         res = search_id_name(search_name, symtable_ptr->my_head);
//     else
//         for (int i = (int)(symtable_ptr->heads_chain.size()) - 1; i >= 0; --i) {
//             res = search_id_name(search_name, symtable_ptr->heads_chain[i]);
//             if (res != nullptr) return res;
//         }
//     return res;
// }

// void AST_safe::RaiseError(const std::string& error_code, const std::shared_ptr<token_node>& token_node) {
//     std::cout << "ERROR: " << error_code << std::endl;
//     if (token_node != nullptr)
//         std::cout << "    where: " << token_node->data
//                   << " at line " << token_node->line << ", column "
//                   << token_node->column << std::endl;
//     else
//         std::cout << "    where: reach the end of the file" << std::endl;
//     Safe::GlobalError = true;
// }

// AST_tuple AST_safe::count_child_number(const std::shared_ptr<AST_node>& now_node) {
//     if (now_node == nullptr) return (AST_tuple){0, false};
//     AST_tuple res = {0, true};
//     AST_PTR temp = now_node->child;
//     while (temp != nullptr) {
//         res.count++;
//         if (!temp->count_expr_ending)
//             res.judge = false;
//         if (temp->declaration_bound_sym_node != nullptr) {
//             if (temp->declaration_bound_sym_node->treat_as_constexpr) {
//                 res.judge = true;
//                 temp->IVTT = temp->declaration_bound_sym_node->IVTT;
//             }
//         }
//         temp = temp->sister;
//     }
//     return res;
// }

// std::shared_ptr<symtable_node> AST_safe::search_only_name(const std::string& only_name) {
//     for (int i = (int)(Symtable::all_symtable_heads.size()) - 1; i >= 0; --i) {
//         std::shared_ptr<symtable_node> sym_head = Symtable::all_symtable_heads[i];
//         if (sym_head == nullptr) continue;
//         std::shared_ptr<symtable_node> compare_now = sym_head->next;
//         while (compare_now != nullptr) {
//             if (compare_now->only_name == only_name)
//                 return compare_now;
//             compare_now = compare_now->next;
//         }
//     }
//     return nullptr;
// }

// // Optimise

// void AST_optimize_safe::RaiseError(const std::string& error_code) {
//     std::cout << "AST Optimize ERROR: " << error_code << std::endl;
//     Safe::GlobalError = true;
// }

// // IRGen

// std::string IR_tuple::to_string(bool attribute) const {
//     if (attribute)
//         if (is_name)
//             return "{name}[" + IVTT.to_string() + "]" + name;
//         else
//             return IVTT.to_string();
//     else
//         if (is_name)
//             return name;
//         else
//             return IVTT.to_string();
// }

// IR_tuple::IR_tuple() {
//     is_name = true;
// }

// IR_tuple::IR_tuple(const std::string& str, basic_type type) {
//     is_name = true;
//     this->name = str;
//     IVTT.reset_and_parse_from_basic_type(type);
// }

// IR_tuple::IR_tuple(int int_num) {
//     is_name = false;
//     IVTT.reset_and_assign_as_int(int_num);
// }

// IR_tuple::IR_tuple(double double_num) {
//     is_name = false;
//     IVTT.reset_and_assign_as_float(double_num);
// }

// IR_tuple::IR_tuple(basic_type pointer_represent_type) {
//     is_name = false;
//     IVTT.reset_and_parse_from_basic_type(pointer_represent_type, true);
// }

// void IR_node::print() const {
//     std::cout << index << "\t";
//     if (ir_type == ir_forth) {
//         std::cout << "    ";
//         if (opera == "jump" || opera == "jumpr" || opera == "call")
//             std::cout << opera << " -> "
//                       << target.to_string(false);
//         else if (opera == "jumpe")
//             std::cout << opera << " -> "
//                       << target.to_string(false) << " if "
//                       << org_1.to_string() << " == zero";
//         else if (opera == "jumpn")
//             std::cout << opera << " -> "
//                       << target.to_string(false) << " if "
//                       << org_1.to_string() << " != zero";
//         else if (opera == "assign" || opera == "sw" || opera == "lw" ||
//                  opera == "cast-int" || opera == "cast-float" ||
//                  opera == "not" ||
//                  opera == "alloc-static" || opera == "alloc-stack")
//             std::cout << target.to_string() << " = "
//                       << opera << ", "
//                       << org_1.to_string();
//         else
//             std::cout << target.to_string() << " = "
//                       << opera << ", "
//                       << org_1.to_string() << ", "
//                       << org_2.to_string();
//     }
//     else if (ir_type == ir_label) {
//         std::cout << target.to_string(false) << ":";
//     }
//     std::cout << (comment.empty() ? "" : "\t# " + comment)  << std::endl;
// }

// void IR_node::print_all(const std::shared_ptr<IR_node>& IR_head) {
//     std::shared_ptr<IR_node> now = IR_head;
//     if (now == nullptr) return;
//     now = now->next;
//     while (now != nullptr) {
//         now->print();
//         now = now->next;
//     }
// }

// void IR_safe::RaiseError(const std::string& error_code) {
//     std::cout << "IR ERROR: " << error_code << std::endl;
//     Safe::GlobalError = true;
// }