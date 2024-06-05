/**
 * @file lexical.cpp
 * @author Fediory Feng
 * @brief all the operators, punctuation, keyword, comment list
 * @date 2024/05/01
 */

#include "lexical.h"

// operators
std::vector<char> Lexical::operator_begin_list = {'|', '&', '<', '>', '=', '!', '+', '-', '*', '/', '%', '[', ']', '(', ')'};
std::vector<char> Lexical::operator_end_list = {'|', '&', '=', '/', '*'};
std::vector<char> Lexical::double_operator_begin_list = {'|', '&', '<', '>', '=', '!', '/'};
std::vector<std::string> Lexical::single_operator_list = {"<", ">", "=", "!", "+", "-", "*", "/", "%", "[", "]", "(", ")"};
std::vector<std::string> Lexical::double_operator_list = {"||", "&&", "<=", ">=", "==", "!="};

// punc
std::vector<char> Lexical::punctuation_begin_list = {'{', '}', ';', ','};
// std::vector<std::string> Lexical::punctuation_list = {"{", "}", ";", ","};

// keyword
std::vector<std::string> Lexical::keyword_list = {"const", "int", "if", "else", "while", "break", "continue", "return"};

// comment
std::vector<std::string> Lexical::comment_list = {"//", "/*"};

// token_node
std::shared_ptr<token_node> token_safe::next(const std::shared_ptr<token_node> &now)
{
    if (now != nullptr)
        return now->next;
    return nullptr;
}

std::string token_safe::data(const std::shared_ptr<token_node> &node)
{
    if (node != nullptr)
        return node->data;
    return "";
}

token_type token_safe::type(const std::shared_ptr<token_node> &now)
{
    if (now != nullptr)
        return now->type;
    return NONE;
}

// only use in assign
bool token_safe::search_data(std::shared_ptr<token_node> now, const std::string &target, const std::string &end)
{
    while (now != nullptr)
    {
        if (token_safe::data(now) == target)
            return true;
        if (token_safe::data(now) == end)
            return false;
        now = token_safe::next(now);
    }
    return false;
}

void token_node::print_all(const std::shared_ptr<token_node> &head)
{
    std::shared_ptr<token_node> now(head);
    std::ofstream outputFile("./debug/debug.lex");
    while (now != nullptr)
    {
        outputFile << token_type_string_name[now->type] << now->line << ", " << now->column << ", " << now->data;
        if (now->type == NUMBER)
        {
            outputFile << ", " << now->IVTT.to_string();
        }
        outputFile << "\n";
        now = now->next;
    }
    outputFile.close();
}
