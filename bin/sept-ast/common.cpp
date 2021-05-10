// 2021.05.08 - Victor Dods

#include "common.hpp"

#include <array>

std::string const &as_string (ASTNPTerm t) {
    static std::array<std::string,AST_NP_TERM_COUNT> const TABLE{
        // BinOp
        "And",
        "Or",
        "Xor",
        "Add",
        "Sub",
        "Mul",
        "Div",
        "Pow",

        // UnOp
        "Not",
        "Neg",

        // Misc
        "MapsTo",
        "DeclaredAs",
        "DefinedAs",
        "AssignFrom",
        "RoundOpen",
        "RoundClose",
        "SquareOpen",
        "SquareClose",
        "CurlyOpen",
        "CurlyClose",
        "If",
        "Then",
        "Else",
    };
    return TABLE.at(size_t(t));
}
