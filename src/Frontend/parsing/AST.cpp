/**
 * @file AST.cpp
 * @author Fediory Feng
 * @brief implementation of headers of AST related
 * @date 2024/05/01
 */
#include "AST.h"
#include "expr.h"
#include "keystmt.h"

std::vector<SYM_PTR> Symtable::all_symtable_heads;
int Symtable::table_counts = 0;

TOKEN_PTR SingleAssignmentAST::Parse()
{
    head->type = SingleAssignment;

    if (token_safe::type(now_token) != IDENT)
    {
        AST_safe::raise_error("in SingleAssignment, beginning is not a valid name", now_token);
        return now_token;
    }

    // v --- sym search & attribution --- v //
    SYM_PTR temp_sym_node = AST_safe::search_id_name(token_safe::data(now_token), symtable_ptr);
    if (temp_sym_node == nullptr)
    {
        AST_safe::raise_error("Usage without definition", now_token);
        GoNext(); // ! important
        return now_token;
    }
    head->absorb_sym_attribution(temp_sym_node);
    head->declaration_bound_sym_node = temp_sym_node;
    // ^ --- sym search & attribution--- ^ //

    AST_PTR var_name = std::make_shared<AST_node>();
    AST_node::connect_child(head, var_name);
    var_name->type = Identifier;
    var_name->data = token_safe::data(now_token);
    GoNext();

    if (token_safe::data(now_token) != "=")
    {
        AST_safe::raise_error("in SingleAssignment, lost punctuation [=]", now_token);
        return now_token;
    }
    GoNext();

    ExpressionAST expr(now_token, symtable_ptr);
    AST_node::connect_child(head, expr.head);
    expr.head->data = "value";
    now_token = expr.Parse();
    next_token = token_safe::next(now_token);

    // meddle check, may cause error, could be removed
    if (token_safe::data(now_token) != "," && token_safe::data(now_token) != ";")
    {
        AST_safe::raise_error("in SingleAssignment, lost punctuation [;] or [,]", now_token);
        return now_token;
    }

    return now_token;
}

TOKEN_PTR ArrayAssignmentAST::Parse()
{
    head->type = ArrayAssignment;

    if (token_safe::type(now_token) != IDENT)
    {
        AST_safe::raise_error("in ArrayAssignment, beginning is not a valid name", now_token);
        return now_token;
    }

    // v --- sym search & attribution --- v //
    SYM_PTR temp_sym_node = AST_safe::search_id_name(token_safe::data(now_token), symtable_ptr);
    if (temp_sym_node == nullptr)
    {
        AST_safe::raise_error("Usage without definition", now_token);
        GoNext(); // ! important
        return now_token;
    }
    head->absorb_sym_attribution(temp_sym_node);
    head->declaration_bound_sym_node = temp_sym_node;
    // ^ --- sym search & attribution--- ^ //

    AST_PTR var_name = std::make_shared<AST_node>();
    AST_node::connect_child(head, var_name);
    var_name->type = Identifier;
    var_name->data = token_safe::data(now_token);
    GoNext();

    if (token_safe::data(now_token) != "[")
    {
        AST_safe::raise_error("in ArrayAssignment, lost punctuation [[]", now_token);
        return now_token;
    }
    GoNext();

    AST_PTR index_ast = std::make_shared<AST_node>();
    AST_node::connect_child(head, index_ast);
    index_ast->type = Index;

    ExpressionAST index(now_token, symtable_ptr);
    AST_node::connect_child(index_ast, index.head);
    index.head->data = "value";
    index.head->comment = "index";
    now_token = index.Parse();
    next_token = token_safe::next(now_token);

    if (token_safe::data(now_token) != "]")
    {
        AST_safe::raise_error("in ArrayAssignment, lost punctuation []]", now_token);
        return now_token;
    }
    GoNext();

    while (token_safe::data(now_token) == "[")
    {
        GoNext();

        ExpressionAST index_addi(now_token, symtable_ptr);
        AST_node::connect_child(index_ast, index_addi.head);
        index_addi.head->data = "value";
        index_addi.head->comment = "index";
        now_token = index_addi.Parse();
        next_token = token_safe::next(now_token);

        if (token_safe::data(now_token) != "]")
        {
            AST_safe::raise_error("in ArrayAssignment, lost punctuation []]", now_token);
            return now_token;
        }
        GoNext();
    }

    if (token_safe::data(now_token) != "=")
    {
        AST_safe::raise_error("in ArrayAssignment, lost punctuation [=]", now_token);
        return now_token;
    }
    GoNext();

    ExpressionAST expr(now_token, symtable_ptr);
    AST_node::connect_child(head, expr.head);
    expr.head->data = "value";
    now_token = expr.Parse();
    next_token = token_safe::next(now_token);

    // meddle check, may cause error, could be removed
    if (token_safe::data(now_token) != "," && token_safe::data(now_token) != ";")
    {
        AST_safe::raise_error("in ArrayAssignment, lost punctuation [;] or [,]", now_token);
        return now_token;
    }

    return now_token;
}

TOKEN_PTR SingleDefinitionAST::Parse()
{
    head->type = SingleDefinition;

    if (token_safe::type(now_token) != IDENT)
    {
        AST_safe::raise_error("in SingleDefinition, beginning is not a valid name", now_token);
        return now_token;
    }
    AST_PTR var_name = std::make_shared<AST_node>();
    AST_node::connect_child(head, var_name);
    var_name->type = Identifier;
    var_name->data = token_safe::data(now_token);
    GoNext();

    if (token_safe::data(now_token) == "=")
    {
        GoNext();

        ExpressionAST expr(now_token, symtable_ptr);
        AST_node::connect_child(head, expr.head);
        expr.head->data = "value";
        now_token = expr.Parse();
        next_token = token_safe::next(now_token);
    }

    // meddle check, may cause error, could be removed
    if (token_safe::data(now_token) != "," && token_safe::data(now_token) != ";")
    {
        AST_safe::raise_error("in SingleDefinition, lost punctuation [;] or [,]", now_token);
        return now_token;
    }

    return now_token;
}

TOKEN_PTR ArrayInitialBlockAST::Parse()
{
    head->type = ArrayInitialBlock;

    if (token_safe::data(now_token) != "{")
    {
        AST_safe::raise_error("in ArrayInitialBlock, lost punctuation [{]", now_token);
        return now_token;
    }
    GoNext();

    if (token_safe::data(now_token) == "}")
    {
        GoNext();
        return now_token;
    }

    else if (token_safe::data(now_token) == "{")
    {
        ArrayInitialBlockAST recur_array_init(now_token, symtable_ptr);
        AST_node::connect_child(head, recur_array_init.head);
        now_token = recur_array_init.Parse();
        next_token = token_safe::next(now_token);
    }

    else
    {
        ExpressionAST index(now_token, symtable_ptr);
        AST_node::connect_child(head, index.head);
        index.head->data = "value";
        now_token = index.Parse();
        next_token = token_safe::next(now_token);
    }

    while (true)
    {

        if (token_safe::data(now_token) == "}")
        {
            GoNext();
            break;
        }

        else if (token_safe::data(now_token) != ",")
        {
            AST_safe::raise_error("in ArrayInitialBlock, punctuation should be [{] or [,]", now_token);
            return now_token;
        }
        GoNext();

        if (token_safe::data(now_token) == "{")
        {
            ArrayInitialBlockAST recur_array_init(now_token, symtable_ptr);
            AST_node::connect_child(head, recur_array_init.head);
            now_token = recur_array_init.Parse();
            next_token = token_safe::next(now_token);
        }

        else
        {
            ExpressionAST index(now_token, symtable_ptr);
            AST_node::connect_child(head, index.head);
            index.head->data = "value";
            now_token = index.Parse();
            next_token = token_safe::next(now_token);
        }
    }

    return now_token;
}

TOKEN_PTR ArrayDefinitionAST::Parse()
{
    head->type = ArrayDefinition;

    if (token_safe::type(now_token) != IDENT)
    {
        AST_safe::raise_error("in ArrayDefinition, beginning is not a valid name", now_token);
        return now_token;
    }
    AST_PTR var_name = std::make_shared<AST_node>();
    AST_node::connect_child(head, var_name);
    var_name->type = Identifier;
    var_name->data = token_safe::data(now_token);
    GoNext();

    if (token_safe::data(now_token) != "[")
    {
        AST_safe::raise_error("in ArrayDefinition, lost punctuation [[]", now_token);
        return now_token;
    }

    AST_PTR index_ast = std::make_shared<AST_node>();
    AST_node::connect_child(head, index_ast);
    index_ast->type = Index;

    // v --- sym change --- v //
    symtable_ptr->my_tail->IVTT.self_add_nest();
    // ^ --- sym change --- ^//

    GoNext();

    ExpressionAST index(now_token, symtable_ptr);
    AST_node::connect_child(index_ast, index.head);
    index.head->data = "value";
    index.head->comment = "index";
    now_token = index.Parse();
    next_token = token_safe::next(now_token);

    if (token_safe::data(now_token) != "]")
    {
        AST_safe::raise_error("in ArrayDefinition, lost punctuation []]", now_token);
        return now_token;
    }
    GoNext();

    while (token_safe::data(now_token) == "[")
    {

        // v --- sym change --- v //
        symtable_ptr->my_tail->IVTT.self_add_nest();
        // ^ --- sym change --- ^//

        GoNext();

        ExpressionAST index_addi(now_token, symtable_ptr);
        AST_node::connect_child(index_ast, index_addi.head);
        index_addi.head->data = "value";
        index_addi.head->comment = "index";
        now_token = index_addi.Parse();
        next_token = token_safe::next(now_token);

        if (token_safe::data(now_token) != "]")
        {
            AST_safe::raise_error("in ArrayDefinition, lost punctuation []]", now_token);
            return now_token;
        }
        GoNext();
    }

    if (token_safe::data(now_token) == "=")
    {
        GoNext();

        ArrayInitialBlockAST array_init(now_token, symtable_ptr);
        AST_node::connect_child(head, array_init.head);
        now_token = array_init.Parse();
        next_token = token_safe::next(now_token);
    }

    // meddle check, may cause error, could be removed
    if (token_safe::data(now_token) != "," && token_safe::data(now_token) != ";")
    {
        AST_safe::raise_error("in Definition, lost punctuation [;] or [,]", now_token);
        return now_token;
    }

    return now_token;
}

TOKEN_PTR DeclarationStatementAST::Parse()
{
    head->type = DeclarationStatement;

    // v --- sym --- v //
    symtable_node placeholder_sym_node;
    placeholder_sym_node.is_static = head->is_static;
    // ^ --- sym --- ^ //

    if (token_safe::data(now_token) != "const" && token_safe::data(now_token) != "int")
    {
        AST_safe::raise_error("in DeclarationStatement, beginning is not [const] or [int]", now_token);
        return now_token;
    }

    if (token_safe::data(now_token) == "const")
    {
        head->data = "const";

        // v --- sym change --- v //
        placeholder_sym_node.is_const = true;
        // ^ --- sym change --- ^ //

        GoNext();
    }

    if (token_safe::data(now_token) != "int")
    {
        AST_safe::raise_error("in DeclarationStatement, type is not [int]", now_token);
        return now_token;
    }
    // v --- useless, since placeholder_sym_node take place of its work --- v //
    // AST_PTR var_type = std::make_shared<AST_node>();
    // AST_node::connect_child(head, var_type);
    // var_type->type = BasicType;
    // var_type->data = token_safe::data(now_token);

    // v --- sym change --- v //
    placeholder_sym_node.IVTT.reset_and_parse_from_string(token_safe::data(now_token));
    // ^ --- sym change --- ^ //

    GoNext();

    if (token_safe::type(now_token) == IDENT)
    {

        // v --- sym search --- v //
        SYM_PTR temp_sym_node = AST_safe::search_id_name(token_safe::data(now_token), symtable_ptr, true);
        if (temp_sym_node != nullptr)
        {
            AST_safe::raise_error("Redefinition", now_token);
            return now_token;
        }
        // ^ --- sym search --- ^ //

        // v --- sym --- v //
        symtable_node sym_node;
        sym_node.identifier_name = token_safe::data(now_token);
        sym_node.is_static = placeholder_sym_node.is_static;
        sym_node.is_const = placeholder_sym_node.is_const;
        sym_node.IVTT = placeholder_sym_node.IVTT;
        // ^ --- sym --- ^ //

        if (token_safe::data(next_token) == "[")
        {

            // v --- sym change --- v //
            sym_node.IVTT.self_change_to_pointer();
            // ^ --- sym change --- ^ //

            // v --- sym append --- v //
            symtable_ptr->append(sym_node);
            // ^ --- sym append --- ^ //

            ArrayDefinitionAST array_def(now_token, symtable_ptr);
            AST_node::connect_child(head, array_def.head);
            now_token = array_def.Parse();
            next_token = token_safe::next(now_token);

            // v --- sym attribute --- v //
            array_def.head->absorb_sym_attribution(symtable_ptr->my_tail);
            array_def.head->declaration_bound_sym_node = symtable_ptr->my_tail;
            // ^ --- sym attribute --- ^ //
        }

        else
        {

            SingleDefinitionAST single_def(now_token, symtable_ptr);
            AST_node::connect_child(head, single_def.head);
            now_token = single_def.Parse();
            next_token = token_safe::next(now_token);

            // v --- sym append & attribute --- v //
            symtable_ptr->append(sym_node);
            single_def.head->absorb_sym_attribution(symtable_ptr->my_tail);
            single_def.head->declaration_bound_sym_node = symtable_ptr->my_tail;
            // ^ --- sym append & attribute --- ^ //
        }
    }

    else
    {
        AST_safe::raise_error("in DeclarationStatement, missing identify name", now_token);
        return now_token;
    }

    while (true)
    {

        if (token_safe::data(now_token) == ";")
        {
            GoNext();
            break;
        }

        else if (token_safe::data(now_token) == ",")
        {
            GoNext();
        }

        else
        {
            AST_safe::raise_error("in DeclarationStatement, separate punctuation should be [,]", now_token);
            return now_token;
        }

        if (token_safe::type(now_token) == IDENT)
        {

            // v --- sym search --- v //
            SYM_PTR temp_sym_node = AST_safe::search_id_name(token_safe::data(now_token), symtable_ptr, true);
            if (temp_sym_node != nullptr)
            {
                AST_safe::raise_error("Redefinition", now_token);
                return now_token;
            }
            // ^ --- sym search --- ^ //

            // v --- sym --- v //
            symtable_node sym_node;
            sym_node.identifier_name = token_safe::data(now_token);
            sym_node.is_static = placeholder_sym_node.is_static;
            sym_node.is_const = placeholder_sym_node.is_const;
            sym_node.IVTT = placeholder_sym_node.IVTT;
            // ^ --- sym --- ^ //

            if (token_safe::data(next_token) == "[")
            {

                // v --- sym change --- v //
                sym_node.IVTT.self_change_to_pointer();
                // ^ --- sym change --- ^ //

                // v --- sym append --- v //
                symtable_ptr->append(sym_node);
                // ^ --- sym append --- ^ //

                ArrayDefinitionAST array_def(now_token, symtable_ptr);
                AST_node::connect_child(head, array_def.head);
                now_token = array_def.Parse();
                next_token = token_safe::next(now_token);

                // v --- sym attribute --- v //
                array_def.head->absorb_sym_attribution(symtable_ptr->my_tail);
                array_def.head->declaration_bound_sym_node = symtable_ptr->my_tail;
                // ^ --- sym attribute --- ^ //
            }

            else
            {

                SingleDefinitionAST single_def(now_token, symtable_ptr);
                AST_node::connect_child(head, single_def.head);
                now_token = single_def.Parse();
                next_token = token_safe::next(now_token);

                // v --- sym append & attribute --- v //
                symtable_ptr->append(sym_node);
                single_def.head->absorb_sym_attribution(symtable_ptr->my_tail);
                single_def.head->declaration_bound_sym_node = symtable_ptr->my_tail;
                // ^ --- sym append & attribute --- ^ //
            }
        }

        else
        {
            AST_safe::raise_error("in DeclarationStatement, missing identify name", now_token);
            return now_token;
        }
    }
    return now_token;
}

TOKEN_PTR NormalStatementAST::Parse()
{
    head->type = NormalStatement;

    if (token_safe::type(now_token) == IDENT && token_safe::search_data(next_token, "=", ";"))
    {

        if (token_safe::data(next_token) == "[")
        {
            ArrayAssignmentAST assi(now_token, symtable_ptr);
            AST_node::connect_child(head, assi.head);
            now_token = assi.Parse();
            next_token = token_safe::next(now_token);

            if (token_safe::data(now_token) != ";")
            {
                AST_safe::raise_error("in NormalStatement, lost punctuation [;]", now_token);
                return now_token;
            }
            GoNext();
        }

        else
        {
            SingleAssignmentAST assi(now_token, symtable_ptr);
            AST_node::connect_child(head, assi.head);
            now_token = assi.Parse();
            next_token = token_safe::next(now_token);

            if (token_safe::data(now_token) != ";")
            {
                AST_safe::raise_error("in NormalStatement, lost punctuation [;]", now_token);
                return now_token;
            }
            GoNext();
        }
    }

    else if (token_safe::data(now_token) == "{")
    {

        // v --- sym block --- v //
        Symtable block_symtable;
        block_symtable.extend_from(symtable_ptr);
        block_symtable.my_head->rename("block");
        // ^ --- sym block --- ^ //

        BlockStatementAST block(now_token, block_symtable);
        AST_node::connect_child(head, block.head);
        now_token = block.Parse();
        next_token = token_safe::next(now_token);
    }

    else if (token_safe::type(now_token) == KEYWORD)
    {

        std::string now_data = token_safe::data(now_token);
        if (now_data == "if" || now_data == "while" || now_data == "break" ||
            now_data == "continue" || now_data == "return")
        {
            KeywordStatementAST key_stmt(now_token, symtable_ptr);
            AST_node::connect_child(head, key_stmt.head);
            now_token = key_stmt.Parse();
            next_token = token_safe::next(now_token);
        }

        else
        {
            AST_safe::raise_error("in NormalStatement, begin with wrong sign", now_token);
            GoNext(); // !important, to avoid circle in normal statement
            return now_token;
        }
    }

    else if (token_safe::type(now_token) == NUMBER || token_safe::type(now_token) == IDENT)
    {

        ExpressionAST expr(now_token, symtable_ptr);
        AST_node::connect_child(head, expr.head);
        expr.head->data = "value";
        expr.head->comment = "statement";
        now_token = expr.Parse();
        next_token = token_safe::next(now_token);

        if (token_safe::data(now_token) != ";")
        {
            AST_safe::raise_error("in NormalStatement, lost punctuation [;]", now_token);
            return now_token;
        }
        GoNext();
    }

    else if (token_safe::data(now_token) == ";")
    {
        GoNext();
    }

    else
    {
        AST_safe::raise_error("in NormalStatement, begin with wrong sign", now_token);
        GoNext(); // !important, to avoid circle in normal statement
        return now_token;
    }

    return now_token;
}

TOKEN_PTR StatementAST::Parse()
{
    head->type = Statement;

    if (token_safe::data(now_token) == "int" || token_safe::data(now_token) == "const")
    {
        DeclarationStatementAST decl_stmt(now_token, symtable_ptr);
        AST_node::connect_child(head, decl_stmt.head);
        now_token = decl_stmt.Parse();
        next_token = token_safe::next(now_token);
    }

    else if (token_safe::data(now_token) != "}" && now_token != nullptr)
    {
        NormalStatementAST norm_stmt(now_token, symtable_ptr);
        AST_node::connect_child(head, norm_stmt.head);
        now_token = norm_stmt.Parse();
        next_token = token_safe::next(now_token);
    }

    return now_token;
}

TOKEN_PTR BlockStatementAST::Parse()
{
    head->type = BlockStatement;

    if (token_safe::data(now_token) != "{")
    {
        AST_safe::raise_error("in BlockStatement, beginning is not [{]", now_token);
        return now_token;
    }
    GoNext();

    while (true)
    {

        if (token_safe::data(now_token) != "}" && now_token != nullptr)
        {
            StatementAST stmt(now_token, symtable_ptr);
            AST_node::connect_child(head, stmt.head);
            now_token = stmt.Parse();
            next_token = token_safe::next(now_token);
        }

        else if (token_safe::data(now_token) == "}")
        {
            GoNext();
            break;
        }

        else
        {
            AST_safe::raise_error("in BlockStatement, ending is not [}]", now_token);
            return now_token;
        }
    }

    return now_token;
}

TOKEN_PTR FunctionFormParamAST::Parse()
{
    head->type = FunctionFormParam;

    // v --- sym --- v //
    symtable_node sym_node;
    // ^ --- sym --- ^ //

    if (token_safe::data(now_token) != "int")
    {
        AST_safe::raise_error("in FunctionFormParam, type is not [int]", now_token);
        return now_token;
    }
    // v --- useless, since placeholder_sym_node take place of its work --- v //
    // AST_PTR type_name = std::make_shared<AST_node>();
    // AST_node::connect_child(head, type_name);
    // type_name->type = BasicType;
    // type_name->data = token_safe::data(now_token);

    // v --- sym --- v //
    sym_node.IVTT.reset_and_parse_from_string(token_safe::data(now_token));
    // ^ --- sym --- ^ //

    GoNext();

    if (token_safe::type(now_token) != IDENT)
    {
        AST_safe::raise_error("in FunctionFormParam, identify name is not valid", now_token);
        return now_token;
    }

    // v --- sym search --- v //
    SYM_PTR temp_sym_node = AST_safe::search_id_name(token_safe::data(now_token), symtable_ptr, true);
    if (temp_sym_node != nullptr)
    {
        AST_safe::raise_error("Redefinition", now_token);
        return now_token;
    }
    // ^ --- sym search --- ^ //

    // v --- sym change --- v //
    sym_node.identifier_name = token_safe::data(now_token);
    // ^ --- sym change --- ^ //

    AST_PTR id_name = std::make_shared<AST_node>();
    AST_node::connect_child(head, id_name);
    id_name->type = Identifier;
    id_name->data = token_safe::data(now_token);
    GoNext();

    // v --- sym append --- v //
    symtable_ptr->append(sym_node);
    // ^ --- sym append --- ^ //

    if (token_safe::data(now_token) == "[")
    {

        AST_PTR index_ast = std::make_shared<AST_node>();
        AST_node::connect_child(head, index_ast);
        index_ast->type = Index;

        // v --- sym change --- v //
        symtable_ptr->my_tail->IVTT.self_change_to_pointer();
        symtable_ptr->my_tail->IVTT.self_add_nest();
        // ^ --- sym change --- ^ //

        GoNext();

        if (token_safe::data(now_token) == "]")
        {
            AST_PTR empty = std::make_shared<AST_node>();
            AST_node::connect_child(index_ast, empty);
            empty->type = Expression;
            GoNext();
        }
        else
        {
            ExpressionAST index(now_token, symtable_ptr);
            AST_node::connect_child(index_ast, index.head);
            index.head->data = "value";
            index.head->comment = "index";
            now_token = index.Parse();
            next_token = token_safe::next(now_token);

            if (token_safe::data(now_token) != "]")
            {
                AST_safe::raise_error("in FunctionFormParam, lost punctuation []]", now_token);
                return now_token;
            }
            GoNext();
        }

        while (token_safe::data(now_token) == "[")
        {

            // v --- sym change --- v //
            symtable_ptr->my_tail->IVTT.self_add_nest();
            // ^ --- sym change --- ^ //

            GoNext();

            ExpressionAST index_addi(now_token, symtable_ptr);
            AST_node::connect_child(index_ast, index_addi.head);
            index_addi.head->data = "value";
            index_addi.head->comment = "index";
            now_token = index_addi.Parse();
            next_token = token_safe::next(now_token);

            if (token_safe::data(now_token) != "]")
            {
                AST_safe::raise_error("in FunctionFormParam, lost punctuation []]", now_token);
                return now_token;
            }
            GoNext();
        }
    }

    // v --- sym append & attribute --- v //
    symtable_ptr->my_head->IVTT.additional_storage_push(symtable_ptr->my_tail->IVTT);
    head->absorb_sym_attribution(symtable_ptr->my_tail);
    head->declaration_bound_sym_node = symtable_ptr->my_tail;
    // ^ --- sym append & attribute --- ^ //

    return now_token;
}

TOKEN_PTR FunctionParamsAST::Parse()
{
    head->type = FunctionParams;

    if (token_safe::data(now_token) == ")")
    {
        return now_token;
    }

    FunctionFormParamAST func_param(now_token, symtable_ptr);
    AST_node::connect_child(head, func_param.head);
    now_token = func_param.Parse();
    next_token = token_safe::next(now_token);

    while (true)
    {

        if (token_safe::data(now_token) == ")")
        {
            break;
        }

        if (token_safe::data(now_token) != ",")
        {
            AST_safe::raise_error("in FunctionParams, punctuation should be [,] or [)]", now_token);
            return now_token;
        }
        GoNext();

        FunctionFormParamAST addi_func_param(now_token, symtable_ptr);
        AST_node::connect_child(head, addi_func_param.head);
        now_token = addi_func_param.Parse();
        next_token = token_safe::next(now_token);
    }

    return now_token;
}

TOKEN_PTR FunctionDefinitionAST::Parse()
{
    head->type = FunctionDefinition;

    // v --- sym --- v //
    symtable_node sym_node;
    sym_node.IVTT.reset_and_parse_from_string("function*");
    // ^ --- sym --- ^ //

    // v --- sym block --- v //
    Symtable func_block_symtable;
    func_block_symtable.extend_from(symtable_ptr);
    // ^ --- sym block --- ^ //

    if (token_safe::data(now_token) != "int" && token_safe::data(now_token) != "void")
    {
        AST_safe::raise_error("in FunctionDefinition, type is not [int] or [void]", now_token);
        return now_token;
    }

    // v --- sym change --- v //
    sym_node.IVTT.return_storage_parse_from_string(token_safe::data(now_token));
    // ^ --- sym change --- ^ //

    GoNext();

    if (token_safe::type(now_token) != IDENT)
    {
        AST_safe::raise_error("in FunctionDefinition, function name is not a valid name", now_token);
        return now_token;
    }

    // v --- sym search --- v //
    SYM_PTR temp_sym_node = AST_safe::search_id_name(token_safe::data(now_token), symtable_ptr, true);
    if (temp_sym_node != nullptr)
    {
        AST_safe::raise_error("Redefinition", now_token);
        return now_token;
    }
    // ^ --- sym search --- ^ //

    AST_PTR func_name = std::make_shared<AST_node>();
    AST_node::connect_child(head, func_name);
    func_name->type = Identifier;
    func_name->data = token_safe::data(now_token);

    // v --- sym change --- v //
    sym_node.identifier_name = token_safe::data(now_token);
    func_block_symtable.my_head->rename("function_block");
    // ^ --- sym change --- ^ //

    auto func_block_symtable_ptr = std::make_shared<Symtable>(func_block_symtable);

    GoNext();

    if (token_safe::data(now_token) != "(")
    {
        AST_safe::raise_error("in FunctionDefinition, lost punctuation [(]", now_token);
        return now_token;
    }
    GoNext();

    FunctionParamsAST func_para(now_token, func_block_symtable_ptr);
    AST_node::connect_child(head, func_para.head);
    now_token = func_para.Parse();
    next_token = token_safe::next(now_token);

    // v --- sym change --- v //
    sym_node.IVTT.additional_storage_vector = func_para.GetBackSymtableAttribute()->IVTT.additional_storage_vector;
    // ^ --- sym change --- ^ //

    // v --- sym append & attribute --- v //
    symtable_ptr->append(sym_node);
    head->absorb_sym_attribution(symtable_ptr->my_tail);
    head->declaration_bound_sym_node = symtable_ptr->my_tail;
    // ^ --- sym append & attribute --- ^ //

    if (token_safe::data(now_token) != ")")
    {
        AST_safe::raise_error("in FunctionDefinition, lost punctuation [)]", now_token);
        return now_token;
    }
    GoNext();

    BlockStatementAST func_block(now_token, func_block_symtable_ptr);
    AST_node::connect_child(head, func_block.head);
    func_block.head->data = "FunctionBlock";
    now_token = func_block.Parse();
    next_token = token_safe::next(now_token);

    return now_token;
}

TOKEN_PTR ProgramAST::Parse()
{
    head->type = ProgramBody;

    symtable_ptr->my_head->rename("this_program");

    while (now_token != nullptr)
    {

        // in all ASTs, GlobalError only use here
        if (Safe::GlobalError)
        {
            break;
        }

        TOKEN_PTR look_next = token_safe::next(next_token);

        if (token_safe::data(now_token) == "void" ||
            (token_safe::data(now_token) == "int" && token_safe::data(look_next) == "("))
        {
            FunctionDefinitionAST func_def(now_token, symtable_ptr);
            AST_node::connect_child(head, func_def.head);
            now_token = func_def.Parse();
            next_token = token_safe::next(now_token);
        }

        else if (token_safe::data(now_token) == "int" || token_safe::data(now_token) == "const")
        {
            DeclarationStatementAST stmt(now_token, symtable_ptr);
            AST_node::connect_child(head, stmt.head);
            stmt.head->is_static = true;
            now_token = stmt.Parse();
            next_token = token_safe::next(now_token);
        }

        else
        {
            AST_safe::raise_error("in ProgramAST, beginning must be a keyword [int] [void] or [const]", now_token);
            break;
        }
    }

    return now_token;
}