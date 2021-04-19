#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <lvd/abort.hpp>
#include <lvd/call_site.hpp>
#include <lvd/comma.hpp>
#include <lvd/fmt.hpp>
#include <lvd/g_log.hpp>
#include <lvd/ScopeGuard.hpp>
#include "sept/ArrayTerm.hpp"
#include "sept/ArrayType.hpp"
#include "sept/FormalTypeOf.hpp"
#include "sept/GlobalSymRef.hpp"
#include "sept/LocalSymRef.hpp"
#include "sept/MemRef.hpp"
#include "sept/NPType.hpp"
#include "sept/Tuple.hpp"
#include "sept/Union.hpp"

/*
AST design notes
-   Make a simple functional language with arithmetic operations, functions, conditionals, etc.
-   Goal is to have sept GUI be able to view and work with these AST classes natively and naturally.
-   Terms
    -   LogicalBinOp, which has inhabitants (this could be made into a Rust-style enum)
        -   And
        -   Or
        -   Xor
    -   LogicalUnOp, which has inhabitants (this could be made into a Rust-style enum)
        -   Not
    -   LogicalBinOpExpr := Tuple(LogicalExpr, LogicalBinOp, LogicalExpr)
    -   LogicalUnOpExpr := Tuple(LogicalUnOp, LogicalExpr)
    -   LogicalParenExpr := Tuple(LogicalExpr)
    -   LogicalCondExpr := Tuple(LogicalExpr, LogicalExpr, LogicalExpr) // (condition, positive_expr, negative_expr)
    -   LogicalExpr := Union(Bool, LogicalBinOpExpr, LogicalCondExpr, LogicalParenExpr, LogicalUnOpExpr)
    -   NumericBinOp, which has inhabitants (this could be made into a Rust-style enum)
        -   Add
        -   Sub
        -   Mul
        -   Div
        -   Pow
    -   NumericUnOp, which has inhabitants (this could be made into a Rust-style enum)
        -   Neg
    -   NumericBinOpExpr := Tuple(NumericExpr, NumericBinOp, NumericExpr)
    -   NumericUnOpExpr := Tuple(NumericUnOp, NumericExpr)
    -   NumericParenExpr := Tuple(NumericExpr)
    -   NumericCondExpr := Tuple(LogicalExpr, NumericExpr, NumericExpr)
    -   NumericExpr := Union(Float64, NumericBinOpExpr, NumericUnOpExpr, NumericParenExpr)
-   Can use Array instead of Tuple for now, since Array is a weakly-typed Tuple.
    Note that it is only because the various Tuple-based type definitions are mutually disjoint
    that a type without a semantic subtype tag (e.g. LogicalParenExpr(LogicalExpr)) can be used.
-   Now that the type system is much more free and flexible, can define Expr and CondExpr to not
    be Numeric- or Logical-, but rather form a type that overlaps both.
    -   ExprKind := TypeSet(LogicalExpr, NumericExpr)
    -   Expr := UnionOfTypeSet(ExprKind)
    -   CondExpr := Tuple(LogicalExpr, X, X) where X:ExprKind
    The latter definition, for CondExpr, is not possible (yet) declaratively, but it is possible
    by a definition of `inhabits`.
-   Other stuff that could be useful
    -   BinOp := Union(LogicalBinOp, NumericBinOp)
    -   LogicalThing := Union(LogicalBinOp, LogicalBinOpExpr, LogicalCondExpr, LogicalExpr, LogicalParenExpr, LogicalUnOp, LogicalUnOpExpr)
    -   NumericThing := Union(NumericBinOp, NumericBinOpExpr, NumericCondExpr, NumericExpr, NumericParenExpr, NumericUnOp, NumericUnOpExpr)
-   Idea for improvement: Just make a single enum for BinOpTerm_c and a single enum for UnOpTerm_c,
    containing all classes of BinOps and UnOps respectively, and then define semantic classes
    that select out subsets of those binops.  Since there is no reason to have distinct concrete
    types for these things.  In fact, because the ops are just non-parametric terms, there could
    potentially be a single enum for all ops, and semantic classes are used to select different
    semantic subtypes.
-   Notes on adding an `inhabits` binop to LogicalBinOp
    -   So far, it was assumed that LogicalBinOps accept LogicalExpr (i.e. something that evaluates to LogicalValueTerminal)
        and produce LogicalValueTerminal.
    -   But `x inhabits T` has differently typed arguments.  `x` can be any term (including a type), and `T` can be any type.
    -   So the name "LogicalBinOp" is rather underspecified.  It really needs to specify that both arguments are
        LogicalExpr and the output is LogicalValueTerminal.  Then there is "room" for another type name which `harbors`
        BinOps taking other types but producing LogicalValueTerminal, and then LogicalBinOpExpr would be any BinOpExpr
        that produces LogicalValueTerminal.
-   Adding functions and type constructions and type declarations
    -   SymbolTypeDecl is Tuple(SymbolId, DeclaredAs, TypeExpr) i.e. `x : T`
        Eventually include `where` clauses, i.e. `x : T where <LogicalExpr in x>`; this would implicitly declare a subtype of T.
    -   FuncType is Tuple(TypeExpr, MapsTo, TypeExpr) i.e. `T1 -> T2`
    -   FuncPrototype Tuple(Parenthesized(SymbolTypeDecl), MapsTo, TypeExpr) i.e. `(x:T1) -> T2`
        or maybe also include Tuple(Parenthesized(SymbolTypeDecl), MapsTo, Parenthesized(SymbolTypeDecl)) i.e. `(x:T1) -> (y:T2)`
    -   TypeExpr := Union(TypeTerminal, FuncType) -- add more later, such as tuples, arrays, etc
    -   FuncValue := Tuple(FuncPrototype, Expr) -- e.g. `(x:Float64) -> Float64 { x - x^3.0 / factorial(3.0) + x^5.0 / factorial(5.0) }
    -   Later, do dependent functions.  This is a lot more complicated though and isn't a good first step.
    -   In order to implement evaluate for SymbolId, it's required to implement SymbolicRef
-   Idea: Use round brackets, i.e. parens, exclusively for syntactical grouping, and not to indicate data types.
*/

//
// StaticAssociation_t for evaluate_expr_data
//

class EvalCtx;

using EvaluateExprFunction = std::function<sept::Data(sept::Data const &expr, EvalCtx &ctx)>;
using DataEvaluateExprFunctionMap = std::unordered_map<std::type_index,EvaluateExprFunction>;
LVD_STATIC_ASSOCIATION_DEFINE(EvaluateExpr, DataEvaluateExprFunctionMap)

#define SEPT__REGISTER__EVALUATE_EXPR__GIVE_ID__EVALUATOR(ExprType, unique_id, evaluator) \
    LVD_STATIC_ASSOCIATION_REGISTER( \
        EvaluateExpr, \
        unique_id, \
        std::type_index(typeid(ExprType)), \
        evaluator \
    )
#define SEPT__REGISTER__EVALUATE_EXPR__EVALUATOR(ExprType, evaluator) \
    SEPT__REGISTER__EVALUATE_EXPR__GIVE_ID__EVALUATOR(ExprType, ExprType, evaluator)
#define SEPT__REGISTER__EVALUATE_EXPR__GIVE_ID(ExprType, unique_id) \
    SEPT__REGISTER__EVALUATE_EXPR__GIVE_ID__EVALUATOR( \
        ExprType, \
        unique_id, \
        [](sept::Data const &expr_data, EvalCtx &ctx) -> sept::Data { \
            auto const &expr = expr_data.cast<ExprType const &>(); \
            return evaluate_expr(expr, ctx); \
        } \
    )
#define SEPT__REGISTER__EVALUATE_EXPR(ExprType) \
    SEPT__REGISTER__EVALUATE_EXPR__GIVE_ID(ExprType, ExprType)

sept::Data evaluate_expr_data (sept::Data const &expr_data, EvalCtx &ctx) {
    // Look up the type pair in the evaluator map.
    auto const &evaluator_map = lvd::static_association_singleton<EvaluateExpr>();
    auto it = evaluator_map.find(std::type_index(expr_data.type()));
    if (it == evaluator_map.end())
//         throw std::runtime_error(LVD_FMT("Data type " << expr_data.type().name() << " is not registered in EvaluateExpr for use in evaluate_expr_data"));
        // Evaluation is the identity.  TODO: Maybe don't do this, because it makes missing registrations harder to detect.
        return expr_data;

    return it->second(expr_data, ctx);
}

//
// StaticAssociation_t for execute_stmt_data
//

using ExecuteStmtFunction = std::function<void(sept::Data const &stmt, EvalCtx &ctx)>;
using DataExecuteStmtFunctionMap = std::unordered_map<std::type_index,ExecuteStmtFunction>;
LVD_STATIC_ASSOCIATION_DEFINE(ExecuteStmt, DataExecuteStmtFunctionMap)

#define SEPT__REGISTER__EXECUTE_STMT__GIVE_ID__EVALUATOR(StmtType, unique_id, evaluator) \
    LVD_STATIC_ASSOCIATION_REGISTER( \
        ExecuteStmt, \
        unique_id, \
        std::type_index(typeid(StmtType)), \
        evaluator \
    )
#define SEPT__REGISTER__EXECUTE_STMT__EVALUATOR(StmtType, evaluator) \
    SEPT__REGISTER__EXECUTE_STMT__GIVE_ID__EVALUATOR(StmtType, StmtType, evaluator)
#define SEPT__REGISTER__EXECUTE_STMT__GIVE_ID(StmtType, unique_id) \
    SEPT__REGISTER__EXECUTE_STMT__GIVE_ID__EVALUATOR( \
        StmtType, \
        unique_id, \
        [](sept::Data const &stmt_data, EvalCtx &ctx) -> void { \
            auto const &stmt = stmt_data.cast<StmtType const &>(); \
            execute_stmt(stmt, ctx); \
        } \
    )
#define SEPT__REGISTER__EXECUTE_STMT(StmtType) \
    SEPT__REGISTER__EXECUTE_STMT__GIVE_ID(StmtType, StmtType)

void execute_stmt_data (sept::Data const &stmt_data, EvalCtx &ctx) {
    // Look up the type pair in the evaluator map.
    auto const &evaluator_map = lvd::static_association_singleton<ExecuteStmt>();
    auto it = evaluator_map.find(std::type_index(stmt_data.type()));
    if (it == evaluator_map.end())
        throw std::runtime_error(LVD_FMT("Data type " << stmt_data.type().name() << " is not registered in ExecuteStmt for use in execute_stmt_data"));

    it->second(stmt_data, ctx);
}

//
// Misc
//

int compare (uint8_t lhs, uint8_t rhs) {
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

std::ostream &operator<< (std::ostream &out, ASTNPTerm const &t) {
    return out << as_string(t);
}

int compare (ASTNPTerm const &lhs, ASTNPTerm const &rhs) {
    return compare(ASTNPTermRepr(lhs), ASTNPTermRepr(rhs));
}


// BinOp
ASTNPTerm const And{ASTNPTerm::AND};
ASTNPTerm const Or{ASTNPTerm::OR};
ASTNPTerm const Xor{ASTNPTerm::XOR};
ASTNPTerm const Add{ASTNPTerm::ADD};
ASTNPTerm const Sub{ASTNPTerm::SUB};
ASTNPTerm const Mul{ASTNPTerm::MUL};
ASTNPTerm const Div{ASTNPTerm::DIV};
ASTNPTerm const Pow{ASTNPTerm::POW};
// UnOp
ASTNPTerm const Not{ASTNPTerm::NOT};
ASTNPTerm const Neg{ASTNPTerm::NEG};
// Misc
ASTNPTerm const MapsTo{ASTNPTerm::MAPS_TO};
ASTNPTerm const DeclaredAs{ASTNPTerm::DECLARED_AS};
ASTNPTerm const DefinedAs{ASTNPTerm::DEFINED_AS};
ASTNPTerm const AssignFrom{ASTNPTerm::ASSIGN_FROM};
ASTNPTerm const RoundOpen{ASTNPTerm::ROUND_OPEN};
ASTNPTerm const RoundClose{ASTNPTerm::ROUND_CLOSE};
ASTNPTerm const SquareOpen{ASTNPTerm::SQUARE_OPEN};
ASTNPTerm const SquareClose{ASTNPTerm::SQUARE_CLOSE};
ASTNPTerm const CurlyOpen{ASTNPTerm::CURLY_OPEN};
ASTNPTerm const CurlyClose{ASTNPTerm::CURLY_CLOSE};
ASTNPTerm const If{ASTNPTerm::IF};
ASTNPTerm const Then{ASTNPTerm::THEN};
ASTNPTerm const Else{ASTNPTerm::ELSE};

//
// Sigils that represent different categories of things.  This is sort of vaguely defined at the moment.
//

#define ABSTRACT_TYPE_CONSTRUCTOR \
    template <typename Argument_> \
    Argument_ operator() (Argument_ &&argument) const { \
        if (!inhabits(argument, *this)) \
            throw std::runtime_error(LVD_FMT("argument " << argument << " does not inhabit abstract type " << *this)); \
        return std::forward<Argument_>(argument); \
    }

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
bool constexpr operator== (BinOp_c const &, BinOp_c const &) { return true; }
bool constexpr operator== (UnOp_c const &, UnOp_c const &) { return true; }
bool constexpr operator== (SymbolId_c const &, SymbolId_c const &) { return true; }

std::ostream &operator<< (std::ostream &out, BinOp_c const &) { return out << "BinOp"; }
std::ostream &operator<< (std::ostream &out, UnOp_c const &) { return out << "UnOp"; }
std::ostream &operator<< (std::ostream &out, SymbolId_c const &) { return out << "SymbolId"; }

inline sept::NonParametricType_c abstract_type_of (BinOp_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (UnOp_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (SymbolId_c const &) { return sept::NonParametricType; }

BinOp_c const BinOp;
UnOp_c const UnOp;
SymbolId_c const SymbolId;

// TODO: Figure out how to keep this as sept::UnionTerm_c so that its usage doesn't require Data indirection.
extern sept::Data Expr_as_Data;
auto const Expr_as_Ref = sept::MemRef(&Expr_as_Data);

// TODO: Figure out how to keep this as sept::UnionTerm_c so that its usage doesn't require Data indirection.
extern sept::Data TypeExpr_as_Data;
auto const TypeExpr_as_Ref = sept::MemRef(&TypeExpr_as_Data);

auto const ValueTerminal = sept::Union(
    sept::Array,
    sept::Bool,
    sept::FalseType,
    sept::Float32,
    sept::Float64,
    sept::Sint8,
    sept::Sint16,
    sept::Sint32,
    sept::Sint64,
    sept::Uint8,
    sept::Uint16,
    sept::Uint32,
    sept::Uint64,
    sept::TrueType,
    sept::Tuple
);
auto const TypeTerminal = sept::Union(
    sept::ArrayES,
    sept::ArrayE,
    sept::ArrayS,
    sept::ArrayType,
    sept::BoolType,
    sept::FormalTypeOf(sept::FalseType),
    sept::Sint8Type,
    sept::Sint16Type,
    sept::Sint32Type,
    sept::Sint64Type,
    sept::Uint8Type,
    sept::Uint16Type,
    sept::Uint32Type,
    sept::Uint64Type,
    sept::Float32Type,
    sept::Float64Type,
    sept::FormalTypeOf(sept::TrueType),
    sept::TupleType
);
// TODO: Could define variables for each as well

auto const SymbolDefn = sept::Tuple(SymbolId, sept::FormalTypeOf(DefinedAs), Expr_as_Ref);
// TODO: Eventually this must support assignment to lvalues, not just symbols.
auto const Assignment = sept::Tuple(SymbolId, sept::FormalTypeOf(AssignFrom), Expr_as_Ref);

auto const Stmt = sept::Union(SymbolDefn, Assignment);
auto const StmtArray = sept::ArrayE(Stmt);
auto const ExprArray = sept::ArrayE(Expr_as_Ref);
// This defines a Rust-style block expression, where the last expression is the produced value of the block.
auto const BlockExpr = sept::Tuple(StmtArray, Expr_as_Ref);

// These are effectively structural subtypes (indistinguishable from the specific tuple terms)
auto const BinOpExpr = sept::Tuple(Expr_as_Ref, BinOp, Expr_as_Ref);
// NOTE: This should be called LeftUnOpExpr
auto const UnOpExpr = sept::Tuple(UnOp, Expr_as_Ref);
auto const CondExpr = sept::Tuple(sept::FormalTypeOf(If), Expr_as_Ref, sept::FormalTypeOf(Then), Expr_as_Ref, sept::FormalTypeOf(Else), Expr_as_Ref);
auto const RoundExpr = sept::Tuple(sept::FormalTypeOf(RoundOpen), ExprArray, sept::FormalTypeOf(RoundClose));
auto const SquareExpr = sept::Tuple(sept::FormalTypeOf(SquareOpen), Expr_as_Ref, sept::FormalTypeOf(SquareClose)); // TODO: change to sept::Tuple(Expr_as_Ref)
auto const CurlyExpr = sept::Tuple(sept::FormalTypeOf(CurlyOpen), Expr_as_Ref, sept::FormalTypeOf(CurlyClose)); // TODO: change to sept::Tuple(Expr_as_Ref)

// TODO: SymbolId has to be std::string basically
auto const SymbolTypeDecl = sept::Tuple(SymbolId, sept::FormalTypeOf(DeclaredAs), TypeExpr_as_Ref);
auto const FuncType = sept::Tuple(TypeExpr_as_Ref, sept::FormalTypeOf(MapsTo), TypeExpr_as_Ref);
auto const FuncPrototypeParams = sept::ArrayE(SymbolTypeDecl);
auto const FuncPrototype = sept::Tuple(FuncPrototypeParams, sept::FormalTypeOf(MapsTo), TypeExpr_as_Ref);
auto const FuncLiteral = sept::Tuple(FuncPrototype, Expr_as_Ref);
auto const FuncEval = sept::Tuple(SymbolId, RoundExpr);
auto const ElementEval = sept::Tuple(Expr_as_Ref, SquareExpr);
auto const Construction = sept::Tuple(TypeExpr_as_Ref, CurlyExpr);

// This can't be const.
sept::Data Expr_as_Data{
    sept::Union(
        BinOpExpr,
        BlockExpr,
        CondExpr,
        Construction,
        FuncEval,
        RoundExpr,
        UnOpExpr,
        SymbolId,
        ValueTerminal
    )
};
auto const &Expr = Expr_as_Data.cast<sept::UnionTerm_c const &>();

// TODO: More type constructions
sept::Data TypeExpr_as_Data{
    sept::Union(
        sept::ArrayES,
        sept::ArrayE,
        sept::ArrayS,
        sept::ArrayType,
        TypeTerminal,
        FuncType,
        FuncPrototype,
        SymbolId,
        sept::Tuple
    )
};
auto const &TypeExpr = TypeExpr_as_Data.cast<sept::UnionTerm_c const &>();

sept::ArrayTerm_c func_param_type_array_of (sept::ArrayTerm_c const &param_array) {
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << '\n'
//                << LVD_REFLECT(param_array) << '\n'
//                << LVD_REFLECT(ExprArray) << '\n';
    assert(inhabits(param_array, ExprArray));
    sept::DataVector param_types;
    param_types.reserve(param_array.size());
    for (auto const &param : param_array.elements()) {
        assert(inhabits_data(param, SymbolTypeDecl));
        param_types.emplace_back(param[2]);
    }
    return sept::ArrayTerm_c{std::move(param_types)};
}

//
// Definitions of inhabits
//

// inline sept::True_c constexpr inhabits (ASTNPTerm t, Any_c const &) { return sept::True; }

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
// evaluate
//

class EvalCtx {
public:

    // Start off using global_symbol_table.
    EvalCtx ()
        :   m_current_scope(sept::global_symbol_table())
    { }

    lvd::nnsp<sept::SymbolTable> const &current_scope () const { return m_current_scope; }

    // This will return a ScopeGuard object which facilitates exception-safe popping of scope.
    // That ScopeGuard object should be kept alive as long as the scope is meant to live.
    [[nodiscard]] lvd::ScopeGuard push_scope () {
        m_current_scope = m_current_scope->push_symbol_table();
        return lvd::ScopeGuard{[this](){ this->pop_scope(); }};
    }
    void pop_scope () {
        // This will throw if there is no parent.
        m_current_scope = m_current_scope->parent_symbol_table();
    }

private:

    lvd::nnsp<sept::SymbolTable> m_current_scope;
};

// Forward declarations
sept::Data evaluate_expr__as_SymbolId (std::string const &symbol_id, EvalCtx &ctx);
void execute_stmt__as_StmtArray (sept::Data const &stmt, EvalCtx &ctx);

bool evaluate_expr (bool const &expr, EvalCtx &ctx) { return expr; }
sept::BoolTerm_c evaluate_expr (sept::BoolTerm_c const &expr, EvalCtx &ctx) { return expr; }
double evaluate_expr (double const &expr, EvalCtx &ctx) { return expr; }
sept::Array_c evaluate_expr (sept::Array_c const &expr, EvalCtx &ctx) { return expr; }
sept::ArrayESTerm_c evaluate_expr (sept::ArrayESTerm_c const &expr, EvalCtx &ctx) { return expr; }
sept::ArrayETerm_c evaluate_expr (sept::ArrayETerm_c const &expr, EvalCtx &ctx) { return expr; }
sept::ArraySTerm_c evaluate_expr (sept::ArraySTerm_c const &expr, EvalCtx &ctx) { return expr; }
sept::ArrayTerm_c evaluate_expr (sept::ArrayTerm_c const &expr, EvalCtx &ctx) {
    sept::DataVector evaluated_elements;
    evaluated_elements.reserve(expr.size());
    for (auto const &element : expr.elements())
        evaluated_elements.emplace_back(evaluate_expr_data(element, ctx));
    return sept::ArrayTerm_c(std::move(evaluated_elements));
}
sept::Tuple_c evaluate_expr (sept::Tuple_c const &expr, EvalCtx &ctx) { return expr; }
sept::Data evaluate_expr (std::string const &expr, EvalCtx &ctx) {
    return evaluate_expr__as_SymbolId(expr, ctx);
}

// template <typename T_>
sept::Data evaluate_expr__as_BinOpExpr (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    assert(inhabits(t, BinOpExpr));

    auto lhs_data = evaluate_expr_data(t[0], ctx);
    auto bin_op = t[1].cast<ASTNPTerm>();
    auto rhs_data = evaluate_expr_data(t[2], ctx);
    switch (bin_op) {
        case ASTNPTerm::AND: return lhs_data.cast<bool>() && rhs_data.cast<bool>();
        case ASTNPTerm::OR:  return lhs_data.cast<bool>() || rhs_data.cast<bool>();
        case ASTNPTerm::XOR: return lhs_data.cast<bool>() != rhs_data.cast<bool>();
        case ASTNPTerm::ADD: return lhs_data.cast<double>() + rhs_data.cast<double>();
        case ASTNPTerm::SUB: return lhs_data.cast<double>() - rhs_data.cast<double>();
        case ASTNPTerm::MUL: return lhs_data.cast<double>() * rhs_data.cast<double>();
        case ASTNPTerm::DIV: return lhs_data.cast<double>() / rhs_data.cast<double>();
        case ASTNPTerm::POW: return std::pow(lhs_data.cast<double>(), rhs_data.cast<double>());
        default: LVD_ABORT(LVD_FMT("invalid ASTNPTerm for use as a BinOp: " << uint32_t(bin_op)));
    }
}

sept::Data evaluate_expr__as_UnOpExpr (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    if (!inhabits(t, UnOpExpr)) {
        throw std::runtime_error(LVD_FMT("failed condition: inhabits(t, UnOpExpr), where t = " << t << " and UnOpExpr = " << UnOpExpr));
    }
    assert(inhabits(t, UnOpExpr));

    auto un_op = t[0].cast<ASTNPTerm>();
    auto operand_data = evaluate_expr_data(t[1], ctx);
    switch (un_op) {
        case ASTNPTerm::NOT: return !operand_data.cast<bool>();
        case ASTNPTerm::NEG: return -operand_data.cast<double>();
        default: LVD_ABORT(LVD_FMT("invalid ASTNPTerm for use as an UnOp: " << uint32_t(un_op)));
    }
}

sept::Data evaluate_expr__as_CondExpr (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    assert(inhabits(t, CondExpr));
    auto condition = evaluate_expr_data(t[1], ctx).cast<bool>();
    return evaluate_expr_data(t[condition ? 3 : 5], ctx);
}

sept::ArrayTerm_c evaluate_expr__as_RoundExpr (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    assert(inhabits(t, RoundExpr));
    auto const &round_expr_array = t[1].cast<sept::ArrayTerm_c const &>();
    sept::DataVector evaluated_elements;
    evaluated_elements.reserve(round_expr_array.size());
    for (auto const &element : round_expr_array.elements())
        evaluated_elements.emplace_back(evaluate_expr_data(element, ctx));
    return sept::ArrayTerm_c{std::move(evaluated_elements)};
}

sept::Data evaluate_expr__as_BlockExpr (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    assert(inhabits(t, BlockExpr));
    // Push a new scope for this block.
    auto scope_guard = ctx.push_scope();
    // Execute all the statements in the block before the final expression.
    execute_stmt__as_StmtArray(t[0], ctx);
    // Evaluate the final expression, which is what renders the value of the BlockExpr
    auto final_value = evaluate_expr_data(t[1], ctx);
    return final_value;
}

sept::Data evaluate_expr__as_SymbolId (std::string const &symbol_id, EvalCtx &ctx) {
    assert(inhabits(symbol_id, SymbolId));
    return sept::LocalSymRef(symbol_id, ctx.current_scope());
}

sept::Data evaluate_expr__as_Construction (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    assert(inhabits(t, Construction));
    // Resolve the type that will be constructed.
//     lvd::g_log << lvd::Log::trc() << '\n' << LVD_CALL_SITE() << " - " << LVD_REFLECT(t) << '\n';
//     lvd::g_log << lvd::Log::trc() << '\n' << LVD_CALL_SITE() << " - " << LVD_REFLECT(t[0]) << '\n';
    auto type_to_construct = evaluate_expr_data(t[0], ctx);
//     lvd::g_log << lvd::Log::trc() << '\n' << LVD_CALL_SITE() << " - " << LVD_REFLECT(type_to_construct) << '\n';
    auto curly_param = t[1];
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(curly_param) << '\n';
    assert(inhabits_data(curly_param, CurlyExpr));
    auto param = curly_param[1];
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(param) << '\n';
    auto evaled_param = evaluate_expr_data(param, ctx);
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(evaled_param) << '\n';
    return type_to_construct(evaled_param).deref();
}

sept::Data evaluate_expr__as_ElementEval (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    assert(inhabits(t, ElementEval));
    // Resolve the container whose element will be eval'ed
    auto container = evaluate_expr_data(t[0], ctx);
//     lvd::g_log << lvd::Log::trc() << '\n' << LVD_CALL_SITE() << " - " << LVD_REFLECT(container) << '\n';
    auto square_param = t[1];
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(square_param) << '\n';
    assert(inhabits_data(square_param, SquareExpr));
    auto param = square_param[1];
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(param) << '\n';
    auto evaled_param = evaluate_expr_data(param, ctx);
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(evaled_param) << '\n';

    return container[evaled_param].deref();
}

sept::Data evaluate_expr__as_FuncEval (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    assert(inhabits(t, FuncEval));
    assert(!inhabits(t, Construction));
    assert(!inhabits(t, ElementEval));
    // Resolve the function symbol
    auto func = ctx.current_scope()->resolve_symbol_const(t[0].cast<std::string>());
//     lvd::g_log << lvd::Log::trc() << '\n' << LVD_CALL_SITE() << " - " << LVD_REFLECT(func) << '\n';
    assert(sept::inhabits_data(func, FuncLiteral));
    auto round_param = t[1];
    auto evaled_param_array = evaluate_expr__as_RoundExpr(round_param.cast<sept::TupleTerm_c const &>(), ctx);
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(evaled_param_array) << '\n';

    // Identify the parameter symbol decl
    // NOTE: This probably copy-constructs, instead of returning a const ref.
    // TODO: Could have the stuff return a MemRef.
    auto func_prototype = func[0].cast<sept::TupleTerm_c const &>();
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(func_prototype) << '\n';
    auto const &func_prototype_param_array = func_prototype.elements()[0].cast<sept::ArrayTerm_c const &>();
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(func_prototype_param_array) << '\n';
    auto func_param_type_array = func_param_type_array_of(func_prototype_param_array);
    // Check that there are the expected number of parameters in the FuncEval
    // TODO: Could check this earlier.
    if (func_param_type_array.size() != evaled_param_array.size())
        throw std::runtime_error(LVD_FMT("Expected " << func_param_type_array.size() << " parameters in call to function " << t[0] << ", but got " << evaled_param_array.size()));
    // Now check that all the param types are as expected.
    for (size_t i = 0; i < func_param_type_array.size(); ++i) {
        auto const &evaled_param = evaled_param_array.elements()[i];
        auto const &func_param_type = func_param_type_array.elements()[i];
        // NOTE/TEMP HACK: This will resolve any symbolic references in the param types using the local symbol table,
        // and not the symbol table of the scope that the function was declared in.  TODO: Fix this.
        auto evaled_func_param_type = evaluate_expr_data(func_param_type, ctx);
        if (!inhabits_data(evaled_param, evaled_func_param_type))
            throw std::runtime_error(LVD_FMT("In parameter " << i << " in call to function " << t[0] << ": Expected a value of type " << evaled_func_param_type << " but got " << evaled_param << " (which has abstract type " << sept::abstract_type_of_data(evaled_param) << ')'));
    }

    auto return_type = func_prototype[2];
    auto return_type_eval = evaluate_expr_data(return_type, ctx);

    // Push a context, define the function param(s).
    auto scope_guard = ctx.push_scope();
    for (size_t i = 0; i < func_param_type_array.size(); ++i) {
        auto &evaled_param = evaled_param_array.elements()[i];
        auto const &func_param_symbol_type_decl = func_prototype_param_array.elements()[i].cast<sept::TupleTerm_c const &>();
        auto const &func_param_symbol_id = func_param_symbol_type_decl.elements()[0].cast<std::string const &>();
        ctx.current_scope()->define_symbol(func_param_symbol_id, std::move(evaled_param));
    }
    // Evaluate the function body using the new scope.
    auto func_body = func[1];
    // Have to deref the data, since this might be a reference.
    auto retval = evaluate_expr_data(func_body, ctx).deref();
    // Check that the parameter is the correct type
    if (!sept::inhabits_data(retval, return_type_eval))
        throw std::runtime_error(LVD_FMT("Expected return value " << retval << " to evaluate to a term of type " << return_type << " but it didn't; retval: " << retval.deref()));
    return retval;
}

sept::Data evaluate_expr (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    // TODO: Make this efficient -- some sort of poset search.
    if (false)
        { }
    else if (inhabits(t, BinOpExpr))
        return evaluate_expr__as_BinOpExpr(t, ctx);
    else if (inhabits(t, BlockExpr))
        return evaluate_expr__as_BlockExpr(t, ctx);
    else if (inhabits(t, CondExpr))
        return evaluate_expr__as_CondExpr(t, ctx);
    else if (inhabits(t, Construction))
        return evaluate_expr__as_Construction(t, ctx);
    else if (inhabits(t, ElementEval))
        return evaluate_expr__as_ElementEval(t, ctx);
    else if (inhabits(t, FuncEval))
        return evaluate_expr__as_FuncEval(t, ctx);
    else if (inhabits(t, RoundExpr))
        return evaluate_expr__as_RoundExpr(t, ctx);
    else if (inhabits(t, UnOpExpr))
        return evaluate_expr__as_UnOpExpr(t, ctx);
    else if (inhabits(t, Expr)) {
        LVD_ABORT(LVD_FMT("unhandled Expr: " << t));
//         return t; // Just a plain Tuple.
    } else
        LVD_ABORT(LVD_FMT("attempting to evaluate_expr for a non-Expr: " << t));
}

void execute_stmt__as_StmtArray (sept::Data const &stmt, EvalCtx &ctx) {
    assert(inhabits_data(stmt, StmtArray));
    for (auto const &stmt : stmt.cast<sept::ArrayTerm_c const &>().elements())
        execute_stmt_data(stmt, ctx);
}

void execute_stmt__as_SymbolDefn (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    assert(inhabits(t, SymbolDefn));
    auto symbol_id = t[0];
    auto value = t[2];
    // Simply define the symbol in the current scope.
    ctx.current_scope()->define_symbol(symbol_id.cast<std::string>(), evaluate_expr_data(value, ctx));
    // If a SymbolDefn were a valid Expr, then return Void, or potentially return value, or symbol_id, or some reference.
}

void execute_stmt__as_Assignment (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    assert(inhabits(t, Assignment));
    auto symbol_id = t[0];
    auto value = t[2];
    // Simply define the symbol in the current scope.
    ctx.current_scope()->resolve_symbol_nonconst(symbol_id.cast<std::string>()) = evaluate_expr_data(value, ctx);
    // If an Assignment were a valid Expr, then return value, or symbol_id, or some reference.
}

void execute_stmt (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    // TODO: Make this efficient -- some sort of poset search.
    if (false)
        { }
    else if (inhabits(t, Assignment))
        return execute_stmt__as_Assignment(t, ctx);
    else if (inhabits(t, SymbolDefn))
        return execute_stmt__as_SymbolDefn(t, ctx);
    else if (inhabits(t, Stmt))
        LVD_ABORT(LVD_FMT("unhandled Stmt: " << t));
    else
        LVD_ABORT(LVD_FMT("attempting to execute_stmt for a non-Stmt: " << t));
}

//
// sept data model registrations
//

namespace sept {
SEPT__REGISTER__PRINT(ASTNPTerm)
SEPT__REGISTER__PRINT(BinOp_c)
SEPT__REGISTER__PRINT(UnOp_c)
SEPT__REGISTER__PRINT(SymbolId_c)
SEPT__REGISTER__PRINT__GIVE_ID(char const *, __char_const_ptr__)

SEPT__REGISTER__EQ(BinOp_c)
SEPT__REGISTER__EQ(UnOp_c)
SEPT__REGISTER__EQ(ASTNPTerm)
SEPT__REGISTER__EQ(SymbolId_c)

SEPT__REGISTER__ABSTRACT_TYPE_OF(BinOp_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(UnOp_c)

SEPT__REGISTER__INHABITS__NONDATA(ASTNPTerm,     BinOp_c)
SEPT__REGISTER__INHABITS__NONDATA(ASTNPTerm,     UnOp_c)
SEPT__REGISTER__INHABITS__NONDATA(ASTNPTerm,     FormalTypeOf_Term_c)
SEPT__REGISTER__INHABITS__GIVE_ID__NONDATA(std::string,   SymbolId_c, __std_string___SymbolId_c__)
SEPT__REGISTER__INHABITS__GIVE_ID__NONDATA(char const *,  SymbolId_c, __char_const_ptr___SymbolId_c__)
// TODO there are probably some missing

SEPT__REGISTER__COMPARE__SINGLETON(BinOp_c)
SEPT__REGISTER__COMPARE__SINGLETON(UnOp_c)
SEPT__REGISTER__COMPARE(ASTNPTerm, ASTNPTerm)

SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(BinOp_c,      ASTNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(UnOp_c,       ASTNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(FormalTypeOf_Term_c, ASTNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF___GIVE_ID__ABSTRACT_TYPE(SymbolId_c, std::string, __SymbolId_c___std_string__)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF___GIVE_ID__ABSTRACT_TYPE(SymbolId_c, char const *, __SymbolId_c___char_const_ptr__)

SEPT__REGISTER__EVALUATE_EXPR(bool)
SEPT__REGISTER__EVALUATE_EXPR(True_c)
SEPT__REGISTER__EVALUATE_EXPR(False_c)
SEPT__REGISTER__EVALUATE_EXPR(double)
SEPT__REGISTER__EVALUATE_EXPR__GIVE_ID(std::string, __std_string__)
SEPT__REGISTER__EVALUATE_EXPR(TupleTerm_c)
SEPT__REGISTER__EVALUATE_EXPR(Array_c)
SEPT__REGISTER__EVALUATE_EXPR(ArrayESTerm_c)
SEPT__REGISTER__EVALUATE_EXPR(ArrayETerm_c)
SEPT__REGISTER__EVALUATE_EXPR(ArraySTerm_c)
SEPT__REGISTER__EVALUATE_EXPR(ArrayTerm_c)

SEPT__REGISTER__EXECUTE_STMT(TupleTerm_c)

// TEMP HACK
SEPT__REGISTER__INHABITS__NONDATA(ASTNPTerm, UnionTerm_c)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(UnionTerm_c, ASTNPTerm)
} // end namespace sept

int main (int argc, char **argv) {
//     lvd::g_log.set_log_level_threshold(lvd::LogLevel::DBG);
    lvd::g_log.out().precision(std::numeric_limits<double>::max_digits10+1);
    lvd::g_log.out().setf(std::ios_base::boolalpha, std::ios_base::boolalpha);

    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(BinOp) << '\n'
               << LVD_REFLECT(BinOpExpr) << '\n'
               << LVD_REFLECT(CondExpr) << '\n'
               << LVD_REFLECT(Expr) << '\n'
               << LVD_REFLECT(UnOp) << '\n'
               << LVD_REFLECT(UnOpExpr) << '\n'
               << '\n';
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(inhabits(Add, BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Sub, BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Mul, BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Div, BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Pow, BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Neg, BinOp)) << '\n'
               << LVD_REFLECT(inhabits(And, BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Or, BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Xor, BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Not, BinOp)) << '\n'
               << '\n'
               << LVD_REFLECT(inhabits(Add, UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Sub, UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Mul, UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Div, UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Pow, UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Neg, UnOp)) << '\n'
               << LVD_REFLECT(inhabits(And, UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Or, UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Xor, UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Not, UnOp)) << '\n'
               << '\n'
               << LVD_REFLECT(inhabits(MapsTo, sept::FormalTypeOf(MapsTo))) << '\n'
               << LVD_REFLECT(inhabits(MapsTo, UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Xor, sept::FormalTypeOf(MapsTo))) << '\n'
               << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(123.4, Pow, 2.0), Expr)) << '\n'
               << '\n'
               << LVD_REFLECT(inhabits(sept::Float64, TypeExpr)) << '\n'
               << LVD_REFLECT(inhabits("hippo1", SymbolId)) << '\n'
               << LVD_REFLECT(inhabits(std::string("hippo2"), SymbolId)) << '\n'
               << LVD_REFLECT(inhabits(SymbolId("hippo3"), SymbolId)) << '\n'
               << LVD_REFLECT(inhabits(SymbolId(std::string("hippo4")), SymbolId)) << '\n'
               << LVD_REFLECT(inhabits(SymbolTypeDecl(sept::TupleTerm_c(std::string("hippo5"), DeclaredAs, sept::Float64)), SymbolTypeDecl)) << '\n'
               << LVD_REFLECT(inhabits(SymbolTypeDecl(sept::TupleTerm_c(SymbolId(std::string("hippo6")), DeclaredAs, sept::Float64)), SymbolTypeDecl)) << '\n'
               << LVD_REFLECT(inhabits(SymbolTypeDecl(sept::TupleTerm_c("hippo7", DeclaredAs, sept::Float64)), SymbolTypeDecl)) << '\n'
               << LVD_REFLECT(inhabits(SymbolTypeDecl(sept::TupleTerm_c(SymbolId("hippo8"), DeclaredAs, sept::Float64)), SymbolTypeDecl)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c("hippo9", DeclaredAs, sept::Float64), SymbolTypeDecl)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(SymbolId("hippo9"), DeclaredAs, sept::Float64), SymbolTypeDecl)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(std::string("hippo10"), DeclaredAs, sept::Float64), SymbolTypeDecl)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(sept::Float64, MapsTo, sept::Bool), FuncType)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(sept::Float64, MapsTo, sept::TupleTerm_c(sept::Bool, MapsTo, sept::Uint32)), FuncType)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(FuncPrototypeParams(sept::TupleTerm_c("hippo11", DeclaredAs, sept::Float64)), MapsTo, sept::Bool), FuncPrototype)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(FuncPrototypeParams(sept::TupleTerm_c(std::string("hippo12"), DeclaredAs, sept::Float64)), MapsTo, sept::Bool), FuncPrototype)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(FuncPrototypeParams(sept::TupleTerm_c(std::string("hippo13"), DeclaredAs, sept::Float64), sept::TupleTerm_c(std::string("ostrich44"), DeclaredAs, sept::Uint32)), MapsTo, sept::Bool), FuncPrototype)) << '\n'
               << '\n';
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(ValueTerminal(123.45)) << '\n'
               << LVD_REFLECT(Expr(123.45)) << '\n'
               << LVD_REFLECT(BinOpExpr(123.45, Add, 0.05)) << '\n'
               << '\n';

    EvalCtx ctx;

    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(evaluate_expr_data(123.45, ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(true, ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(BinOpExpr(123.45, Add, 0.05), ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(CondExpr(If, true, Then, 1.2, Else, 0.05), ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(RoundExpr(RoundOpen, ExprArray(1.2), RoundClose), ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(RoundExpr(RoundOpen, ExprArray(1.2, 3.4, 5.6), RoundClose), ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(RoundExpr(RoundOpen, ExprArray(RoundExpr(RoundOpen, ExprArray(false), RoundClose)), RoundClose), ctx)) << '\n'
               << '\n';
    lvd::g_log << lvd::Log::trc()
               << LVD_REFLECT(inhabits_data(RoundOpen, sept::FormalTypeOf(RoundOpen))) << '\n'
               << LVD_REFLECT(inhabits_data(SquareOpen, sept::FormalTypeOf(RoundOpen))) << '\n'
               << LVD_REFLECT(inhabits_data(CurlyOpen, sept::FormalTypeOf(RoundOpen))) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(RoundOpen, ExprArray(1.0), RoundClose), RoundExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(RoundOpen, ExprArray(1.0), RoundClose), SquareExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(RoundOpen, ExprArray(1.0), RoundClose), CurlyExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(SquareOpen, 1.0, SquareClose), RoundExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(SquareOpen, 1.0, SquareClose), SquareExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(SquareOpen, 1.0, SquareClose), CurlyExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(CurlyOpen, 1.0, CurlyClose), RoundExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(CurlyOpen, 1.0, CurlyClose), SquareExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(CurlyOpen, 1.0, CurlyClose), CurlyExpr)) << '\n'
               << '\n';

    // Silly, but hey.
    auto x = 0.1;
    auto three_factorial = 3.0*2.0*1.0;
    auto five_factorial = 5.0*4.0*three_factorial;
    auto seven_factorial = 7.0*6.0*five_factorial;
    auto nine_factorial = 9.0*8.0*seven_factorial;
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(x) << '\n'
               << LVD_REFLECT(three_factorial) << '\n'
               << LVD_REFLECT(five_factorial) << '\n'
               << LVD_REFLECT(seven_factorial) << '\n'
               << LVD_REFLECT(nine_factorial) << '\n'
               << LVD_REFLECT(BinOpExpr(x, Pow, 3.0)) << '\n'
               << LVD_REFLECT(evaluate_expr(BinOpExpr(x, Pow, 3.0), ctx)) << '\n'
               << '\n';
    // sin(x) = x - x^3 / 3! + x^5 / 5! - x^7 / 7! + x^9 / 9! - ...
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(evaluate_expr(BinOpExpr(x, Pow, 9.0), ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr(nine_factorial, ctx)) << '\n'
               << LVD_REFLECT(inhabits(BinOpExpr(x, Pow, 9.0), Expr)) << '\n'
               << LVD_REFLECT(inhabits(Div, BinOp)) << '\n'
               << LVD_REFLECT(inhabits(nine_factorial, Expr)) << '\n'
               << LVD_REFLECT(sept::inhabits_data(BinOpExpr(x, Pow, 9.0), Expr)) << '\n'
               << LVD_REFLECT(sept::inhabits_data(BinOpExpr(x, Pow, 9.0), Expr)) << '\n'
               << LVD_REFLECT(sept::inhabits_data(Div, BinOp)) << '\n'
               << LVD_REFLECT(sept::inhabits_data(nine_factorial, Expr)) << '\n'
               << LVD_REFLECT(inhabits(BinOpExpr(x, Pow, 9.0), BinOpExpr)) << '\n'
               << LVD_REFLECT(inhabits(BinOpExpr(x, Pow, 9.0), CondExpr)) << '\n'
               << LVD_REFLECT(inhabits(BinOpExpr(x, Pow, 9.0), Expr)) << '\n'
               << '\n';
    lvd::g_log << lvd::Log::dbg() << evaluate_expr(
        BinOpExpr(
            BinOpExpr(x, Pow, 9.0),
            Div,
            nine_factorial
        ),
        ctx
    ) << '\n';

    lvd::g_log << lvd::Log::dbg() << evaluate_expr(
        BinOpExpr(
            BinOpExpr(
                BinOpExpr(
                    BinOpExpr(
                        x,
                        Sub,
                        BinOpExpr(
                            BinOpExpr(x, Pow, 3.0),
                            Div,
                            three_factorial
                        )
                    ),
                    Add,
                    BinOpExpr(
                        BinOpExpr(x, Pow, 5.0),
                        Div,
                        five_factorial
                    )
                ),
                Sub,
                BinOpExpr(
                    BinOpExpr(x, Pow, 7.0),
                    Div,
                    seven_factorial
                )
            ),
            Add,
            BinOpExpr(
                BinOpExpr(x, Pow, 9.0),
                Div,
                nine_factorial
            )
        ),
        ctx
    ) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(std::sin(x)) << '\n';

    auto stuff = sept::Data{sept::Array(sept::True, sept::False, sept::Uint32(456))};
    // Define some global symbols
    ctx.current_scope()->define_symbol(std::string("hippa"), sept::True);
    ctx.current_scope()->define_symbol(std::string("ostrich"), sept::Float64(123.0));
    // Add a reference to the symbol table, demonstrating that nested references work.
    ctx.current_scope()->define_symbol(std::string("donkey"), sept::MemRef(&stuff));
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(sept::GlobalSymRef("ostrich")) << '\n'
               << LVD_REFLECT(sept::Data{sept::GlobalSymRef("ostrich")}) << '\n'
               << LVD_REFLECT(stuff) << '\n'
               << LVD_REFLECT(sept::GlobalSymRef("donkey")) << '\n'
               << LVD_REFLECT(sept::Data{sept::GlobalSymRef("donkey")}) << '\n'
               << '\n'
               << LVD_REFLECT(inhabits(sept::Tuple(std::string("f"), sept::Tuple(std::string("x"))), FuncEval)) << '\n'
               << LVD_REFLECT(evaluate_expr__as_SymbolId(std::string("hippa"), ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr__as_SymbolId(std::string("ostrich"), ctx)) << '\n'
               << '\n';

    // Define a global function.
    ctx.current_scope()->define_symbol(
        SymbolId("square"),
        FuncLiteral(
            FuncPrototype(
                FuncPrototypeParams(
                    SymbolTypeDecl(SymbolId("x"), DeclaredAs, sept::Float64)
                ),
                MapsTo,
                sept::Float64
            ),
            BinOpExpr(SymbolId("x"), Mul, SymbolId("x"))
        )
    );
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(ExprArray(sept::Float64(100.1))) << '\n'
               << LVD_REFLECT(ExprArray(sept::Float64(100.1), sept::Uint32(1234))) << '\n'
               << LVD_REFLECT(RoundExpr(RoundOpen, ExprArray(sept::Float64(100.1)), RoundClose)) << '\n'
               << LVD_REFLECT(FuncEval(SymbolId("square"), RoundExpr(RoundOpen, ExprArray(sept::Float64(100.1)), RoundClose))) << '\n'
               << LVD_REFLECT(evaluate_expr__as_FuncEval(FuncEval(SymbolId("square"), RoundExpr(RoundOpen, ExprArray(sept::Float64(100.1)), RoundClose)), ctx)) << '\n'
               << LVD_REFLECT(Stmt) << '\n'
               << LVD_REFLECT(StmtArray) << '\n'
               << LVD_REFLECT(ExprArray) << '\n'
               << LVD_REFLECT(BlockExpr) << '\n'
               << '\n';

    auto stmt_array = StmtArray(
        SymbolDefn(SymbolId("fwee"), DefinedAs, sept::True),
        SymbolDefn(SymbolId("gwaa"), DefinedAs, sept::Float64(40302.01))
    );
    lvd::g_log << lvd::Log::dbg() << LVD_CALL_SITE() << '\n';
    auto blah = sept::ArrayE(sept::Union(SymbolDefn))(
        SymbolDefn(SymbolId("fwee"), DefinedAs, sept::True),
        SymbolDefn(SymbolId("gwaa"), DefinedAs, sept::Float64(40302.01))
    );
    lvd::g_log << lvd::Log::dbg() << LVD_CALL_SITE() << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(inhabits_data(stmt_array, StmtArray)) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(inhabits_data(blah, StmtArray)) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(stmt_array) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(blah) << '\n';
    bool eq_val = stmt_array == blah;

    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(SymbolDefn) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(SymbolDefn == SymbolDefn) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(Stmt) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(Stmt == Stmt) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(sept::Union(SymbolDefn) == Stmt) << '\n';


    lvd::g_log << lvd::Log::dbg() << "stmt_array == blah" << " = " << eq_val << '\n';
    auto block_expr = BlockExpr(stmt_array, SymbolId("fwee"));
    lvd::g_log << lvd::Log::dbg() << LVD_CALL_SITE() << '\n'
               << LVD_REFLECT(block_expr) << '\n'
               << LVD_REFLECT(inhabits(block_expr, BlockExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(block_expr, BlockExpr)) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_CALL_SITE() << '\n';
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(
                    StmtArray(
                        SymbolDefn(SymbolId("fwee"), DefinedAs, sept::True),
                        SymbolDefn(SymbolId("gwaa"), DefinedAs, sept::Float64(40302.01))
                    )
                  ) << '\n'
               << LVD_REFLECT(
                    BlockExpr(
                        StmtArray(
                            SymbolDefn(SymbolId("fwee"), DefinedAs, sept::True),
                            SymbolDefn(SymbolId("gwaa"), DefinedAs, sept::Float64(40302.01)),
                            Assignment(SymbolId("fwee"), AssignFrom, sept::Float64(8888.55))
                        ),
                        SymbolId("fwee")
                    )
                  ) << '\n'
               << LVD_REFLECT(
                    evaluate_expr__as_BlockExpr(
                        BlockExpr(
                            StmtArray(
                                SymbolDefn(SymbolId("fwee"), DefinedAs, sept::True),
                                SymbolDefn(SymbolId("gwaa"), DefinedAs, sept::Float64(40302.01)),
                                Assignment(SymbolId("fwee"), AssignFrom, sept::Float64(8888.55))
                            ),
                            SymbolId("fwee")
                        ),
                        ctx
                    ).deref()
                  ) << '\n'
               << '\n';

    // Define another global function.
    ctx.current_scope()->define_symbol(
        SymbolId("exp"),
        FuncLiteral(
            FuncPrototype(
                FuncPrototypeParams(
                    SymbolTypeDecl(SymbolId("x"), DeclaredAs, sept::Float64)
                ),
                MapsTo,
                sept::Float64
            ),
            BlockExpr(
                StmtArray(
                    SymbolDefn(SymbolId("retval"), DefinedAs, 0.0),
                    SymbolDefn(SymbolId("i"), DefinedAs, 0.0),
                    SymbolDefn(SymbolId("accumulator"), DefinedAs, 1.0),

                    // Just do an unrolled loop for now.

                    Assignment(SymbolId("retval"), AssignFrom, BinOpExpr(SymbolId("retval"), Add, SymbolId("accumulator"))),
                    Assignment(SymbolId("i"), AssignFrom, BinOpExpr(SymbolId("i"), Add, 1.0)),
                    Assignment(SymbolId("accumulator"), AssignFrom, BinOpExpr(BinOpExpr(SymbolId("accumulator"), Mul, SymbolId("x")), Div, SymbolId("i"))),

                    Assignment(SymbolId("retval"), AssignFrom, BinOpExpr(SymbolId("retval"), Add, SymbolId("accumulator"))),
                    Assignment(SymbolId("i"), AssignFrom, BinOpExpr(SymbolId("i"), Add, 1.0)),
                    Assignment(SymbolId("accumulator"), AssignFrom, BinOpExpr(BinOpExpr(SymbolId("accumulator"), Mul, SymbolId("x")), Div, SymbolId("i"))),

                    Assignment(SymbolId("retval"), AssignFrom, BinOpExpr(SymbolId("retval"), Add, SymbolId("accumulator"))),
                    Assignment(SymbolId("i"), AssignFrom, BinOpExpr(SymbolId("i"), Add, 1.0)),
                    Assignment(SymbolId("accumulator"), AssignFrom, BinOpExpr(BinOpExpr(SymbolId("accumulator"), Mul, SymbolId("x")), Div, SymbolId("i"))),

                    Assignment(SymbolId("retval"), AssignFrom, BinOpExpr(SymbolId("retval"), Add, SymbolId("accumulator"))),
                    Assignment(SymbolId("i"), AssignFrom, BinOpExpr(SymbolId("i"), Add, 1.0)),
                    Assignment(SymbolId("accumulator"), AssignFrom, BinOpExpr(BinOpExpr(SymbolId("accumulator"), Mul, SymbolId("x")), Div, SymbolId("i"))),

                    Assignment(SymbolId("retval"), AssignFrom, BinOpExpr(SymbolId("retval"), Add, SymbolId("accumulator"))),
                    Assignment(SymbolId("i"), AssignFrom, BinOpExpr(SymbolId("i"), Add, 1.0)),
                    Assignment(SymbolId("accumulator"), AssignFrom, BinOpExpr(BinOpExpr(SymbolId("accumulator"), Mul, SymbolId("x")), Div, SymbolId("i"))),

                    Assignment(SymbolId("retval"), AssignFrom, BinOpExpr(SymbolId("retval"), Add, SymbolId("accumulator"))),
                    Assignment(SymbolId("i"), AssignFrom, BinOpExpr(SymbolId("i"), Add, 1.0)),
                    Assignment(SymbolId("accumulator"), AssignFrom, BinOpExpr(BinOpExpr(SymbolId("accumulator"), Mul, SymbolId("x")), Div, SymbolId("i"))),

                    Assignment(SymbolId("retval"), AssignFrom, BinOpExpr(SymbolId("retval"), Add, SymbolId("accumulator"))),
                    Assignment(SymbolId("i"), AssignFrom, BinOpExpr(SymbolId("i"), Add, 1.0)),
                    Assignment(SymbolId("accumulator"), AssignFrom, BinOpExpr(BinOpExpr(SymbolId("accumulator"), Mul, SymbolId("x")), Div, SymbolId("i")))
                ),
                SymbolId("retval")
            )
        )
    );
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(evaluate_expr_data(FuncEval(SymbolId("exp"), RoundExpr(RoundOpen, ExprArray(0.1), RoundClose)), ctx)) << '\n'
               << LVD_REFLECT(std::exp(0.1)) << '\n';

    // Testing ElementEval
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(evaluate_expr_data(ElementEval(sept::Array(0.5, 1.5, 2.5, 3.5), SquareExpr(SquareOpen, sept::Uint32(2), SquareClose)), ctx)) << '\n';

    // Testing Construction
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(evaluate_expr_data(Construction(sept::ArrayES(sept::Float64,sept::Uint32(2)), CurlyExpr(CurlyOpen, sept::Array(1.5, 3.5), CurlyClose)), ctx)) << '\n';


    // Attempt to use a typedef
    ctx.current_scope()->define_symbol(SymbolId("Complex"), sept::ArrayES(sept::Float64, sept::Uint32(2)));
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(evaluate_expr_data(SymbolId("Complex"), ctx).deref()) << '\n'
               << LVD_REFLECT(evaluate_expr_data(SymbolId("Complex"), ctx).deref()) << '\n'
               << LVD_REFLECT(evaluate_expr_data(Construction(SymbolId("Complex"), CurlyExpr(CurlyOpen, sept::Array(8.1, 9.2), CurlyClose)), ctx)) << '\n'
               << LVD_REFLECT(
                    inhabits_data(
                        evaluate_expr_data(Construction(SymbolId("Complex"), CurlyExpr(CurlyOpen, sept::Array(8.1, 9.2), CurlyClose)), ctx),
                        SymbolId("Complex")
                    )
                ) << '\n'
               << LVD_REFLECT(
                    inhabits_data(
                        sept::Array(8.1, 9.2),
                        evaluate_expr_data(SymbolId("Complex"), ctx)
                    )
                ) << '\n'
               << LVD_REFLECT(sept::Array(8.1, 9.2)) << '\n'
               << LVD_REFLECT(sept::ArrayES(sept::Float64, sept::Uint32(2))(8.1, 9.2)) << '\n'
               << LVD_REFLECT(
                    inhabits_data(
                        sept::Array(8.1, 9.2),
                        sept::Array
                    )
                ) << '\n'
               << LVD_REFLECT(
                    inhabits_data(
                        sept::Array(8.1, 9.2),
                        sept::ArrayES(sept::Float64, sept::Uint32(2))
                    )
                ) << '\n'
               << LVD_REFLECT(
                    inhabits_data(
                        sept::ArrayES(sept::Float64, sept::Uint32(2))(8.1, 9.2),
                        sept::Array
                    )
                ) << '\n'
               << LVD_REFLECT(
                    inhabits_data(
                        sept::ArrayES(sept::Float64, sept::Uint32(2))(8.1, 9.2),
                        sept::ArrayES(sept::Float64, sept::Uint32(2))
                    )
                ) << '\n'
               << LVD_REFLECT(
                    inhabits_data(
                        sept::Array(8.1, 9.2),
                        evaluate_expr_data(SymbolId("Complex"), ctx)
                    )
                ) << '\n'
               << LVD_REFLECT(
                    inhabits_data(
                        sept::ArrayES(sept::Float64, sept::Uint32(2))(8.1, 9.2),
                        evaluate_expr_data(SymbolId("Complex"), ctx)
                    )
                ) << '\n'
               << LVD_REFLECT(
                    evaluate_expr_data(sept::ArrayES(sept::Float64, sept::Uint32(2))(8.1, 9.2), ctx)
                ) << '\n'
               << '\n';

    ctx.current_scope()->define_symbol(
        SymbolId("Complex_square"),
        FuncLiteral(
            FuncPrototype(
                FuncPrototypeParams(
                    SymbolTypeDecl(SymbolId("z"), DeclaredAs, SymbolId("Complex"))
                ),
                MapsTo,
                SymbolId("Complex")
            ),
            BlockExpr(
                StmtArray(
                    SymbolDefn(SymbolId("re"), DefinedAs, ElementEval(SymbolId("z"), SquareExpr(SquareOpen, sept::Uint32(0), SquareClose))),
                    SymbolDefn(SymbolId("im"), DefinedAs, ElementEval(SymbolId("z"), SquareExpr(SquareOpen, sept::Uint32(1), SquareClose)))
                ),
                Construction(
                    SymbolId("Complex"),
                    CurlyExpr(
                        CurlyOpen,
                        sept::Array(
                            BinOpExpr(BinOpExpr(SymbolId("re"), Mul, SymbolId("re")), Sub, BinOpExpr(SymbolId("im"), Mul, SymbolId("im"))),
                            BinOpExpr(2.0, Mul, BinOpExpr(SymbolId("re"), Mul, SymbolId("im")))
                        ),
                        CurlyClose
                    )
                )
            )
        )
    );
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(
                    evaluate_expr_data(
                        FuncEval(
                            SymbolId("Complex_square"),
                            RoundExpr(
                                RoundOpen,
                                ExprArray(
                                    Construction(
                                        SymbolId("Complex"),
                                        CurlyExpr(
                                            CurlyOpen,
                                            sept::Array(3.0, 4.0),
                                            CurlyClose
                                        )
                                    )
                                ),
                                RoundClose
                            )
                        ),
                        ctx
                    )
                ) << '\n';

    ctx.current_scope()->define_symbol(
        SymbolId("Complex_mul"),
        FuncLiteral(
            FuncPrototype(
                FuncPrototypeParams(
                    SymbolTypeDecl(SymbolId("w"), DeclaredAs, SymbolId("Complex")),
                    SymbolTypeDecl(SymbolId("z"), DeclaredAs, SymbolId("Complex"))
                ),
                MapsTo,
                SymbolId("Complex")
            ),
            BlockExpr(
                StmtArray(
                    SymbolDefn(SymbolId("w_re"), DefinedAs, ElementEval(SymbolId("w"), SquareExpr(SquareOpen, sept::Uint32(0), SquareClose))),
                    SymbolDefn(SymbolId("w_im"), DefinedAs, ElementEval(SymbolId("w"), SquareExpr(SquareOpen, sept::Uint32(1), SquareClose))),
                    SymbolDefn(SymbolId("z_re"), DefinedAs, ElementEval(SymbolId("z"), SquareExpr(SquareOpen, sept::Uint32(0), SquareClose))),
                    SymbolDefn(SymbolId("z_im"), DefinedAs, ElementEval(SymbolId("z"), SquareExpr(SquareOpen, sept::Uint32(1), SquareClose)))
                ),
                Construction(
                    SymbolId("Complex"),
                    CurlyExpr(
                        CurlyOpen,
                        sept::Array(
                            BinOpExpr(BinOpExpr(SymbolId("w_re"), Mul, SymbolId("z_re")), Sub, BinOpExpr(SymbolId("w_im"), Mul, SymbolId("z_im"))),
                            BinOpExpr(BinOpExpr(SymbolId("w_re"), Mul, SymbolId("z_im")), Add, BinOpExpr(SymbolId("w_im"), Mul, SymbolId("z_re")))
                        ),
                        CurlyClose
                    )
                )
            )
        )
    );
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(
                    evaluate_expr_data(
                        FuncEval(
                            SymbolId("Complex_mul"),
                            RoundExpr(
                                RoundOpen,
                                ExprArray(
                                    Construction(
                                        SymbolId("Complex"),
                                        CurlyExpr(
                                            CurlyOpen,
                                            sept::Array(3.0, 4.0),
                                            CurlyClose
                                        )
                                    ),
                                    Construction(
                                        SymbolId("Complex"),
                                        CurlyExpr(
                                            CurlyOpen,
                                            sept::Array(1.0, -2.0),
                                            CurlyClose
                                        )
                                    )
                                ),
                                RoundClose
                            )
                        ),
                        ctx
                    )
                ) << '\n';

    return 0;
}
