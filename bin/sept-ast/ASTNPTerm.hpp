// 2021.05.08 - Victor Dods

#pragma once

// Includes from this program's source
#include "core.hpp"

#include <string>

//
// Misc
//

inline int compare (uint8_t lhs, uint8_t rhs) {
    return int(lhs) - int(rhs);
}

//
// Non-parametric terms for this AST POC
//

// TODO: Could potentially split these up to make them distinct C++ types,
// which would make certain code be constexpr.
using ASTNPTermRepr = uint8_t;
enum class ASTNPTerm : ASTNPTermRepr {
    // BinOp
    AND = 0,
    OR,
    XOR,
    ADD,
    SUB,
    MUL,
    DIV,
    POW,
    __BinOp_LOWEST__ = AND,
    __BinOp_HIGHEST__ = POW,

    // UnOp
    NOT,
    NEG,
    __UnOp_LOWEST__ = NOT,
    __UnOp_HIGHEST__ = NEG,

    //
    // Misc -- TODO: Categorize
    //

    MAPS_TO,        // ->
    DECLARED_AS,    // :
    DEFINED_AS,     // ::=
    ASSIGN_FROM,    // =
    ROUND_OPEN,     // (
    ROUND_CLOSE,    // )
    SQUARE_OPEN,    // [
    SQUARE_CLOSE,   // ]
    CURLY_OPEN,     // {
    CURLY_CLOSE,    // }
    IF,
    THEN,
    ELSE,
    __Misc_LOWEST__ = MAPS_TO,
    __Misc_HIGHEST__ = ELSE,

    __LOWEST__ = __BinOp_LOWEST__,
    __HIGHEST__ = __Misc_HIGHEST__,
};

size_t constexpr AST_NP_TERM_COUNT = size_t(ASTNPTerm::__HIGHEST__)+1 - size_t(ASTNPTerm::__LOWEST__);

std::string const &as_string (ASTNPTerm t);

inline std::ostream &operator<< (std::ostream &out, ASTNPTerm const &t) {
    return out << as_string(t);
}

inline int compare (ASTNPTerm const &lhs, ASTNPTerm const &rhs) {
    return compare(ASTNPTermRepr(lhs), ASTNPTermRepr(rhs));
}

// BinOp
inline ASTNPTerm constexpr And = ASTNPTerm::AND;
inline ASTNPTerm constexpr Or  = ASTNPTerm::OR;
inline ASTNPTerm constexpr Xor = ASTNPTerm::XOR;
inline ASTNPTerm constexpr Add = ASTNPTerm::ADD;
inline ASTNPTerm constexpr Sub = ASTNPTerm::SUB;
inline ASTNPTerm constexpr Mul = ASTNPTerm::MUL;
inline ASTNPTerm constexpr Div = ASTNPTerm::DIV;
inline ASTNPTerm constexpr Pow = ASTNPTerm::POW;
// UnOp
inline ASTNPTerm constexpr Not = ASTNPTerm::NOT;
inline ASTNPTerm constexpr Neg = ASTNPTerm::NEG;
// Misc
inline ASTNPTerm constexpr MapsTo = ASTNPTerm::MAPS_TO;
inline ASTNPTerm constexpr DeclaredAs = ASTNPTerm::DECLARED_AS;
inline ASTNPTerm constexpr DefinedAs = ASTNPTerm::DEFINED_AS;
inline ASTNPTerm constexpr AssignFrom = ASTNPTerm::ASSIGN_FROM;
inline ASTNPTerm constexpr RoundOpen = ASTNPTerm::ROUND_OPEN;
inline ASTNPTerm constexpr RoundClose = ASTNPTerm::ROUND_CLOSE;
inline ASTNPTerm constexpr SquareOpen = ASTNPTerm::SQUARE_OPEN;
inline ASTNPTerm constexpr SquareClose = ASTNPTerm::SQUARE_CLOSE;
inline ASTNPTerm constexpr CurlyOpen = ASTNPTerm::CURLY_OPEN;
inline ASTNPTerm constexpr CurlyClose = ASTNPTerm::CURLY_CLOSE;
inline ASTNPTerm constexpr If = ASTNPTerm::IF;
inline ASTNPTerm constexpr Then = ASTNPTerm::THEN;
inline ASTNPTerm constexpr Else = ASTNPTerm::ELSE;
