#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <lvd/abort.hpp>
#include <lvd/call_site.hpp>
#include <lvd/comma.hpp>
#include <lvd/fmt.hpp>
#include <lvd/g_log.hpp>
#include "sept/ArrayTerm.hpp"
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
// StaticAssociation_t for evaluate
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
    // LogicalBinOp
    AND = 0,
    OR,
    XOR,
    __LogicalBinOp_LOWEST__ = AND,
    __LogicalBinOp_HIGHEST__ = XOR,

    // LogicalUnOp
    NOT,
    __LogicalUnOp_LOWEST__ = NOT,
    __LogicalUnOp_HIGHEST__ = NOT,

    __Logical_LOWEST__ = __LogicalBinOp_LOWEST__,
    __Logical_HIGHEST__ = __LogicalUnOp_HIGHEST__,

    // NumericBinOp
    ADD,
    SUB,
    MUL,
    DIV,
    POW,
    __NumericBinOp_LOWEST__ = ADD,
    __NumericBinOp_HIGHEST__ = POW,

    // NumericUnOp
    NEG,
    __NumericUnOp_LOWEST__ = NEG,
    __NumericUnOp_HIGHEST__ = NEG,

    __Numeric_LOWEST__ = __NumericBinOp_LOWEST__,
    __Numeric_HIGHEST__ = __NumericUnOp_HIGHEST__,

    //
    // Misc -- TODO: Categorize
    //

    MAPS_TO,        // ->
    DECLARED_AS,    // :
    DEFINED_AS,     // ::=
    __Misc_LOWEST__ = MAPS_TO,
    __Misc_HIGHEST__ = DEFINED_AS,

    __LOWEST__ = __Logical_LOWEST__,
    __HIGHEST__ = __Misc_HIGHEST__,
};

size_t constexpr AST_NP_TERM_COUNT = size_t(ASTNPTerm::__HIGHEST__)+1 - size_t(ASTNPTerm::__LOWEST__);

std::string const &as_string (ASTNPTerm t) {
    static std::array<std::string,AST_NP_TERM_COUNT> const TABLE{
        // LogicalBinOp
        "And",
        "Or",
        "Xor",
        // LogicalUnOp
        "Not",
        // NumericBinOp
        "Add",
        "Sub",
        "Mul",
        "Div",
        "Pow",
        // NumericUnOp
        "Neg",
        // Misc
        "MapsTo",
        "DeclaredAs",
        "DefinedAs",
    };
    return TABLE.at(size_t(t));
}

std::ostream &operator<< (std::ostream &out, ASTNPTerm const &t) {
    return out << as_string(t);
}

int compare (ASTNPTerm const &lhs, ASTNPTerm const &rhs) {
    return compare(ASTNPTermRepr(lhs), ASTNPTermRepr(rhs));
}


// LogicalBinOp
ASTNPTerm const And{ASTNPTerm::AND};
ASTNPTerm const Or{ASTNPTerm::OR};
ASTNPTerm const Xor{ASTNPTerm::XOR};
// LogicalUnOp
ASTNPTerm const Not{ASTNPTerm::NOT};
// NumericBinOp
ASTNPTerm const Add{ASTNPTerm::ADD};
ASTNPTerm const Sub{ASTNPTerm::SUB};
ASTNPTerm const Mul{ASTNPTerm::MUL};
ASTNPTerm const Div{ASTNPTerm::DIV};
ASTNPTerm const Pow{ASTNPTerm::POW};
// NumericUnOp
ASTNPTerm const Neg{ASTNPTerm::NEG};
// Misc
ASTNPTerm const MapsTo{ASTNPTerm::MAPS_TO};
ASTNPTerm const DeclaredAs{ASTNPTerm::DECLARED_AS};
ASTNPTerm const DefinedAs{ASTNPTerm::DEFINED_AS};

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

struct Any_c { ABSTRACT_TYPE_CONSTRUCTOR };
struct LogicalBinOp_c { ABSTRACT_TYPE_CONSTRUCTOR };
struct LogicalUnOp_c { ABSTRACT_TYPE_CONSTRUCTOR };
struct NumericBinOp_c { ABSTRACT_TYPE_CONSTRUCTOR };
struct NumericUnOp_c { ABSTRACT_TYPE_CONSTRUCTOR };
// TODO: Type intersections (LogicalExpr := Intersection(Expr,Logical)) and/or

struct SymbolId_c {
    std::string operator() (char const *c_str) const {
        // Cast to std::string and forward.
        return this->operator()(std::string(c_str));
    }
    ABSTRACT_TYPE_CONSTRUCTOR
};

std::ostream &operator<< (std::ostream &out, Any_c const &) { return out << "Any"; }
std::ostream &operator<< (std::ostream &out, LogicalBinOp_c const &) { return out << "LogicalBinOp"; }
std::ostream &operator<< (std::ostream &out, LogicalUnOp_c const &) { return out << "LogicalUnOp"; }
std::ostream &operator<< (std::ostream &out, NumericBinOp_c const &) { return out << "NumericBinOp"; }
std::ostream &operator<< (std::ostream &out, NumericUnOp_c const &) { return out << "NumericUnOp"; }
std::ostream &operator<< (std::ostream &out, SymbolId_c const &) { return out << "SymbolId"; }

inline sept::NonParametricType_c abstract_type_of (Any_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (LogicalBinOp_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (LogicalUnOp_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (NumericBinOp_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (NumericUnOp_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (SymbolId_c const &) { return sept::NonParametricType; }

Any_c const Any;

LogicalBinOp_c const LogicalBinOp;
LogicalUnOp_c const LogicalUnOp;
NumericBinOp_c const NumericBinOp;
NumericUnOp_c const NumericUnOp;
SymbolId_c const SymbolId;

sept::UnionTerm_c const BinOp{LogicalBinOp, NumericBinOp};
sept::UnionTerm_c const UnOp{LogicalUnOp, NumericUnOp};

// TODO: Figure out how to keep this as sept::UnionTerm_c so that its usage doesn't require Data indirection.
extern sept::Data Expr_as_Data;
sept::RefTerm_c const Expr_as_Ref{sept::MemRef(&Expr_as_Data)};

// TODO: Figure out how to keep this as sept::UnionTerm_c so that its usage doesn't require Data indirection.
extern sept::Data LogicalExpr_as_Data;
sept::RefTerm_c const LogicalExpr_as_Ref{sept::MemRef(&LogicalExpr_as_Data)};

// TODO: Figure out how to keep this as sept::UnionTerm_c so that its usage doesn't require Data indirection.
extern sept::Data NumericExpr_as_Data;
sept::RefTerm_c const NumericExpr_as_Ref{sept::MemRef(&NumericExpr_as_Data)};

// TODO: Figure out how to keep this as sept::UnionTerm_c so that its usage doesn't require Data indirection.
extern sept::Data TypeExpr_as_Data;
sept::RefTerm_c const TypeExpr_as_Ref{sept::MemRef(&TypeExpr_as_Data)};

// These are effectively structural subtypes (indistinguishable from sept::Bool and sept::Float64 respectively)
sept::UnionTerm_c const LogicalValueTerminal{sept::Bool, sept::TrueType, sept::FalseType};
sept::Float64_c const NumericValueTerminal;
sept::UnionTerm_c const ValueTerminal{LogicalValueTerminal, NumericValueTerminal};
sept::UnionTerm_c const LogicalTypeTerminal{sept::BoolType}; // Note that TrueType and FalseType both inhabit BoolType.
sept::UnionTerm_c const NumericTypeTerminal{sept::Float64Type};
sept::UnionTerm_c const TypeTerminal{LogicalTypeTerminal, NumericTypeTerminal};
// TODO: Could define variables for each as well

sept::UnionTerm_c const LogicalOp{LogicalBinOp, LogicalUnOp};
sept::UnionTerm_c const Logical{LogicalExpr_as_Ref, LogicalOp, LogicalValueTerminal, LogicalTypeTerminal};
sept::UnionTerm_c const NumericOp{NumericBinOp, NumericUnOp};
sept::UnionTerm_c const Numeric{NumericExpr_as_Ref, NumericOp, NumericValueTerminal, NumericTypeTerminal};

// These are effectively structural subtypes (indistinguishable from the specific tuple terms)
sept::TupleTerm_c const BinOpExpr{Expr_as_Ref, BinOp, Expr_as_Ref};
sept::TupleTerm_c const UnOpExpr{Expr_as_Ref, UnOp, Expr_as_Ref};
sept::TupleTerm_c const CondExpr{LogicalExpr_as_Ref, Expr_as_Ref, Expr_as_Ref};
sept::TupleTerm_c const ParenExpr{Expr_as_Ref};

sept::TupleTerm_c const LogicalBinOpExpr{LogicalExpr_as_Ref, LogicalBinOp, LogicalExpr_as_Ref};
sept::TupleTerm_c const NumericBinOpExpr{NumericExpr_as_Ref, NumericBinOp, NumericExpr_as_Ref};

sept::TupleTerm_c const LogicalUnOpExpr{LogicalUnOp, LogicalExpr_as_Ref};
sept::TupleTerm_c const NumericUnOpExpr{NumericUnOp, NumericExpr_as_Ref};

sept::TupleTerm_c const LogicalCondExpr{LogicalExpr_as_Ref, LogicalExpr_as_Ref, LogicalExpr_as_Ref};
sept::TupleTerm_c const NumericCondExpr{LogicalExpr_as_Ref, NumericExpr_as_Ref, NumericExpr_as_Ref};

sept::TupleTerm_c const LogicalParenExpr{LogicalExpr_as_Ref};
sept::TupleTerm_c const NumericParenExpr{NumericExpr_as_Ref};

// TODO: SymbolId has to be std::string basically
sept::TupleTerm_c const SymbolTypeDecl{SymbolId, sept::FormalTypeOf(DeclaredAs), TypeExpr_as_Ref};
sept::TupleTerm_c const FuncType{TypeExpr_as_Ref, sept::FormalTypeOf(MapsTo), TypeExpr_as_Ref};
sept::TupleTerm_c const FuncPrototype{SymbolTypeDecl, sept::FormalTypeOf(MapsTo), TypeExpr_as_Ref};
sept::TupleTerm_c const FuncLiteral{FuncPrototype, Expr_as_Ref};

sept::TupleTerm_c const FuncEval{SymbolId, ParenExpr};

sept::Data Expr_as_Data{sept::UnionTerm_c{BinOpExpr, CondExpr, FuncEval, ParenExpr, UnOpExpr, ValueTerminal, SymbolId}};
sept::UnionTerm_c const &Expr{Expr_as_Data.cast<sept::UnionTerm_c const &>()};

sept::Data LogicalExpr_as_Data{sept::UnionTerm_c{LogicalBinOpExpr, LogicalCondExpr, LogicalParenExpr, LogicalUnOpExpr, LogicalValueTerminal, SymbolId}};
sept::UnionTerm_c const &LogicalExpr{LogicalExpr_as_Data.cast<sept::UnionTerm_c const &>()};

sept::Data NumericExpr_as_Data{sept::UnionTerm_c{NumericBinOpExpr, NumericCondExpr, NumericParenExpr, NumericUnOpExpr, NumericValueTerminal, SymbolId}};
sept::UnionTerm_c const &NumericExpr{NumericExpr_as_Data.cast<sept::UnionTerm_c const &>()};

// TODO: More type constructions
sept::Data TypeExpr_as_Data{sept::UnionTerm_c{TypeTerminal, FuncType, FuncPrototype, SymbolId}};
sept::UnionTerm_c const &TypeExpr{TypeExpr_as_Data.cast<sept::UnionTerm_c const &>()};

//
// Definitions of inhabits
//

inline sept::True_c constexpr inhabits (ASTNPTerm t, Any_c const &) { return sept::True; }

inline bool constexpr inhabits (ASTNPTerm t, LogicalBinOp_c const &) {
    return ASTNPTermRepr(ASTNPTerm::__LogicalBinOp_LOWEST__) <= ASTNPTermRepr(t)
        && ASTNPTermRepr(t) <= ASTNPTermRepr(ASTNPTerm::__LogicalBinOp_HIGHEST__);
}

inline bool constexpr inhabits (ASTNPTerm t, LogicalUnOp_c const &) {
    return ASTNPTermRepr(ASTNPTerm::__LogicalUnOp_LOWEST__) <= ASTNPTermRepr(t)
        && ASTNPTermRepr(t) <= ASTNPTermRepr(ASTNPTerm::__LogicalUnOp_HIGHEST__);
}

inline bool constexpr inhabits (ASTNPTerm t, NumericBinOp_c const &) {
    return ASTNPTermRepr(ASTNPTerm::__NumericBinOp_LOWEST__) <= ASTNPTermRepr(t)
        && ASTNPTermRepr(t) <= ASTNPTermRepr(ASTNPTerm::__NumericBinOp_HIGHEST__);
}
inline bool constexpr inhabits (ASTNPTerm t, NumericUnOp_c const &) {
    return ASTNPTermRepr(ASTNPTerm::__NumericUnOp_LOWEST__) <= ASTNPTermRepr(t)
        && ASTNPTermRepr(t) <= ASTNPTermRepr(ASTNPTerm::__NumericUnOp_HIGHEST__);
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

    void push_scope () {
        m_current_scope = m_current_scope->push_symbol_table();
    }
    void pop_scope () {
        // This will throw if there is no parent.
        m_current_scope = m_current_scope->parent_symbol_table();
    }

private:

    lvd::nnsp<sept::SymbolTable> m_current_scope;
};

// Forward declaration
sept::Data evaluate_expr__as_SymbolId (std::string const &symbol_id, EvalCtx &ctx);

bool evaluate_expr (bool const &expr, EvalCtx &ctx) { return expr; }
// bool evaluate_expr (sept::BoolTerm_c const &expr, EvalCtx &ctx) { return expr; }
sept::BoolTerm_c evaluate_expr (sept::BoolTerm_c const &expr, EvalCtx &ctx) { return expr; }
double evaluate_expr (double const &expr, EvalCtx &ctx) { return expr; }
sept::Data evaluate_expr (std::string const &expr, EvalCtx &ctx) {
    return evaluate_expr__as_SymbolId(expr, ctx);
}

template <typename T_>
T_ evaluate_expr__as_BinOpExpr (sept::TupleTerm_c const &t, EvalCtx &ctx) {
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << lvd::IndentGuard() << '\n'
//                << LVD_REFLECT(typeid(T_).name()) << '\n'
//                << LVD_REFLECT(t) << '\n'
//                << LVD_REFLECT(BinOpExpr) << '\n'
//                << LVD_REFLECT(LogicalBinOpExpr) << '\n'
//                << LVD_REFLECT(NumericBinOpExpr) << '\n'
//                << LVD_REFLECT(inhabits(t, BinOpExpr)) << '\n'
//                << LVD_REFLECT(inhabits(t, LogicalBinOpExpr)) << '\n'
//                << LVD_REFLECT(inhabits(t, NumericBinOpExpr)) << '\n'
//                << '\n';
    assert(inhabits(t, BinOpExpr));
    if constexpr (std::is_same_v<T_,bool>) {
        if (!inhabits(t, LogicalBinOpExpr)) {
            throw std::runtime_error(LVD_FMT("failed condition: inhabits(t, LogicalBinOpExpr), where t = " << t << " and LogicalBinOpExpr = " << LogicalBinOpExpr));
        }
        assert(!inhabits(t, NumericBinOpExpr));
    } else {
        if (!inhabits(t, NumericBinOpExpr)) {
            throw std::runtime_error(LVD_FMT("failed condition: inhabits(t, NumericBinOpExpr), where t = " << t << " and NumericBinOpExpr = " << NumericBinOpExpr));
        }
        assert(!inhabits(t, LogicalBinOpExpr));
    }

    auto lhs = evaluate_expr_data(t[0], ctx).cast<T_>();
    auto bin_op = t[1].cast<ASTNPTerm>();
    auto rhs = evaluate_expr_data(t[2], ctx).cast<T_>();
    if constexpr (std::is_same_v<T_,bool>) {
        switch (bin_op) {
            case ASTNPTerm::AND: return lhs && rhs;
            case ASTNPTerm::OR:  return lhs || rhs;
            case ASTNPTerm::XOR: return lhs != rhs;
            default: LVD_ABORT(LVD_FMT("invalid ASTNPTerm for use as a BinOp for Bool: " << uint32_t(bin_op)));
        }
    } else {
        switch (bin_op) {
            case ASTNPTerm::ADD: return lhs + rhs;
            case ASTNPTerm::SUB: return lhs - rhs;
            case ASTNPTerm::MUL: return lhs * rhs;
            case ASTNPTerm::DIV: return lhs / rhs;
            case ASTNPTerm::POW: return std::pow(lhs, rhs);
            default: LVD_ABORT(LVD_FMT("invalid ASTNPTerm for use as a BinOp for Float64: " << uint32_t(bin_op)));
        }
    }
}

template <typename T_>
T_ evaluate_expr__as_UnOpExpr (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    if (!inhabits(t, UnOpExpr)) {
        throw std::runtime_error(LVD_FMT("failed condition: inhabits(t, UnOpExpr), where t = " << t << " and UnOpExpr = " << UnOpExpr));
    }
    assert(inhabits(t, UnOpExpr));

    auto un_op = t[0].cast<ASTNPTerm>();
    auto operand = evaluate_expr_data(t[1], ctx).cast<T_>();
    switch (un_op) {
        case ASTNPTerm::NOT: return !operand;
        case ASTNPTerm::NEG: return -operand;
        default: LVD_ABORT(LVD_FMT("invalid ASTNPTerm for use as an UnOp: " << uint32_t(un_op)));
    }
}

template <typename T_>
T_ evaluate_expr__as_CondExpr (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    assert(inhabits(t, CondExpr));

    auto condition = evaluate_expr_data(t[0], ctx).cast<bool>();
    return evaluate_expr_data(t[condition ? 1 : 2], ctx).cast<T_>();
}

template <typename T_>
T_ evaluate_expr__as_ParenExpr (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    assert(inhabits(t, ParenExpr));
    return evaluate_expr_data(t[0], ctx).cast<T_>();
}

sept::Data evaluate_expr__as_SymbolId (std::string const &symbol_id, EvalCtx &ctx) {
    assert(inhabits(symbol_id, SymbolId));
    return sept::LocalSymRef(symbol_id, ctx.current_scope());
}

sept::Data evaluate_expr__as_FuncEval (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    assert(inhabits(t, FuncEval));
    // Resolve the function symbol
    auto func = ctx.current_scope()->resolve_symbol_const(t[0].cast<std::string>());
    lvd::g_log << lvd::Log::trc() << '\n' << LVD_CALL_SITE() << " - " << LVD_REFLECT(func) << '\n';
    assert(sept::inhabits_data(func, FuncLiteral));
    auto param = t[1];
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(param) << '\n';
    auto evaled_param = evaluate_expr_data(param, ctx);
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(evaled_param) << '\n';
    // Identify the parameter symbol decl
    // NOTE: This probably copy-constructs, instead of returning a const ref.
    // TODO: Could have the stuff return a MemRef.
    auto func_prototype = func[0];
    auto symbol_decl = func_prototype[0];
    auto symbol_id = symbol_decl[0];
    assert(symbol_id.can_cast<std::string>());
    auto symbol_type = symbol_decl[2]; // TODO: Change this to eval, so that symbolic references for types are possible.
    // Check that the parameter is the correct type
    if (!sept::inhabits_data(evaled_param, symbol_type))
        throw std::runtime_error(LVD_FMT("Expected parameter " << param << " to evaluate to a term of type " << symbol_type << " but it didn't"));

    auto return_type = func_prototype[2];

    // Push a context, define the function param(s).
    // TODO: Use a scope guard here
    ctx.push_scope();
    ctx.current_scope()->define_symbol(symbol_id.cast<std::string>(), std::move(evaled_param));
    // Evaluate the function body using the new scope.
    auto func_body = func[1];
    auto retval = evaluate_expr_data(func_body, ctx);
    // Pop the function local scope.
    ctx.pop_scope();
    // Check that the parameter is the correct type
    if (!sept::inhabits_data(retval, return_type))
        throw std::runtime_error(LVD_FMT("Expected return value " << retval << " to evaluate to a term of type " << return_type << " but it didn't"));
    return retval;
}

sept::Data evaluate_expr (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    // TODO: Make this efficient -- some sort of poset search.
    if (inhabits(t, LogicalBinOpExpr))
        return evaluate_expr__as_BinOpExpr<bool>(t, ctx);
    else if (inhabits(t, LogicalCondExpr))
        return evaluate_expr__as_CondExpr<bool>(t, ctx);
    else if (inhabits(t, LogicalParenExpr))
        return evaluate_expr__as_ParenExpr<bool>(t, ctx);
    else if (inhabits(t, LogicalUnOpExpr))
        return evaluate_expr__as_UnOpExpr<bool>(t, ctx);
    else if (inhabits(t, NumericBinOpExpr))
        return evaluate_expr__as_BinOpExpr<double>(t, ctx);
    else if (inhabits(t, NumericCondExpr))
        return evaluate_expr__as_CondExpr<double>(t, ctx);
    else if (inhabits(t, NumericParenExpr))
        return evaluate_expr__as_ParenExpr<double>(t, ctx);
    else if (inhabits(t, NumericUnOpExpr))
        return evaluate_expr__as_UnOpExpr<double>(t, ctx);
    else
        LVD_ABORT(LVD_FMT("unrecognized Expr: " << t));
}

//
// sept data model registrations
//

namespace sept {
SEPT__REGISTER__PRINT(ASTNPTerm)
SEPT__REGISTER__PRINT(Any_c)
SEPT__REGISTER__PRINT(LogicalBinOp_c)
SEPT__REGISTER__PRINT(LogicalUnOp_c)
SEPT__REGISTER__PRINT(NumericBinOp_c)
SEPT__REGISTER__PRINT(NumericUnOp_c)
SEPT__REGISTER__PRINT(SymbolId_c)
SEPT__REGISTER__PRINT__GIVE_ID(char const *, __char_const_ptr__)

SEPT__REGISTER__EQ(Any_c)
SEPT__REGISTER__EQ(LogicalBinOp_c)
SEPT__REGISTER__EQ(LogicalUnOp_c)
SEPT__REGISTER__EQ(NumericBinOp_c)
SEPT__REGISTER__EQ(NumericUnOp_c)
SEPT__REGISTER__EQ(ASTNPTerm)

SEPT__REGISTER__ABSTRACT_TYPE_OF(Any_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(LogicalBinOp_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(LogicalUnOp_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(NumericBinOp_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(NumericUnOp_c)

SEPT__REGISTER__INHABITS__NONDATA(ASTNPTerm,     LogicalBinOp_c)
SEPT__REGISTER__INHABITS__NONDATA(ASTNPTerm,     LogicalUnOp_c)
SEPT__REGISTER__INHABITS__NONDATA(ASTNPTerm,     NumericBinOp_c)
SEPT__REGISTER__INHABITS__NONDATA(ASTNPTerm,     NumericUnOp_c)
SEPT__REGISTER__INHABITS__NONDATA(ASTNPTerm,     FormalTypeOf_Term_c)
SEPT__REGISTER__INHABITS__GIVE_ID__NONDATA(std::string,   SymbolId_c, __std_string___SymbolId_c__)
SEPT__REGISTER__INHABITS__GIVE_ID__NONDATA(char const *,  SymbolId_c, __char_const_ptr___SymbolId_c__)
// TODO there are probably some missing

SEPT__REGISTER__COMPARE__SINGLETON(Any_c)
SEPT__REGISTER__COMPARE__SINGLETON(LogicalBinOp_c)
SEPT__REGISTER__COMPARE__SINGLETON(LogicalUnOp_c)
SEPT__REGISTER__COMPARE__SINGLETON(NumericBinOp_c)
SEPT__REGISTER__COMPARE__SINGLETON(NumericUnOp_c)
SEPT__REGISTER__COMPARE(ASTNPTerm, ASTNPTerm)

SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(LogicalBinOp_c,      ASTNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(LogicalUnOp_c,       ASTNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(NumericBinOp_c,      ASTNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(NumericUnOp_c,       ASTNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(FormalTypeOf_Term_c, ASTNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF___GIVE_ID__ABSTRACT_TYPE(SymbolId_c, std::string, __SymbolId_c___std_string__)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF___GIVE_ID__ABSTRACT_TYPE(SymbolId_c, char const *, __SymbolId_c___char_const_ptr__)

SEPT__REGISTER__EVALUATE_EXPR(bool)
SEPT__REGISTER__EVALUATE_EXPR(True_c)
SEPT__REGISTER__EVALUATE_EXPR(False_c)
SEPT__REGISTER__EVALUATE_EXPR(double)
SEPT__REGISTER__EVALUATE_EXPR__GIVE_ID(std::string, __std_string__)
SEPT__REGISTER__EVALUATE_EXPR(TupleTerm_c)

// TEMP HACK
SEPT__REGISTER__INHABITS__NONDATA(ASTNPTerm, UnionTerm_c)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(UnionTerm_c, ASTNPTerm)
} // end namespace sept

int main (int argc, char **argv) {
    lvd::g_log.out().precision(std::numeric_limits<double>::max_digits10+1);
    lvd::g_log.out().setf(std::ios_base::boolalpha, std::ios_base::boolalpha);

    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(Any) << '\n'
               << LVD_REFLECT(BinOp) << '\n'
               << LVD_REFLECT(BinOpExpr) << '\n'
               << LVD_REFLECT(CondExpr) << '\n'
               << LVD_REFLECT(Expr) << '\n'
               << LVD_REFLECT(Logical) << '\n'
               << LVD_REFLECT(LogicalBinOp) << '\n'
               << LVD_REFLECT(LogicalBinOpExpr) << '\n'
               << LVD_REFLECT(LogicalCondExpr) << '\n'
               << LVD_REFLECT(LogicalExpr) << '\n'
               << LVD_REFLECT(LogicalParenExpr) << '\n'
               << LVD_REFLECT(LogicalUnOp) << '\n'
               << LVD_REFLECT(LogicalUnOpExpr) << '\n'
               << LVD_REFLECT(Numeric) << '\n'
               << LVD_REFLECT(NumericBinOp) << '\n'
               << LVD_REFLECT(NumericBinOpExpr) << '\n'
               << LVD_REFLECT(NumericCondExpr) << '\n'
               << LVD_REFLECT(NumericExpr) << '\n'
               << LVD_REFLECT(NumericParenExpr) << '\n'
               << LVD_REFLECT(NumericUnOp) << '\n'
               << LVD_REFLECT(NumericUnOpExpr) << '\n'
               << LVD_REFLECT(ParenExpr) << '\n'
               << LVD_REFLECT(UnOp) << '\n'
               << LVD_REFLECT(UnOpExpr) << '\n'
               << '\n';
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(inhabits(Add, LogicalBinOp)) << '\n'
               << LVD_REFLECT(inhabits(Sub, LogicalBinOp)) << '\n'
               << LVD_REFLECT(inhabits(Mul, LogicalBinOp)) << '\n'
               << LVD_REFLECT(inhabits(Div, LogicalBinOp)) << '\n'
               << LVD_REFLECT(inhabits(Pow, LogicalBinOp)) << '\n'
               << LVD_REFLECT(inhabits(Neg, LogicalBinOp)) << '\n'
               << LVD_REFLECT(inhabits(And, LogicalBinOp)) << '\n'
               << LVD_REFLECT(inhabits(Or, LogicalBinOp)) << '\n'
               << LVD_REFLECT(inhabits(Xor, LogicalBinOp)) << '\n'
               << LVD_REFLECT(inhabits(Not, LogicalBinOp)) << '\n'
               << '\n'
               << LVD_REFLECT(inhabits(Add, NumericBinOp)) << '\n'
               << LVD_REFLECT(inhabits(Sub, NumericBinOp)) << '\n'
               << LVD_REFLECT(inhabits(Mul, NumericBinOp)) << '\n'
               << LVD_REFLECT(inhabits(Div, NumericBinOp)) << '\n'
               << LVD_REFLECT(inhabits(Pow, NumericBinOp)) << '\n'
               << LVD_REFLECT(inhabits(Neg, NumericBinOp)) << '\n'
               << LVD_REFLECT(inhabits(And, NumericBinOp)) << '\n'
               << LVD_REFLECT(inhabits(Or, NumericBinOp)) << '\n'
               << LVD_REFLECT(inhabits(Xor, NumericBinOp)) << '\n'
               << LVD_REFLECT(inhabits(Not, NumericBinOp)) << '\n'
               << '\n'
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
               << LVD_REFLECT(inhabits(Add, LogicalUnOp)) << '\n'
               << LVD_REFLECT(inhabits(Sub, LogicalUnOp)) << '\n'
               << LVD_REFLECT(inhabits(Mul, LogicalUnOp)) << '\n'
               << LVD_REFLECT(inhabits(Div, LogicalUnOp)) << '\n'
               << LVD_REFLECT(inhabits(Pow, LogicalUnOp)) << '\n'
               << LVD_REFLECT(inhabits(Neg, LogicalUnOp)) << '\n'
               << LVD_REFLECT(inhabits(And, LogicalUnOp)) << '\n'
               << LVD_REFLECT(inhabits(Or, LogicalUnOp)) << '\n'
               << LVD_REFLECT(inhabits(Xor, LogicalUnOp)) << '\n'
               << LVD_REFLECT(inhabits(Not, LogicalUnOp)) << '\n'
               << '\n'
               << LVD_REFLECT(inhabits(Add, NumericUnOp)) << '\n'
               << LVD_REFLECT(inhabits(Sub, NumericUnOp)) << '\n'
               << LVD_REFLECT(inhabits(Mul, NumericUnOp)) << '\n'
               << LVD_REFLECT(inhabits(Div, NumericUnOp)) << '\n'
               << LVD_REFLECT(inhabits(Pow, NumericUnOp)) << '\n'
               << LVD_REFLECT(inhabits(Neg, NumericUnOp)) << '\n'
               << LVD_REFLECT(inhabits(And, NumericUnOp)) << '\n'
               << LVD_REFLECT(inhabits(Or, NumericUnOp)) << '\n'
               << LVD_REFLECT(inhabits(Xor, NumericUnOp)) << '\n'
               << LVD_REFLECT(inhabits(Not, NumericUnOp)) << '\n'
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
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(123.4, Pow, 2.0), NumericExpr)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(sept::TupleTerm_c(80.0, Add, 5.5), Pow, 2.0), NumericExpr)) << '\n'
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
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(sept::TupleTerm_c("hippo11", DeclaredAs, sept::Float64), MapsTo, sept::Bool), FuncPrototype)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(sept::TupleTerm_c(std::string("hippo12"), DeclaredAs, sept::Float64), MapsTo, sept::Bool), FuncPrototype)) << '\n'
               << '\n';
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(NumericValueTerminal(123.45)) << '\n'
               << LVD_REFLECT(NumericExpr(123.45)) << '\n'
               << LVD_REFLECT(NumericBinOpExpr(123.45, Add, 0.05)) << '\n'
               << LVD_REFLECT(BinOpExpr(123.45, Add, 0.05)) << '\n'
               << '\n';

    EvalCtx ctx;

    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(evaluate_expr_data(123.45, ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(true, ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(BinOpExpr(123.45, Add, 0.05), ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(CondExpr(true, 1.2, 0.05), ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(ParenExpr(1.2), ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(ParenExpr(ParenExpr(false)), ctx)) << '\n'
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
               << LVD_REFLECT(NumericBinOpExpr(x, Pow, 3.0)) << '\n'
               << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(x, Pow, 3.0), ctx)) << '\n'
               << '\n';
    // sin(x) = x - x^3 / 3! + x^5 / 5! - x^7 / 7! + x^9 / 9! - ...
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(x, Pow, 9.0), ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr(nine_factorial, ctx)) << '\n'
               << LVD_REFLECT(inhabits(NumericBinOpExpr(x, Pow, 9.0), Expr)) << '\n'
               << LVD_REFLECT(inhabits(Div, BinOp)) << '\n'
               << LVD_REFLECT(inhabits(nine_factorial, Expr)) << '\n'
               << LVD_REFLECT(sept::inhabits_data(NumericBinOpExpr(x, Pow, 9.0), Expr)) << '\n'
               << LVD_REFLECT(sept::inhabits_data(NumericBinOpExpr(x, Pow, 9.0), NumericExpr)) << '\n'
               << LVD_REFLECT(sept::inhabits_data(Div, BinOp)) << '\n'
               << LVD_REFLECT(sept::inhabits_data(nine_factorial, Expr)) << '\n'
               << LVD_REFLECT(inhabits(BinOpExpr(x, Pow, 9.0), BinOpExpr)) << '\n'
               << LVD_REFLECT(inhabits(BinOpExpr(x, Pow, 9.0), Expr)) << '\n'
               << LVD_REFLECT(inhabits(BinOpExpr(x, Pow, 9.0), LogicalBinOpExpr)) << '\n'
               << LVD_REFLECT(inhabits(BinOpExpr(x, Pow, 9.0), LogicalCondExpr)) << '\n'
               << LVD_REFLECT(inhabits(BinOpExpr(x, Pow, 9.0), LogicalExpr)) << '\n'
               << LVD_REFLECT(inhabits(BinOpExpr(x, Pow, 9.0), LogicalParenExpr)) << '\n'
               << LVD_REFLECT(inhabits(BinOpExpr(x, Pow, 9.0), LogicalUnOpExpr)) << '\n'
               << LVD_REFLECT(inhabits(BinOpExpr(x, Pow, 9.0), NumericBinOpExpr)) << '\n'
               << LVD_REFLECT(inhabits(BinOpExpr(x, Pow, 9.0), NumericCondExpr)) << '\n'
               << LVD_REFLECT(inhabits(BinOpExpr(x, Pow, 9.0), NumericExpr)) << '\n'
               << LVD_REFLECT(inhabits(BinOpExpr(x, Pow, 9.0), NumericParenExpr)) << '\n'
               << LVD_REFLECT(inhabits(BinOpExpr(x, Pow, 9.0), NumericUnOpExpr)) << '\n'
               << '\n';
    lvd::g_log << lvd::Log::dbg() << evaluate_expr(
        NumericBinOpExpr(
            NumericBinOpExpr(x, Pow, 9.0),
            Div,
            nine_factorial
        ),
        ctx
    ) << '\n';

    lvd::g_log << lvd::Log::dbg() << evaluate_expr(
        NumericBinOpExpr(
            NumericBinOpExpr(
                NumericBinOpExpr(
                    NumericBinOpExpr(
                        x,
                        Sub,
                        NumericBinOpExpr(
                            NumericBinOpExpr(x, Pow, 3.0),
                            Div,
                            three_factorial
                        )
                    ),
                    Add,
                    NumericBinOpExpr(
                        NumericBinOpExpr(x, Pow, 5.0),
                        Div,
                        five_factorial
                    )
                ),
                Sub,
                NumericBinOpExpr(
                    NumericBinOpExpr(x, Pow, 7.0),
                    Div,
                    seven_factorial
                )
            ),
            Add,
            NumericBinOpExpr(
                NumericBinOpExpr(x, Pow, 9.0),
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
                SymbolTypeDecl(SymbolId("x"), DeclaredAs, sept::Float64),
                MapsTo,
                sept::Float64
            ),
            BinOpExpr(SymbolId("x"), Mul, SymbolId("x"))
        )
    );
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(FuncEval(SymbolId("square"), ParenExpr(sept::Float64(100.1)))) << '\n'
               << LVD_REFLECT(evaluate_expr__as_FuncEval(FuncEval(SymbolId("square"), ParenExpr(sept::Float64(100.1))), ctx)) << '\n'
               << '\n';

    return 0;
}
