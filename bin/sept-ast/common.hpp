// 2021.05.08 - Victor Dods

#pragma once

#include <lvd/fmt.hpp>
#include "sept/FormalTypeOf.hpp"
#include "sept/NPType.hpp"
#include <stdexcept>
#include <string>
#include <utility>

#define ABSTRACT_TYPE_CONSTRUCTOR \
    template <typename Argument_> \
    Argument_ operator() (Argument_ &&argument) const { \
        if (!inhabits(argument, *this)) \
            throw std::runtime_error(LVD_FMT("argument " << argument << " does not inhabit abstract type " << *this)); \
        return std::forward<Argument_>(argument); \
    }

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

//
// Sigils that represent different categories of things.  This is sort of vaguely defined at the moment.
//

struct BinOp_c { ABSTRACT_TYPE_CONSTRUCTOR };
struct UnOp_c { ABSTRACT_TYPE_CONSTRUCTOR };

struct SymbolId_c {
    std::string operator() (char const *c_str) const {
        // Cast to std::string and forward.
        return this->operator()(std::string(c_str));
    }
    ABSTRACT_TYPE_CONSTRUCTOR
};

// These are all singletons
inline bool constexpr operator== (BinOp_c const &, BinOp_c const &) { return true; }
inline bool constexpr operator== (UnOp_c const &, UnOp_c const &) { return true; }
inline bool constexpr operator== (SymbolId_c const &, SymbolId_c const &) { return true; }

inline std::ostream &operator<< (std::ostream &out, BinOp_c const &) { return out << "BinOp"; }
inline std::ostream &operator<< (std::ostream &out, UnOp_c const &) { return out << "UnOp"; }
inline std::ostream &operator<< (std::ostream &out, SymbolId_c const &) { return out << "SymbolId"; }

inline sept::NonParametricType_c abstract_type_of (BinOp_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (UnOp_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (SymbolId_c const &) { return sept::NonParametricType; }

inline auto constexpr BinOp = BinOp_c{};
inline auto constexpr UnOp = UnOp_c{};
inline auto constexpr SymbolId = SymbolId_c{};

//
// Definitions of inhabits
//

inline bool constexpr inhabits (ASTNPTerm t, BinOp_c const &) {
    return ASTNPTermRepr(ASTNPTerm::__BinOp_LOWEST__) <= ASTNPTermRepr(t)
        && ASTNPTermRepr(t) <= ASTNPTermRepr(ASTNPTerm::__BinOp_HIGHEST__);
}

inline bool constexpr inhabits (ASTNPTerm t, UnOp_c const &) {
    return ASTNPTermRepr(ASTNPTerm::__UnOp_LOWEST__) <= ASTNPTermRepr(t)
        && ASTNPTermRepr(t) <= ASTNPTermRepr(ASTNPTerm::__UnOp_HIGHEST__);
}

inline bool inhabits (ASTNPTerm t, sept::FormalTypeOf_Term_c const &f) {
    return inhabits(sept::Data(t), f);
}

// For now just allow any string.
inline bool constexpr inhabits (std::string const &, SymbolId_c const &) {
    return true;
}
inline bool constexpr inhabits (char const *, SymbolId_c const &) {
    return true;
}
