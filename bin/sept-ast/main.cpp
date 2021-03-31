#include <array>
#include <cmath>
#include <iostream>
#include <lvd/abort.hpp>
#include <lvd/call_site.hpp>
#include <lvd/comma.hpp>
#include <lvd/fmt.hpp>
#include <lvd/g_log.hpp>
#include "sept/ArrayTerm.hpp"
#include "sept/NPType.hpp"
#include "sept/TupleTerm.hpp"

#if 0
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
#endif

//
// StaticAssociation_t for evaluate
//

using EvaluateExprFunction = std::function<sept::Data(sept::Data const &expr)>;
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
        [](sept::Data const &expr_data) -> sept::Data { \
            auto const &expr = expr_data.cast<ExprType const &>(); \
            return evaluate_expr(expr); \
        } \
    )
#define SEPT__REGISTER__EVALUATE_EXPR(ExprType) \
    SEPT__REGISTER__EVALUATE_EXPR__GIVE_ID(ExprType, ExprType)

sept::Data evaluate_expr_data (sept::Data const &expr_data) {
    // Look up the type pair in the evaluator map.
    auto const &evaluator_map = lvd::static_association_singleton<EvaluateExpr>();
    auto it = evaluator_map.find(std::type_index(expr_data.type()));
    if (it == evaluator_map.end())
//         throw std::runtime_error(LVD_FMT("Data type " << expr_data.type().name() << " is not registered in EvaluateExpr for use in evaluate_expr_data"));
        // Evaluation is the identity.  TODO: Maybe don't do this, because it makes missing registrations harder to detect.
        return expr_data;

    return it->second(expr_data);
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

    __LOWEST__ = __Logical_LOWEST__,
    __HIGHEST__ = __Numeric_HIGHEST__,
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
struct BinOp_c { ABSTRACT_TYPE_CONSTRUCTOR };
struct Logical_c { ABSTRACT_TYPE_CONSTRUCTOR };
struct LogicalBinOp_c { ABSTRACT_TYPE_CONSTRUCTOR };
struct LogicalUnOp_c { ABSTRACT_TYPE_CONSTRUCTOR };
struct Numeric_c { ABSTRACT_TYPE_CONSTRUCTOR };
struct NumericBinOp_c { ABSTRACT_TYPE_CONSTRUCTOR };
struct NumericUnOp_c { ABSTRACT_TYPE_CONSTRUCTOR };
struct UnOp_c { ABSTRACT_TYPE_CONSTRUCTOR };
// TODO: Type intersections (LogicalExpr := Intersection(Expr,Logical)) and/or
// unions (Expr := Union(LogicalExpr,NumericExpr)) would be useful here.
struct Expr_c { ABSTRACT_TYPE_CONSTRUCTOR };
struct LogicalExpr_c { ABSTRACT_TYPE_CONSTRUCTOR };
struct NumericExpr_c { ABSTRACT_TYPE_CONSTRUCTOR };

std::ostream &operator<< (std::ostream &out, Any_c const &) { return out << "Any"; }
std::ostream &operator<< (std::ostream &out, BinOp_c const &) { return out << "BinOp"; }
std::ostream &operator<< (std::ostream &out, Logical_c const &) { return out << "Logical"; }
std::ostream &operator<< (std::ostream &out, LogicalBinOp_c const &) { return out << "LogicalBinOp"; }
std::ostream &operator<< (std::ostream &out, LogicalUnOp_c const &) { return out << "LogicalUnOp"; }
std::ostream &operator<< (std::ostream &out, Numeric_c const &) { return out << "Numeric"; }
std::ostream &operator<< (std::ostream &out, NumericBinOp_c const &) { return out << "NumericBinOp"; }
std::ostream &operator<< (std::ostream &out, NumericUnOp_c const &) { return out << "NumericUnOp"; }
std::ostream &operator<< (std::ostream &out, UnOp_c const &) { return out << "UnOp"; }

std::ostream &operator<< (std::ostream &out, Expr_c const &) { return out << "Expr"; }
std::ostream &operator<< (std::ostream &out, LogicalExpr_c const &) { return out << "LogicalExpr"; }
std::ostream &operator<< (std::ostream &out, NumericExpr_c const &) { return out << "NumericExpr"; }

inline sept::NonParametricType_c abstract_type_of (Any_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (BinOp_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (Logical_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (LogicalBinOp_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (LogicalUnOp_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (Numeric_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (NumericBinOp_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (NumericUnOp_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (UnOp_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (Expr_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (LogicalExpr_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (NumericExpr_c const &) { return sept::NonParametricType; }

Any_c const Any;

LogicalBinOp_c const LogicalBinOp;
LogicalUnOp_c const LogicalUnOp;
NumericBinOp_c const NumericBinOp;
NumericUnOp_c const NumericUnOp;

BinOp_c const BinOp;
UnOp_c const UnOp;

Logical_c const Logical;
Numeric_c const Numeric;

Expr_c const Expr;
LogicalExpr_c const LogicalExpr;
NumericExpr_c const NumericExpr;

// These are effectively structural subtypes (indistinguishable from sept::Bool and sept::Float64 respectively)
sept::Bool_c const LogicalValue;
sept::Float64_c const NumericValue;
// TODO: Could define variables for each as well

// These are effectively structural subtypes (indistinguishable from the specific tuple terms)
sept::TupleTerm_c const BinOpExpr{Expr, BinOp, Expr};
sept::TupleTerm_c const UnOpExpr{Expr, UnOp, Expr};
sept::TupleTerm_c const CondExpr{LogicalExpr, Expr, Expr};
sept::TupleTerm_c const ParenExpr{Expr};

sept::TupleTerm_c const LogicalBinOpExpr{LogicalExpr, LogicalBinOp, LogicalExpr};
sept::TupleTerm_c const NumericBinOpExpr{NumericExpr, NumericBinOp, NumericExpr};

sept::TupleTerm_c const LogicalUnOpExpr{LogicalUnOp, LogicalExpr};
sept::TupleTerm_c const NumericUnOpExpr{NumericUnOp, NumericExpr};

sept::TupleTerm_c const LogicalCondExpr{LogicalExpr, LogicalExpr, LogicalExpr};
sept::TupleTerm_c const NumericCondExpr{LogicalExpr, NumericExpr, NumericExpr};

sept::TupleTerm_c const LogicalParenExpr{LogicalExpr};
sept::TupleTerm_c const NumericParenExpr{NumericExpr};

//
// Some forward declarations needed to keep inhabits functions in alphabetical order of type (second argument)
//

bool constexpr inhabits (ASTNPTerm t, LogicalBinOp_c const &);
bool inhabits (sept::TupleTerm_c const &t, LogicalExpr_c const &);
bool constexpr inhabits (ASTNPTerm t, LogicalUnOp_c const &);
bool constexpr inhabits (ASTNPTerm t, NumericBinOp_c const &);
bool inhabits (sept::TupleTerm_c const &t, NumericExpr_c const &);
bool constexpr inhabits (ASTNPTerm t, NumericUnOp_c const &);

//
// Definitions of inhabits
//

inline sept::True_c constexpr inhabits (ASTNPTerm t, Any_c const &) { return sept::True; }

inline bool constexpr inhabits (ASTNPTerm t, BinOp_c const &) {
    return inhabits(t, LogicalBinOp) || inhabits(t, NumericBinOp);
}


inline sept::False_c constexpr inhabits (ASTNPTerm t, Expr_c const &) { return sept::False; }
inline sept::True_c constexpr inhabits (bool const &, Expr_c const &) { return sept::True; }
inline sept::True_c constexpr inhabits (double const &, Expr_c const &) { return sept::True; }
inline bool inhabits (sept::TupleTerm_c const &t, Expr_c const &) {
    return inhabits(t, BinOpExpr)
        || inhabits(t, UnOpExpr)
        || inhabits(t, CondExpr)
        || inhabits(t, ParenExpr);
}


inline bool constexpr inhabits (ASTNPTerm t, Logical_c const &) {
    return inhabits(t, LogicalBinOp) || inhabits(t, LogicalUnOp);
}
inline sept::True_c constexpr inhabits (bool const &, Logical_c const &) { return sept::True; }
inline sept::False_c constexpr inhabits (double const &, Logical_c const &) { return sept::False; }
inline bool inhabits (sept::TupleTerm_c const &t, Logical_c const &) { return inhabits(t, LogicalExpr); }

inline bool constexpr inhabits (ASTNPTerm t, LogicalBinOp_c const &) {
    return ASTNPTermRepr(ASTNPTerm::__LogicalBinOp_LOWEST__) <= ASTNPTermRepr(t)
        && ASTNPTermRepr(t) <= ASTNPTermRepr(ASTNPTerm::__LogicalBinOp_HIGHEST__);
}

inline sept::True_c constexpr inhabits (bool const &, LogicalExpr_c const &) { return sept::True; }
inline sept::False_c constexpr inhabits (double const &, LogicalExpr_c const &) { return sept::False; }
inline bool inhabits (sept::TupleTerm_c const &t, LogicalExpr_c const &) {
    return inhabits(t, LogicalBinOpExpr)
        || inhabits(t, LogicalUnOpExpr)
        || inhabits(t, LogicalCondExpr)
        || inhabits(t, LogicalParenExpr);
}

inline bool constexpr inhabits (ASTNPTerm t, LogicalUnOp_c const &) {
    return ASTNPTermRepr(ASTNPTerm::__LogicalUnOp_LOWEST__) <= ASTNPTermRepr(t)
        && ASTNPTermRepr(t) <= ASTNPTermRepr(ASTNPTerm::__LogicalUnOp_HIGHEST__);
}

inline bool constexpr inhabits (ASTNPTerm t, Numeric_c const &) {
    return inhabits(t, NumericBinOp) || inhabits(t, NumericUnOp);
}
inline sept::False_c constexpr inhabits (bool const &, Numeric_c const &) { return sept::False; }
inline sept::True_c constexpr inhabits (double const &, Numeric_c const &) { return sept::True; }
inline bool inhabits (sept::TupleTerm_c const &t, Numeric_c const &) { return inhabits(t, NumericExpr); }

inline bool constexpr inhabits (ASTNPTerm t, NumericBinOp_c const &) {
    return ASTNPTermRepr(ASTNPTerm::__NumericBinOp_LOWEST__) <= ASTNPTermRepr(t)
        && ASTNPTermRepr(t) <= ASTNPTermRepr(ASTNPTerm::__NumericBinOp_HIGHEST__);
}
inline bool constexpr inhabits (ASTNPTerm t, NumericUnOp_c const &) {
    return ASTNPTermRepr(ASTNPTerm::__NumericUnOp_LOWEST__) <= ASTNPTermRepr(t)
        && ASTNPTermRepr(t) <= ASTNPTermRepr(ASTNPTerm::__NumericUnOp_HIGHEST__);
}

inline sept::False_c constexpr inhabits (bool const &, NumericExpr_c const &) { return sept::False; }
inline sept::True_c constexpr inhabits (double const &, NumericExpr_c const &) { return sept::True; }
inline bool inhabits (sept::TupleTerm_c const &t, NumericExpr_c const &) {
    return inhabits(t, NumericBinOpExpr)
        || inhabits(t, NumericUnOpExpr)
        || inhabits(t, NumericCondExpr)
        || inhabits(t, NumericParenExpr);
}

inline bool constexpr inhabits (ASTNPTerm t, UnOp_c const &) {
    return inhabits(t, LogicalUnOp) || inhabits(t, NumericUnOp);
}

//
// evaluate
//

bool evaluate_expr (bool const &expr) { return expr; }
// bool evaluate_expr (sept::BoolTerm_c const &expr) { return expr; }
sept::BoolTerm_c evaluate_expr (sept::BoolTerm_c const &expr) { return expr; }
double evaluate_expr (double const &expr) { return expr; }

template <typename T_>
T_ evaluate_expr__as_BinOpExpr (sept::TupleTerm_c const &t) {
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

    auto lhs = evaluate_expr_data(t[0]).cast<T_>();
    auto bin_op = t[1].cast<ASTNPTerm>();
    auto rhs = evaluate_expr_data(t[2]).cast<T_>();
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
T_ evaluate_expr__as_UnOpExpr (sept::TupleTerm_c const &t) {
    if (!inhabits(t, UnOpExpr)) {
        throw std::runtime_error(LVD_FMT("failed condition: inhabits(t, UnOpExpr), where t = " << t << " and UnOpExpr = " << UnOpExpr));
    }
    assert(inhabits(t, UnOpExpr));

    auto un_op = t[0].cast<ASTNPTerm>();
    auto operand = evaluate_expr_data(t[1]).cast<T_>();
    switch (un_op) {
        case ASTNPTerm::NOT: return !operand;
        case ASTNPTerm::NEG: return -operand;
        default: LVD_ABORT(LVD_FMT("invalid ASTNPTerm for use as an UnOp: " << uint32_t(un_op)));
    }
}

template <typename T_>
T_ evaluate_expr__as_CondExpr (sept::TupleTerm_c const &t) {
    assert(inhabits(t, CondExpr));

    auto condition = evaluate_expr_data(t[0]).cast<bool>();
    return evaluate_expr_data(t[condition ? 1 : 2]).cast<T_>();
}

template <typename T_>
T_ evaluate_expr__as_ParenExpr (sept::TupleTerm_c const &t) {
    assert(inhabits(t, ParenExpr));
    return evaluate_expr_data(t[0]).cast<T_>();
}

sept::Data evaluate_expr (sept::TupleTerm_c const &t) {
    // TODO: Make this efficient -- some sort of poset search.
    if (inhabits(t, LogicalBinOpExpr))
        return evaluate_expr__as_BinOpExpr<bool>(t);
    else if (inhabits(t, LogicalCondExpr))
        return evaluate_expr__as_CondExpr<bool>(t);
    else if (inhabits(t, LogicalParenExpr))
        return evaluate_expr__as_ParenExpr<bool>(t);
    else if (inhabits(t, LogicalUnOpExpr))
        return evaluate_expr__as_UnOpExpr<bool>(t);
    else if (inhabits(t, NumericBinOpExpr))
        return evaluate_expr__as_BinOpExpr<double>(t);
    else if (inhabits(t, NumericCondExpr))
        return evaluate_expr__as_CondExpr<double>(t);
    else if (inhabits(t, NumericParenExpr))
        return evaluate_expr__as_ParenExpr<double>(t);
    else if (inhabits(t, NumericUnOpExpr))
        return evaluate_expr__as_UnOpExpr<double>(t);
    else
        LVD_ABORT(LVD_FMT("unrecognized Expr: " << t));
}

//
// sept data model registrations
//

namespace sept {
SEPT__REGISTER__PRINT(ASTNPTerm)
SEPT__REGISTER__PRINT(Any_c)
SEPT__REGISTER__PRINT(BinOp_c)
SEPT__REGISTER__PRINT(Expr_c)
SEPT__REGISTER__PRINT(Logical_c)
SEPT__REGISTER__PRINT(LogicalBinOp_c)
SEPT__REGISTER__PRINT(LogicalExpr_c)
SEPT__REGISTER__PRINT(LogicalUnOp_c)
SEPT__REGISTER__PRINT(Numeric_c)
SEPT__REGISTER__PRINT(NumericBinOp_c)
SEPT__REGISTER__PRINT(NumericUnOp_c)
SEPT__REGISTER__PRINT(NumericExpr_c)
SEPT__REGISTER__PRINT(UnOp_c)

SEPT__REGISTER__EQ(Any_c)
SEPT__REGISTER__EQ(BinOp_c)
SEPT__REGISTER__EQ(Expr_c)
SEPT__REGISTER__EQ(Logical_c)
SEPT__REGISTER__EQ(LogicalBinOp_c)
SEPT__REGISTER__EQ(LogicalExpr_c)
SEPT__REGISTER__EQ(LogicalUnOp_c)
SEPT__REGISTER__EQ(Numeric_c)
SEPT__REGISTER__EQ(NumericBinOp_c)
SEPT__REGISTER__EQ(NumericUnOp_c)
SEPT__REGISTER__EQ(NumericExpr_c)
SEPT__REGISTER__EQ(UnOp_c)

SEPT__REGISTER__ABSTRACT_TYPE_OF(Any_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(BinOp_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(Expr_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(Logical_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(LogicalBinOp_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(LogicalExpr_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(LogicalUnOp_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(Numeric_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(NumericBinOp_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(NumericUnOp_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(NumericExpr_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(UnOp_c)

SEPT__REGISTER__INHABITS__EVALUATOR_BODY(ASTNPTerm,     BinOp_c,        return inhabits(value, type);)
SEPT__REGISTER__INHABITS(bool,          Expr_c)
SEPT__REGISTER__INHABITS(double,        Expr_c)
SEPT__REGISTER__INHABITS__EVALUATOR_BODY(TupleTerm_c,   Expr_c,         return inhabits(value, type);)
SEPT__REGISTER__INHABITS(bool,          LogicalExpr_c)
SEPT__REGISTER__INHABITS__EVALUATOR_BODY(TupleTerm_c,   LogicalExpr_c,  return inhabits(value, type);)
SEPT__REGISTER__INHABITS(double,        NumericExpr_c)
SEPT__REGISTER__INHABITS__EVALUATOR_BODY(TupleTerm_c,   NumericExpr_c,  return inhabits(value, type);)
SEPT__REGISTER__INHABITS__EVALUATOR_BODY(ASTNPTerm,     LogicalBinOp_c, return inhabits(value, type);)
SEPT__REGISTER__INHABITS__EVALUATOR_BODY(ASTNPTerm,     LogicalUnOp_c,  return inhabits(value, type);)
SEPT__REGISTER__INHABITS__EVALUATOR_BODY(ASTNPTerm,     NumericBinOp_c, return inhabits(value, type);)
SEPT__REGISTER__INHABITS__EVALUATOR_BODY(ASTNPTerm,     NumericUnOp_c,  return inhabits(value, type);)
SEPT__REGISTER__INHABITS__EVALUATOR_BODY(ASTNPTerm,     UnOp_c,         return inhabits(value, type);)
// TODO there are probably some missing

SEPT__REGISTER__COMPARE__SINGLETON(Any_c)
SEPT__REGISTER__COMPARE__SINGLETON(BinOp_c)
SEPT__REGISTER__COMPARE__SINGLETON(Expr_c)
SEPT__REGISTER__COMPARE__SINGLETON(Logical_c)
SEPT__REGISTER__COMPARE__SINGLETON(LogicalBinOp_c)
SEPT__REGISTER__COMPARE__SINGLETON(LogicalExpr_c)
SEPT__REGISTER__COMPARE__SINGLETON(LogicalUnOp_c)
SEPT__REGISTER__COMPARE__SINGLETON(Numeric_c)
SEPT__REGISTER__COMPARE__SINGLETON(NumericBinOp_c)
SEPT__REGISTER__COMPARE__SINGLETON(NumericUnOp_c)
SEPT__REGISTER__COMPARE__SINGLETON(NumericExpr_c)
SEPT__REGISTER__COMPARE__SINGLETON(UnOp_c)
SEPT__REGISTER__COMPARE(ASTNPTerm, ASTNPTerm)

SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(BinOp_c,             ASTNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(Logical_c,           ASTNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(LogicalBinOp_c,      ASTNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(LogicalUnOp_c,       ASTNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(Numeric_c,           ASTNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(NumericBinOp_c,      ASTNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(NumericUnOp_c,       ASTNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(UnOp_c,              ASTNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(Expr_c,              bool)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(LogicalExpr_c,       bool)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(Expr_c,              double)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(NumericExpr_c,       double)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(LogicalExpr_c,       TupleTerm_c)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(NumericExpr_c,       TupleTerm_c)

SEPT__REGISTER__EVALUATE_EXPR(bool)
SEPT__REGISTER__EVALUATE_EXPR(double)
SEPT__REGISTER__EVALUATE_EXPR(TupleTerm_c)
} // end namespace sept

int main (int argc, char **argv) {
    lvd::g_log.out() << std::boolalpha;

    lvd::g_log << lvd::Log::trc()
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
    lvd::g_log << lvd::Log::trc()
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
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(123.4, Pow, 2.0), Expr)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(123.4, Pow, 2.0), NumericExpr)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(sept::TupleTerm_c(80.0, Add, 5.5), Pow, 2.0), NumericExpr)) << '\n'
               << '\n';
    lvd::g_log << lvd::Log::trc()
               << LVD_REFLECT(NumericValue(123.45)) << '\n'
               << LVD_REFLECT(NumericExpr(123.45)) << '\n'
               << LVD_REFLECT(NumericBinOpExpr(123.45, Add, 0.05)) << '\n'
               << LVD_REFLECT(BinOpExpr(123.45, Add, 0.05)) << '\n'
               << '\n';
    lvd::g_log << lvd::Log::trc()
               << LVD_REFLECT(evaluate_expr_data(123.45)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(true)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(BinOpExpr(123.45, Add, 0.05))) << '\n'
               << LVD_REFLECT(evaluate_expr_data(CondExpr(true, 1.2, 0.05))) << '\n'
               << LVD_REFLECT(evaluate_expr_data(ParenExpr(1.2))) << '\n'
               << LVD_REFLECT(evaluate_expr_data(ParenExpr(ParenExpr(false)))) << '\n'
               << '\n';

    // Silly, but hey.
    auto x = 0.1;
    auto three_factorial = 3.0*2.0*1.0;
    auto five_factorial = 5.0*4.0*three_factorial;
    auto seven_factorial = 7.0*6.0*five_factorial;
    auto nine_factorial = 9.0*8.0*seven_factorial;
    lvd::g_log << lvd::Log::trc()
               << LVD_REFLECT(x) << '\n'
               << LVD_REFLECT(three_factorial) << '\n'
               << LVD_REFLECT(five_factorial) << '\n'
               << LVD_REFLECT(seven_factorial) << '\n'
               << LVD_REFLECT(nine_factorial) << '\n'
               << LVD_REFLECT(NumericBinOpExpr(x, Pow, 3.0)) << '\n'
               << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(x, Pow, 3.0))) << '\n'
               << '\n';
    // sin(x) = x - x^3 / 3! + x^5 / 5! - x^7 / 7! + x^9 / 9! - ...
    lvd::g_log << lvd::Log::trc()
               << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(x, Pow, 9.0))) << '\n'
               << LVD_REFLECT(evaluate_expr(nine_factorial)) << '\n'
               << LVD_REFLECT(inhabits(NumericBinOpExpr(x, Pow, 9.0), Expr)) << '\n'
               << LVD_REFLECT(inhabits(Div, BinOp)) << '\n'
               << LVD_REFLECT(inhabits(nine_factorial, Expr)) << '\n'
               << LVD_REFLECT(sept::inhabits_data(NumericBinOpExpr(x, Pow, 9.0), Expr)) << '\n'
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
    lvd::g_log << lvd::Log::trc() << evaluate_expr(
        NumericBinOpExpr(
            NumericBinOpExpr(x, Pow, 9.0),
            Div,
            nine_factorial
        )
    ) << '\n';
//     lvd::g_log << lvd::Log::trc()
//         << LVD_REFLECT(
//             inhabits(
//                 NumericBinOpExpr(
//                     NumericBinOpExpr(
//                         NumericBinOpExpr(
//                             NumericBinOpExpr(
//                                 x,
//                                 Sub,
//                                 NumericBinOpExpr(
//                                     NumericBinOpExpr(x, Pow, 3.0),
//                                     Div,
//                                     three_factorial
//                                 )
//                             ),
//                             Add,
//                             NumericBinOpExpr(
//                                 NumericBinOpExpr(x, Pow, 5.0),
//                                 Div,
//                                 five_factorial
//                             )
//                         ),
//                         Sub,
//                         NumericBinOpExpr(
//                             NumericBinOpExpr(x, Pow, 7.0),
//                             Div,
//                             seven_factorial
//                         )
//                     ),
//                     Add,
//                     NumericBinOpExpr(
//                         NumericBinOpExpr(x, Pow, 9.0),
//                         Div,
//                         nine_factorial
//                     )
//                 ),
//                 NumericBinOpExpr
//             )
//         ) << '\n'
//         << LVD_REFLECT(
//             inhabits(
//                 NumericBinOpExpr(
//                     NumericBinOpExpr(x, Pow, 9.0),
//                     Div,
//                     nine_factorial
//                 ),
//                 NumericBinOpExpr
//             )
//         ) << '\n'
//         << LVD_REFLECT(
//             inhabits(
//                 NumericBinOpExpr(
//                     NumericBinOpExpr(
//                         NumericBinOpExpr(
//                             x,
//                             Sub,
//                             NumericBinOpExpr(
//                                 NumericBinOpExpr(x, Pow, 3.0),
//                                 Div,
//                                 three_factorial
//                             )
//                         ),
//                         Add,
//                         NumericBinOpExpr(
//                             NumericBinOpExpr(x, Pow, 5.0),
//                             Div,
//                             five_factorial
//                         )
//                     ),
//                     Sub,
//                     NumericBinOpExpr(
//                         NumericBinOpExpr(x, Pow, 7.0),
//                         Div,
//                         seven_factorial
//                     )
//                 ),
//                 NumericBinOpExpr
//             )
//         ) << '\n'
//         << '\n';

    lvd::g_log << lvd::Log::trc() << evaluate_expr(
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
        )
    ) << '\n';
    lvd::g_log << lvd::Log::trc() << LVD_REFLECT(std::sin(x)) << '\n';

    return 0;
}
