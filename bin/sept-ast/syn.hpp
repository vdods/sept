// 2021.03.27 - Victor Dods

#pragma once

// Includes from this program's source
#include "core.hpp"
#include "EvalCtx.hpp"

#include <array>
#include "sept/ArrayTerm.hpp"
#include "sept/FormalTypeOf.hpp"
#include "sept/MemRef.hpp"
#include "sept/NPType.hpp"
#include "sept/Tuple.hpp"
#include "sept/Union.hpp"
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

inline std::string const &as_string (ASTNPTerm t) {
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

// This can't be const.
// TODO: Figure out how to keep this as sept::UnionTerm_c so that its usage doesn't require Data indirection.
extern sept::Data Expr_as_Data;
extern sept::RefTerm_c const Expr_as_Ref;
extern sept::UnionTerm_c const &Expr;

// This can't be const.
// TODO: Figure out how to keep this as sept::UnionTerm_c so that its usage doesn't require Data indirection.
extern sept::Data TypeExpr_as_Data;
extern sept::RefTerm_c const TypeExpr_as_Ref;
extern sept::UnionTerm_c const &TypeExpr;

extern sept::UnionTerm_c const ValueTerminal;
extern sept::UnionTerm_c const TypeTerminal;
// TODO: Could define variables for each as well

extern sept::TupleTerm_c const SymbolDefn;
// TODO: Eventually this must support assignment to lvalues, not just symbols.
extern sept::TupleTerm_c const Assignment;

extern sept::UnionTerm_c const Stmt;
extern sept::ArrayETerm_c const StmtArray;
extern sept::ArrayETerm_c const ExprArray;
// This defines a Rust-style block expression, where the last expression is the produced value of the block.
extern sept::TupleTerm_c const BlockExpr;

// These are effectively structural subtypes (indistinguishable from the specific tuple terms)
extern sept::TupleTerm_c const BinOpExpr;
// NOTE: This should be called LeftUnOpExpr, and there should also be RightUnOpExpr
// TODO: Shouldn't these be const?
extern sept::TupleTerm_c UnOpExpr;
extern sept::TupleTerm_c CondExpr;
extern sept::TupleTerm_c RoundExpr;
extern sept::TupleTerm_c SquareExpr;
extern sept::TupleTerm_c CurlyExpr;

// TODO: SymbolId has to be std::string basically
extern sept::TupleTerm_c const SymbolTypeDecl;
extern sept::TupleTerm_c const FuncType;
extern sept::ArrayETerm_c SymbolTypeDeclArray;
extern sept::TupleTerm_c const FuncPrototype;
extern sept::TupleTerm_c const FuncLiteral;
extern sept::TupleTerm_c const FuncEval;
extern sept::TupleTerm_c const ElementEval;
extern sept::TupleTerm_c const Construction;

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

//
// TODO: Deprecate these, since semantic terms are what do evaluate and execute
//

sept::ArrayTerm_c evaluate_expr__as_ExprArray (sept::ArrayTerm_c const &a, EvalCtx &ctx);
sept::Data evaluate_expr__as_BinOpExpr (sept::TupleTerm_c const &t, EvalCtx &ctx);
sept::Data evaluate_expr__as_BlockExpr (sept::TupleTerm_c const &t, EvalCtx &ctx);
sept::Data evaluate_expr__as_CondExpr (sept::TupleTerm_c const &t, EvalCtx &ctx);
sept::Data evaluate_expr__as_Construction (sept::TupleTerm_c const &t, EvalCtx &ctx);
sept::Data evaluate_expr__as_ElementEval (sept::TupleTerm_c const &t, EvalCtx &ctx);
sept::Data evaluate_expr__as_UnOpExpr (sept::TupleTerm_c const &t, EvalCtx &ctx);
sept::ArrayTerm_c evaluate_expr__as_RoundExpr (sept::TupleTerm_c const &t, EvalCtx &ctx);
sept::Data evaluate_expr__as_SymbolId (std::string const &symbol_id, EvalCtx &ctx);
sept::Data evaluate_expr__as_FuncEval (sept::TupleTerm_c const &t, EvalCtx &ctx);
sept::Data evaluate_expr (sept::TupleTerm_c const &t, EvalCtx &ctx);

void execute_stmt__as_SymbolDefn (sept::TupleTerm_c const &t, EvalCtx &ctx);
void execute_stmt__as_Assignment (sept::TupleTerm_c const &t, EvalCtx &ctx);
void execute_stmt (sept::TupleTerm_c const &t, EvalCtx &ctx);
