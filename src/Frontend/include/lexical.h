/**
 * @file lexical.h
 * @author Fediory Feng
 * @brief include in lexical.cpp, only have a class "Lexical".
 * @date 2024/05/01
 */
#pragma once

#include "define.h"

#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>

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

const std::string token_type_string_name[] = {"NONE\t\t", "PUNCT\t\t", "OPERATOR\t", "KEYWORD\t\t", "NUMBER\t\t", "IDENT\t\t"};

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

class Lexical
{
protected:
    std::ifstream ifs;
    int current_char = 0;
    int look_ahead = 0;
    int line = 1;
    int column = 0;
    int delay_column = 0;
    TOKEN_PTR now_token;

    // begin list and true list

    static std::vector<char> operator_begin_list;
    static std::vector<char> operator_end_list;
    static std::vector<char> double_operator_begin_list;
    static std::vector<std::string> single_operator_list;
    static std::vector<std::string> double_operator_list;
    static std::vector<std::string> comment_list;

    static std::vector<char> punctuation_begin_list;
    // static std::vector<std::string> punctuation_list;

    static std::vector<std::string> keyword_list;

public:
    bool fail = false;
    bool end = false;
    TOKEN_PTR head;

    explicit Lexical(const std::string &input_filename)
    {
        head = std::make_shared<token_node>();
        now_token = head;
        ifs.open(input_filename, std::ios::in);
        if (!ifs.is_open())
        {
            std::cout << "Read fail or input file dosen't exist!" << std::endl;
            fail = true;
        }
        current_char = look_ahead;
        look_ahead = ifs.get();
    }

    ~Lexical() { ifs.close(); }

    void GetNext(bool update = true)
    {
        if (current_char == -1)
        {
            end = true;
            return;
        }
        current_char = look_ahead;
        look_ahead = ifs.get();
        if ((char)current_char == '\n')
        {
            line++;
            column = 0;
        }
        else
        {
            column++;
        }
        if (update)
            delay_column = column;
    }

    void raise_error(const std::string &buff) const
    {
        std::cout << "Error occurred. where: [" << buff << "] line " << line << ", "
                  << "column " << delay_column << "." << std::endl;
        Safe::GlobalError = true;
    }

    void save_node(token_type type, const std::string &data)
    {
        now_token->next = std::make_shared<token_node>();
        now_token = now_token->next;
        now_token->data = data;
        now_token->type = type;
        now_token->column = delay_column;
        now_token->line = line;
    }

    void save_int_node(int value, const std::string &data)
    {
        save_node(NUMBER, data);
        now_token->IVTT.reset_and_assign_as_int(value);
    }

    static bool is_number(char c)
    {
        if (c >= '0' && c <= '9')
            return true;
        return false;
    }

    static bool is_oct_number(char c)
    {
        if (c >= '0' && c <= '7')
            return true;
        return false;
    }

    static bool is_hex_number(char c)
    {
        if (c >= '0' && c <= '9' || c >= 'A' && c <= 'F' || c >= 'a' && c <= 'f')
            return true;
        return false;
    }

    static bool is_identifier_begin(char c)
    {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'))
            return true;
        return false;
    }

    static bool is_letter(char c)
    {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
            return true;
        return false;
    }

    static bool is_blank(char c)
    {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
            return true;
        return false;
    }

    template <typename Type>
    static bool in_list(const Type &target, const std::vector<Type> &target_list)
    {
        auto equal = [&](const Type &a)
        { return a == target; };
        return std::any_of(target_list.begin(), target_list.end(), equal);
    }

    void match_operator_or_comment()
    {
        std::string buff;
        buff.push_back((char)current_char);
        if (in_list((char)current_char, double_operator_begin_list) &&
            in_list((char)look_ahead, operator_end_list))
        {
            GetNext(false);
            buff.push_back((char)current_char);
            if (in_list(buff, double_operator_list))
            {
                save_node(OPERATOR, buff);
            }
            else if (in_list(buff, comment_list))
            {
                if (buff == "//")
                {
                    while ((char)look_ahead != '\n')
                    {
                        GetNext();
                    }
                }
                else
                {
                    while (true)
                    {
                        while ((char)look_ahead != '*')
                        {
                            GetNext();
                        }
                        GetNext();
                        if ((char)look_ahead == '/')
                        {
                            GetNext();
                            break;
                        }
                    }
                }
            }
            else
            {
                raise_error(buff);
                return;
            }
        }
        else
        {
            if (in_list(buff, single_operator_list))
            {
                save_node(OPERATOR, buff);
            }
            else
            {
                raise_error(buff);
                return;
            }
        }
    }

    void match_punctuation()
    {
        std::string buff;
        buff.push_back((char)current_char);
        save_node(PUNCT, buff);
    }

    void match_keyword_or_id()
    {
        std::string buff;
        buff.push_back((char)current_char);
        while ((char)look_ahead == '_' || is_letter((char)look_ahead) || is_number((char)look_ahead))
        {
            GetNext(false);
            buff.push_back((char)current_char);
        }
        if (in_list(buff, keyword_list))
        {
            save_node(KEYWORD, buff);
        }
        else
        {
            save_node(IDENT, buff);
        }
    }

    void match_number()
    {
        std::string buff;
        bool is_point = false;
        bool is_e = false;
        if ((char)(current_char) == '0' && ((char)(look_ahead) == 'x' || (char)(look_ahead) == 'X' || is_number((char)look_ahead)))
        {
            if ((char)(look_ahead) == 'x' || (char)(look_ahead) == 'X')
            {
                buff.push_back((char)current_char);
                GetNext(false);
                buff.push_back((char)current_char);
                while (is_hex_number((char)(look_ahead)))
                {
                    GetNext(false);
                    buff.push_back((char)current_char);
                }
            }
            else
            {
                buff.push_back((char)current_char);
                while (is_oct_number((char)(look_ahead)))
                {
                    GetNext(false);
                    buff.push_back((char)current_char);
                }
            }
        }
        else
        {
            while (true)
            {
                if ((char)current_char == '.' && !is_point)
                {
                    buff.push_back((char)current_char);
                    is_point = true;
                    if (!is_number((char)(look_ahead)))
                    {
                        raise_error(buff);
                        return;
                    }
                    while (is_number((char)(look_ahead)))
                    {
                        GetNext(false);
                        buff.push_back((char)current_char);
                    }
                }
                else if (is_number((char)current_char))
                {
                    buff.push_back((char)current_char);
                }
                else
                    break;
                if ((!is_number((char)look_ahead) && (char)look_ahead != '.') || ((char)look_ahead == '.' && is_point))
                    break;
                GetNext(false);
            }
        }
        if ((char)look_ahead == 'e' || (char)current_char == 'E')
        {
            is_e = true;
            GetNext(false);
            buff.push_back((char)current_char);
            if ((char)look_ahead == '-' || (char)look_ahead == '+')
            {
                GetNext(false);
                buff.push_back((char)current_char);
            }
            while (is_number((char)(look_ahead)))
            {
                GetNext(false);
                buff.push_back((char)current_char);
            }
        }
        if (is_point || is_e)
        {
            double value = stod(buff);
        }
        else
        {
            int value = stoi(buff, 0, 0);
            save_int_node(value, buff);
        }
    }

    void Lexicalize()
    {
        if (fail)
            return;

        while (!end)
        {

            GetNext();
            // std::cout << (char)current_char << std::endl;

            if (in_list((char)current_char, operator_begin_list))
            {
                match_operator_or_comment();
            }

            else if (in_list((char)current_char, punctuation_begin_list))
            {
                match_punctuation();
            }

            else if (is_identifier_begin((char)current_char))
            {
                match_keyword_or_id();
            }

            else if (is_number((char)current_char) || (char)current_char == '.')
            {
                match_number();
            }
        }

        head = token_safe::next(head);
    }
};
