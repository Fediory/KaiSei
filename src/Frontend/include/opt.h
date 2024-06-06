//
// Created by Tastror on 2022/5/29.
//

#pragma once

#include "define.h"

// Optimise

namespace AST_optimize_safe {
    void raise_error(const std::string& error_code);
}

namespace Front::Optimiser {
    void Optimize(const AST_PTR& source_AST_head);
    void optimize_single(const AST_PTR& now, const AST_PTR& parent);
}

namespace Optimize_Useful {
    identify_value_type_tuple implicit_conversion(const identify_value_type_tuple& a, const identify_value_type_tuple& b);
    identify_value_type_tuple binary_operator_implicit_conversion(
            const std::string& binary_operator,
            const identify_value_type_tuple& a,
            const identify_value_type_tuple& b
    );
    identify_value_type_tuple calculate(
            const std::string& binary_operator,
            const identify_value_type_tuple& a,
            const identify_value_type_tuple& b
    );
    AST_PTR get_last_child(const AST_PTR& statement_ast);
}