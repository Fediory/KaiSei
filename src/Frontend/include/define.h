/**
 * @file define.h
 * @author Fediory Feng
 * @brief include in define.cpp
 * @date 2024/05/01
 */

#pragma once
#include <string>
#include <memory>
#include <vector>
#include <fstream>

//-------------------------utils----------------------------//

namespace Safe
{
    extern bool GlobalError;
}

namespace Debug
{
    extern std::string Path;
    extern std::ofstream debug_output;

    extern std::string lex_path;
    extern std::string sym_path;
    extern std::string AST_path;
    extern void debug_out(std::string type_path);
}

enum literal_class
{
    literal_int,
    literal_float
};

// basic_function only use in pointer
enum basic_type
{
    basic_unused = 0,
    basic_any,
    basic_int,
    basic_function,
    basic_void
};

const std::string basic_type_string_name[] = {
    "unused", "any", "int", "function", "void"};

class literal_value_storage
{
private:
    bool literal_used = false;
    literal_class literal_type = literal_int;
    union
    {
        int int_value;
    } literal_value;

public:
    [[nodiscard]] std::string to_string() const;
    [[nodiscard]] bool used() const;
    void make_unused();

    void change_to_int();

    void cast_and_assign(int x);

    [[nodiscard]] enum literal_class get_literal_type() const;
    [[nodiscard]] int get_int_value() const;
};

class type_storage
{
public:
    basic_type represent_type = basic_unused;
    bool is_pointer = false;
    int pointer_nest_num = 0;

    [[nodiscard]] std::string to_string() const;
    [[nodiscard]] bool used() const;
    [[nodiscard]] bool self_check() const;
    void make_unused();

    void add_nest();
    [[nodiscard]] int nest_num() const;
};

class value_and_type_storage
{
public:
    type_storage storage_type;
    literal_value_storage storage_value;

    [[nodiscard]] std::string to_string() const;
    [[nodiscard]] bool used() const;
    void make_unused();

    void change_to_int();
    void change_to_pointer();
    void add_nest();
    [[nodiscard]] int nest_num() const;

    [[nodiscard]] int get_int_value() const;

    void parse_from_string(const std::string &str);
    void parse_from_basic_type(basic_type type, bool is_pointer = false);
};

// call it IVTT
class identify_value_type_tuple
{
public:
    // normal use
    value_and_type_storage self_storage;

    // use for function return value
    value_and_type_storage return_storage;
    // use for function parameters
    std::vector<identify_value_type_tuple> additional_storage_vector;

    // use for array length
    std::vector<int> array_length;

    [[nodiscard]] std::string to_string() const;
    [[nodiscard]] bool used() const;
    void make_unused();

    // make_unused and xxx
    void reset_and_assign_as_int(int x);
    void reset_and_parse_from_basic_type(basic_type type, bool is_pointer = false);
    void reset_and_parse_from_string(const std::string &str);

    // self_storage
    [[nodiscard]] type_storage self_type() const;
    [[nodiscard]] bool self_is_pointer() const;
    [[nodiscard]] basic_type self_basic_type() const;
    void self_change_to_int();
    void self_change_to_pointer();
    void self_add_nest();
    [[nodiscard]] int self_get_int_value() const;
    [[nodiscard]] int self_nest_num() const;

    // return_storage
    void return_storage_parse_from_string(const std::string &str);
    [[nodiscard]] type_storage return_type() const;
    [[nodiscard]] bool return_is_pointer() const;
    [[nodiscard]] basic_type return_basic_type() const;

    // additional_storage
    void additional_storage_push(const identify_value_type_tuple &IVTT);
    void additional_storage_push(const std::string &str);
    void additional_storage_pop();
    [[nodiscard]] int parameter_num() const;

    // array_length
    void array_add(const int &n);
};

//-------------------------Lexical----------------------------//

// token_node
#define TOKEN_PTR std::shared_ptr<token_node>

enum token_type
{
    NONE,
    PUNCT,
    OPERATOR,
    KEYWORD,
    NUMBER,
    IDENT
};

const std::string token_type_string_name[] = {
    "NONE\t\t",
    "PUNCT\t\t",
    "OPERATOR\t",
    "KEYWORD\t\t",
    "NUMBER\t\t",
    "IDENT\t\t"};

struct token_node
{

    // basic
    int line = 0, column = 0;
    token_type type = NONE;
    std::string data;
    std::shared_ptr<token_node> next = nullptr;

    // attribute
    identify_value_type_tuple IVTT;

    // print
    static void print_all(const std::shared_ptr<token_node> &head);
};

namespace token_safe
{
    std::shared_ptr<token_node> next(const std::shared_ptr<token_node> &now);
    std::string data(const std::shared_ptr<token_node> &node);
    token_type type(const std::shared_ptr<token_node> &now);
    bool search_data(std::shared_ptr<token_node> now, const std::string &target, const std::string &end);
}

//-------------------------Parsing----------------------------//

// symtable_node

struct symtable_node
{

    // basic
    int table_id = -1;
    bool is_head = false;
    std::string identifier_name;
    std::shared_ptr<symtable_node> next = nullptr;

    // attribute
    std::string only_name;
    identify_value_type_tuple IVTT;
    bool is_const = false;
    bool is_static = false;

    // field, use for const
    bool treat_as_constexpr = false;

    // use for ir
    std::string label_name;

    // methods
    void rename();
    void rename(const std::string &name);

    // print
    static void print(const std::shared_ptr<symtable_node> &symtable_node_head);
};

#define SYM_PTR std::shared_ptr<symtable_node>

// Symtable

#define SYMTABLE_PTR std::shared_ptr<Symtable>
class Symtable
{
public:
    static std::vector<SYM_PTR> all_symtable_heads;
    std::vector<SYM_PTR> heads_chain;
    static int table_counts;
    int table_id;
    SYM_PTR sym_head = nullptr;
    SYM_PTR sym_tail = nullptr;
    SYM_PTR my_head = nullptr;
    SYM_PTR my_tail = nullptr;

    Symtable();
    virtual ~Symtable() = default;

    void extend_from(const std::shared_ptr<Symtable> &last_symtable_ptr);
    void append(const symtable_node &append_sym_node);

    void print() const;
    void print_chain() const;
    static void print_all();
};

// AST_node
#define AST_PTR std::shared_ptr<AST_node>

enum AST_type
{
    None,
    ProgramBody,
    Number,
    Identifier,
    Expression,
    FunctionUsage,
    ArrayUsage,
    BlockStatement,
    Statement,
    KeywordStatement,
    NormalStatement,
    DeclarationStatement,
    SingleAssignment,
    ArrayAssignment,
    ArrayInitialBlock,
    SingleDefinition,
    ArrayDefinition,
    FunctionDefinition,
    FunctionParams,
    FunctionFormParam,
    Index,
};

const std::string AST_type_string_name[] = {
    "None",
    "ProgramBody",
    "Number",
    "Identifier",
    "Expression",
    "FunctionUsage",
    "ArrayUsage",
    "BlockStatement",
    "Statement",
    "KeywordStatement",
    "NormalStatement",
    "DeclarationStatement",
    "SingleAssignment",
    "ArrayAssignment",
    "ArrayInitialBlock",
    "SingleDefinition",
    "ArrayDefinition",
    "FunctionDefinition",
    "FunctionParams",
    "FunctionFormParam",
    "Index",
};



struct AST_node
{

    // normal
    AST_type type = None;
    std::string data; // normally raw data; ExpressionAST use it for operator
    std::string comment;
    std::shared_ptr<AST_node> sister = nullptr;
    std::shared_ptr<AST_node> child = nullptr;
    std::shared_ptr<AST_node> last_child = nullptr;

    // symtable
    std::shared_ptr<Symtable> symtable_ptr = nullptr;

    // attribution (same as symtable node)
    bool using_attribute = false;
    std::string name;
    std::string only_name;
    identify_value_type_tuple IVTT;
    bool is_const = false;
    bool is_static = false;

    // use for expr optimise
    bool count_expr_ending = false;

    // declaration bound sym node
    std::shared_ptr<symtable_node> declaration_bound_sym_node = nullptr;

    // methods
    static void connect_child(const std::shared_ptr<AST_node> &parent, const std::shared_ptr<AST_node> &child);
    static void reverse_connect_child(const std::shared_ptr<AST_node> &parent, const std::shared_ptr<AST_node> &child);
    void absorb_sym_attribution(const std::shared_ptr<symtable_node> &symtable_resource_node);
    void copy(const std::shared_ptr<AST_node> &AST_resource_node);

    // print
    static void print_all(const std::shared_ptr<AST_node> &now, int stage);
};

struct AST_tuple
{
    int count;
    bool judge;
};

namespace AST_safe
{
    void raise_error(const std::string &error_code, const std::shared_ptr<token_node> &token_node);
    std::shared_ptr<symtable_node> search_id_name(const std::string &search_name, const std::shared_ptr<symtable_node> &sym_head);
    std::shared_ptr<symtable_node> search_id_name(const std::string &search_name, const std::shared_ptr<Symtable> &symtable_ptr, bool without_chain = false);
    std::shared_ptr<symtable_node> search_only_name(const std::string &only_name);
    AST_tuple count_child_number(const std::shared_ptr<AST_node> &now_node);
}



// // IRGen

// enum IR_type {
//     ir_forth, ir_label
// };

// struct IR_tuple {

//     bool is_name;
//     std::string name;
//     identify_value_type_tuple IVTT;

//     [[nodiscard]] std::string to_string(bool attribute = true) const;

//     IR_tuple();
//     IR_tuple(const std::string& str, basic_type type = basic_any);
//     IR_tuple(int int_num);
//     IR_tuple(double double_num);
//     IR_tuple(basic_type pointer_represent_type);
// };

// struct IR_node {

//     // basic
//     int index = -1;
//     IR_type ir_type = ir_label;
//     std::shared_ptr<IR_node> next = nullptr;

//     // normal
//     IR_tuple target;

//     // target: "jump", "call"
//     // target + org_1: "alloca", "cast-float", "cast-int", "assign", "jumpe", "jumpn", "lw", "sw"
//     // target + org_1 + org_2: "add", "addf", "sub", "subf", "mul", "mulf", "div", "divf", "mod"
//     std::string opera;

//     IR_tuple org_1;
//     IR_tuple org_2;

//     std::string comment;

//     virtual void print() const;
//     static void print_all(const std::shared_ptr<IR_node>& IR_head);
// };

// namespace IR_safe {
//     void raise_error(const std::string& error_code);
// }

// #define IR_PTR std::shared_ptr<IR_node>