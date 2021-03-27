#include <iostream>
#include <lvd/abort.hpp>
#include <lvd/comma.hpp>
#include <lvd/fmt.hpp>
#include "sept/ArrayTerm.hpp"
#include "sept/NPType.hpp"

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
    -   LogicalParenExpr := Tuple(LogicalExpr) // TODO: There really needs to be a semantic type "tag" to distinguish it
    -   LogicalCondExpr := Tuple(LogicalExpr, LogicalExpr, LogicalExpr) // (condition, positive_expr, negative_expr) // TODO: There really needs to be a semantic type "tag" to distinguish it
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
    -   NumericCondExpr := Tuple(LogicalExpr, NumericExpr, NumericExpr) // (condition, positive_expr, negative_expr)
    -   NumericExpr := Union(Float64, NumericBinOpExpr, NumericUnOpExpr, NumericParenExpr)
#endif

//
// LogicalBinOpTerm_c
//

using LogicalBinOpTermRepr = uint8_t;
enum class LogicalBinOpTerm_c : LogicalBinOpTermRepr {
    AND = 0,
    OR,
    XOR,

    __LOWEST__ = AND,
    __HIGHEST__ = XOR,
};

std::string const &as_string (LogicalBinOpTerm_c t) {
    static std::vector<std::string> const TABLE{
        "And",
        "Or",
        "Xor",
    };
    return TABLE.at(size_t(t));
}

std::ostream &operator<< (std::ostream &out, LogicalBinOpTerm_c const &t) {
    return out << as_string(t);
}

//
// LogicalBinOp_c
//

class LogicalBinOp_c {
public:

    // TODO: Another way this could work is that it applies the abstract type LogicalBinOp to the concrete value returned.
    LogicalBinOpTerm_c operator() (LogicalBinOpTermRepr v) const {
        if (v > LogicalBinOpTermRepr(LogicalBinOpTerm_c::__HIGHEST__))
            throw std::out_of_range("invalid value for LogicalBinOpTerm_c");
        return LogicalBinOpTerm_c{v};
    }
};

std::ostream &operator<< (std::ostream &out, LogicalBinOp_c const &) {
    return out << "LogicalBinOp";
}

//
// LogicalBinOpExpr_c
//

class LogicalBinOpExpr_c {
public:

    // TODO: Template types, use non-Data version of inhabits
    // TODO: Another way this could work is that it applies the abstract type LogicalBinOpExpr to the concrete value returned.
    sept::ArrayTerm_c operator() (sept::Data &&lhs, sept::Data &&bin_op, sept::Data &&rhs) const {
        auto retval = sept::Array(std::move(lhs), std::move(bin_op), std::move(rhs));
        if (!inhabits_data(retval, LogicalBinOpExpr_c{}))
            throw std::runtime_error(LVD_FMT("ill-formed LogicalBinOpExpr: " << retval));
        return retval;
    }
};

std::ostream &operator<< (std::ostream &out, LogicalBinOpExpr_c const &) {
    return out << "LogicalBinOpExpr";
}

//
// LogicalExpr_c
//

class LogicalExpr_c {
public:

    // Really all this does is verify that expr:LogicalExpr
    // TODO: Template types, use non-Data version of inhabits
    // TODO: Another way this could work is that it applies the abstract type LogicalExpr to the concrete value returned.
    sept::Data operator() (sept::Data &&expr) const {
        if (!inhabits_data(expr, LogicalExpr_c{}))
            throw std::runtime_error(LVD_FMT("ill-formed LogicalExpr: " << expr));
        return expr;
    }
};

std::ostream &operator<< (std::ostream &out, LogicalExpr_c const &) {
    return out << "LogicalExpr";
}

//
// LogicalUnOpTerm_c
//

using LogicalUnOpTermRepr = uint8_t;
enum class LogicalUnOpTerm_c : LogicalUnOpTermRepr {
    NOT = 0,

    __LOWEST__ = NOT,
    __HIGHEST__ = NOT,
};

std::string const &as_string (LogicalUnOpTerm_c t) {
    static std::vector<std::string> const TABLE{
        "Not",
    };
    return TABLE.at(size_t(t));
}

std::ostream &operator<< (std::ostream &out, LogicalUnOpTerm_c const &t) {
    return out << as_string(t);
}

//
// LogicalUnOp_c
//

class LogicalUnOp_c {
public:

    // TODO: Another way this could work is that it applies the abstract type LogicalUnOp to the concrete value returned.
    LogicalUnOpTerm_c operator() (LogicalUnOpTermRepr v) const {
        if (v > LogicalUnOpTermRepr(LogicalUnOpTerm_c::__HIGHEST__))
            throw std::out_of_range("invalid value for LogicalUnOpTerm_c");
        return LogicalUnOpTerm_c{v};
    }
};

std::ostream &operator<< (std::ostream &out, LogicalUnOp_c const &) {
    return out << "LogicalUnOp";
}

//
// LogicalUnOpExpr_c
//

class LogicalUnOpExpr_c {
public:

    // TODO: Template types, use non-Data version of inhabits
    sept::ArrayTerm_c operator() (sept::Data &&un_op, sept::Data &&operand) const {
        auto retval = sept::Array(std::move(un_op), std::move(operand));
        if (!inhabits_data(retval, LogicalUnOpExpr_c{}))
            throw std::runtime_error(LVD_FMT("ill-formed LogicalUnOpExpr: " << retval));
        return retval;
    }
};

std::ostream &operator<< (std::ostream &out, LogicalUnOpExpr_c const &) {
    return out << "LogicalUnOpExpr";
}

//
// Declarations/definitions
//

LogicalBinOp_c const LogicalBinOp;
LogicalBinOpExpr_c const LogicalBinOpExpr;
LogicalExpr_c const LogicalExpr;
LogicalUnOp_c const LogicalUnOp;
LogicalUnOpExpr_c const LogicalUnOpExpr;

LogicalBinOpTerm_c const And{LogicalBinOpTerm_c::AND};
LogicalBinOpTerm_c const Or{LogicalBinOpTerm_c::OR};
LogicalBinOpTerm_c const Xor{LogicalBinOpTerm_c::XOR};

LogicalUnOpTerm_c const Not{LogicalUnOpTerm_c::NOT};

inline constexpr LogicalBinOp_c const &abstract_type_of (LogicalBinOpTerm_c const &) { return LogicalBinOp; }
inline constexpr LogicalUnOp_c const &abstract_type_of (LogicalUnOpTerm_c const &) { return LogicalUnOp; }
inline constexpr sept::NonParametricType_c const &abstract_type_of (LogicalBinOp_c const &) { return sept::NonParametricType; }
inline constexpr sept::NonParametricType_c const &abstract_type_of (LogicalBinOpExpr_c const &) { return sept::NonParametricType; }
inline constexpr sept::NonParametricType_c const &abstract_type_of (LogicalExpr_c const &) { return sept::NonParametricType; }
inline constexpr sept::NonParametricType_c const &abstract_type_of (LogicalUnOp_c const &) { return sept::NonParametricType; }
inline constexpr sept::NonParametricType_c const &abstract_type_of (LogicalUnOpExpr_c const &) { return sept::NonParametricType; }

// TEMP HACK -- these probably don't belong in the sept namespace
// namespace sept {

int compare (uint8_t lhs, uint8_t rhs) {
    return int(lhs) - int(rhs);
}

int compare (LogicalBinOpTerm_c const &lhs, LogicalBinOpTerm_c const &rhs) {
    return compare(LogicalBinOpTermRepr(lhs), LogicalBinOpTermRepr(rhs));
}

int compare (LogicalUnOpTerm_c const &lhs, LogicalUnOpTerm_c const &rhs) {
    return compare(LogicalUnOpTermRepr(lhs), LogicalUnOpTermRepr(rhs));
}

bool inhabits (sept::ArrayTerm_c const &value, LogicalBinOpExpr_c const &type) {
    return value.size() == 3
        && inhabits_data(value[0], LogicalExpr)
        && inhabits_data(value[1], LogicalBinOp)
        && inhabits_data(value[2], LogicalExpr);
}

bool inhabits (sept::ArrayTerm_c const &value, LogicalUnOpExpr_c const &type) {
    return value.size() == 2
        && inhabits_data(value[0], LogicalUnOp)
        && inhabits_data(value[1], LogicalExpr);
}

bool inhabits (sept::ArrayTerm_c const &value, LogicalExpr_c const &type) {
    return inhabits_data(value, sept::Bool)
        || inhabits_data(value, LogicalBinOpExpr)
//         || inhabits_data(value, LogicalParenExpr)
        || inhabits_data(value, LogicalUnOpExpr);
}

// } // end namespace sept

// NOTE: This is sorta hacky, though it work.  TODO: come up with a better way
namespace sept {
SEPT__REGISTER__PRINT(LogicalBinOp_c)
SEPT__REGISTER__PRINT(LogicalBinOpExpr_c)
SEPT__REGISTER__PRINT(LogicalBinOpTerm_c)
SEPT__REGISTER__PRINT(LogicalExpr_c)
SEPT__REGISTER__PRINT(LogicalUnOp_c)
SEPT__REGISTER__PRINT(LogicalUnOpExpr_c)
SEPT__REGISTER__PRINT(LogicalUnOpTerm_c)

SEPT__REGISTER__EQ(LogicalBinOp_c)
SEPT__REGISTER__EQ(LogicalBinOpExpr_c)
SEPT__REGISTER__EQ(LogicalBinOpTerm_c)
SEPT__REGISTER__EQ(LogicalExpr_c)
SEPT__REGISTER__EQ(LogicalUnOp_c)
SEPT__REGISTER__EQ(LogicalUnOpExpr_c)
SEPT__REGISTER__EQ(LogicalUnOpTerm_c)

SEPT__REGISTER__ABSTRACT_TYPE_OF(LogicalBinOp_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(LogicalBinOpExpr_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(LogicalBinOpTerm_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(LogicalExpr_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(LogicalUnOp_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(LogicalUnOpExpr_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(LogicalUnOpTerm_c)

SEPT__REGISTER__INHABITS(LogicalBinOp_c, NonParametricType_c)
SEPT__REGISTER__INHABITS(LogicalBinOpExpr_c, NonParametricType_c)
SEPT__REGISTER__INHABITS(LogicalBinOpTerm_c, LogicalBinOp_c)
SEPT__REGISTER__INHABITS(LogicalExpr_c, NonParametricType_c)
// TODO: Could just use `sept::Tuple_c const LogicalBinOpExpr = sept::Tuple(LogicalExpr, LogicalBinOp, LogicalExpr);`
SEPT__REGISTER__INHABITS__EVALUATOR_BODY(ArrayTerm_c, LogicalBinOpExpr_c, return inhabits(value, type);)
// TODO: Could just use `sept::Tuple_c const LogicalUnOpExpr = sept::Tuple(LogicalBinOp, LogicalExpr);`
SEPT__REGISTER__INHABITS__EVALUATOR_BODY(ArrayTerm_c, LogicalUnOpExpr_c, return inhabits(value, type);)
SEPT__REGISTER__INHABITS(bool, LogicalExpr_c)
SEPT__REGISTER__INHABITS(True_c, LogicalExpr_c)
SEPT__REGISTER__INHABITS(False_c, LogicalExpr_c)
SEPT__REGISTER__INHABITS(BoolTerm_c, LogicalExpr_c)
// TODO: Could just use `sept::Union_c const LogicalExpr = sept::Union(Bool, LogicalBinOpExpr, LogicalParenExpr, LogicalUnOpExpr);`
SEPT__REGISTER__INHABITS__EVALUATOR_BODY(ArrayTerm_c, LogicalExpr_c, return inhabits(value, type);)
SEPT__REGISTER__INHABITS(LogicalUnOp_c, NonParametricType_c)
SEPT__REGISTER__INHABITS(LogicalUnOpExpr_c, NonParametricType_c)
SEPT__REGISTER__INHABITS(LogicalUnOpTerm_c, LogicalUnOp_c)

SEPT__REGISTER__COMPARE__SINGLETON(LogicalBinOp_c)
SEPT__REGISTER__COMPARE__SINGLETON(LogicalBinOpExpr_c)
SEPT__REGISTER__COMPARE(LogicalBinOpTerm_c, LogicalBinOpTerm_c)
SEPT__REGISTER__COMPARE__SINGLETON(LogicalExpr_c)
SEPT__REGISTER__COMPARE__SINGLETON(LogicalUnOp_c)
SEPT__REGISTER__COMPARE__SINGLETON(LogicalUnOpExpr_c)
SEPT__REGISTER__COMPARE(LogicalUnOpTerm_c, LogicalUnOpTerm_c)
} // end namespace sept

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

bool evaluate_expr__as_LogicalBinOpExpr (sept::ArrayTerm_c const &a) {
    assert(inhabits(a, LogicalBinOpExpr));

    // This is silly -- TODO: Come up with better conversion to bool
    bool lhs = evaluate_expr_data(a[0]) == sept::True || evaluate_expr_data(a[0]) == sept::Data{true};
    bool rhs = evaluate_expr_data(a[2]) == sept::True || evaluate_expr_data(a[2]) == sept::Data{true};
    switch (a[1].cast<LogicalBinOpTerm_c const &>()) {
        case LogicalBinOpTerm_c::AND: return lhs && rhs;
        case LogicalBinOpTerm_c::OR:  return lhs || rhs;
        case LogicalBinOpTerm_c::XOR: return lhs != rhs;
        default: LVD_ABORT("invalid LogicalBinOpTerm_c");
    }
}

bool evaluate_expr__as_LogicalUnOpExpr (sept::ArrayTerm_c const &a) {
    assert(inhabits(a, LogicalUnOpExpr));

    // This is silly -- TODO: Come up with better conversion to bool
    bool operand = evaluate_expr_data(a[1]) == sept::True || evaluate_expr_data(a[1]) == sept::Data{true};
    switch (a[0].cast<LogicalUnOpTerm_c const &>()) {
        case LogicalUnOpTerm_c::NOT: return !operand;
        default: LVD_ABORT("invalid LogicalUnOpTerm_c");
    }
}

bool evaluate_expr (bool const &b) { return b; }

bool evaluate_expr (sept::BoolTerm_c const &b) { return b; }

bool evaluate_expr (sept::ArrayTerm_c const &a) {
    // This is a bit silly, but hey.
    if (inhabits(a, LogicalBinOpExpr))
        return evaluate_expr__as_LogicalBinOpExpr(a);
    else if (inhabits(a, LogicalUnOpExpr))
        return evaluate_expr__as_LogicalUnOpExpr(a);
    else
        LVD_ABORT(LVD_FMT("invalid LogicalExpr Array: " << a));
}

SEPT__REGISTER__EVALUATE_EXPR(bool)
namespace sept {
SEPT__REGISTER__EVALUATE_EXPR(BoolTerm_c)
SEPT__REGISTER__EVALUATE_EXPR(ArrayTerm_c)
} // end namespace sept

int main (int argc, char **argv) {
    std::cout << std::boolalpha;

    std::cout
        << LVD_REFLECT(And) << '\n'
        << LVD_REFLECT(Or) << '\n'
        << LVD_REFLECT(Xor) << '\n'
        << LVD_REFLECT(Not) << '\n'
        << LVD_REFLECT(LogicalBinOp) << '\n'
        << LVD_REFLECT(LogicalUnOp) << '\n'
        << '\n';
    std::cout
        << LVD_REFLECT(abstract_type_of(And)) << '\n'
        << LVD_REFLECT(abstract_type_of(Or)) << '\n'
        << LVD_REFLECT(abstract_type_of(Xor)) << '\n'
        << LVD_REFLECT(abstract_type_of(Not)) << '\n'
        << LVD_REFLECT(abstract_type_of(LogicalBinOp)) << '\n'
        << LVD_REFLECT(abstract_type_of(LogicalUnOp)) << '\n'
        << '\n';
    std::cout
        << LVD_REFLECT(compare(And, And)) << '\n'
        << LVD_REFLECT(compare(And, Or)) << '\n'
        << LVD_REFLECT(compare(And, Xor)) << '\n'
        << LVD_REFLECT(compare(Or, And)) << '\n'
        << LVD_REFLECT(compare(Or, Or)) << '\n'
        << LVD_REFLECT(compare(Or, Xor)) << '\n'
        << LVD_REFLECT(compare(Xor, And)) << '\n'
        << LVD_REFLECT(compare(Xor, Or)) << '\n'
        << LVD_REFLECT(compare(Xor, Xor)) << '\n'
        << '\n';
    std::cout
        << LVD_REFLECT(compare(Not, Not)) << '\n'
        << '\n';
    std::cout
        << LVD_REFLECT(LogicalExpr(true)) << '\n'
        << LVD_REFLECT(LogicalExpr(false)) << '\n'
        << LVD_REFLECT(LogicalExpr(sept::True)) << '\n'
        << LVD_REFLECT(LogicalExpr(sept::False)) << '\n'
        << LVD_REFLECT(sept::inhabits_data(sept::Term, LogicalExpr)) << '\n'
        << '\n';
    std::cout
        << LVD_REFLECT(sept::inhabits_data(And, LogicalBinOp)) << '\n'
        << LVD_REFLECT(LogicalBinOpExpr(true, And, false)) << '\n'
        << LVD_REFLECT(LogicalBinOpExpr(true, Or, LogicalBinOpExpr(true, Xor, false))) << '\n'
        << LVD_REFLECT(LogicalUnOpExpr(Not, true)) << '\n'
        << LVD_REFLECT(LogicalUnOpExpr(Not, LogicalBinOpExpr(true, And, false))) << '\n'
        << '\n';
    std::cout
        << LVD_REFLECT(evaluate_expr(true)) << '\n'
        << LVD_REFLECT(evaluate_expr(false)) << '\n'
        << '\n'
        << LVD_REFLECT(evaluate_expr(LogicalBinOpExpr(true, And, true))) << '\n'
        << LVD_REFLECT(evaluate_expr(LogicalBinOpExpr(true, And, false))) << '\n'
        << LVD_REFLECT(evaluate_expr(LogicalBinOpExpr(false, And, false))) << '\n'
        << LVD_REFLECT(evaluate_expr(LogicalBinOpExpr(false, And, true))) << '\n'
        << LVD_REFLECT(evaluate_expr(LogicalBinOpExpr(true, Or, true))) << '\n'
        << LVD_REFLECT(evaluate_expr(LogicalBinOpExpr(true, Or, false))) << '\n'
        << LVD_REFLECT(evaluate_expr(LogicalBinOpExpr(false, Or, false))) << '\n'
        << LVD_REFLECT(evaluate_expr(LogicalBinOpExpr(false, Or, true))) << '\n'
        << LVD_REFLECT(evaluate_expr(LogicalBinOpExpr(true, Xor, true))) << '\n'
        << LVD_REFLECT(evaluate_expr(LogicalBinOpExpr(true, Xor, false))) << '\n'
        << LVD_REFLECT(evaluate_expr(LogicalBinOpExpr(false, Xor, false))) << '\n'
        << LVD_REFLECT(evaluate_expr(LogicalBinOpExpr(false, Xor, true))) << '\n'
        << '\n'
        << LVD_REFLECT(evaluate_expr(LogicalUnOpExpr(Not, true))) << '\n'
        << LVD_REFLECT(evaluate_expr(LogicalUnOpExpr(Not, false))) << '\n'
        << '\n';
    std::cout
        << LVD_REFLECT(evaluate_expr_data(true)) << '\n'
        << LVD_REFLECT(evaluate_expr_data(false)) << '\n'
        << '\n'
        << LVD_REFLECT(evaluate_expr_data(LogicalBinOpExpr(true, And, true))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(LogicalBinOpExpr(true, And, false))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(LogicalBinOpExpr(false, And, false))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(LogicalBinOpExpr(false, And, true))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(LogicalBinOpExpr(true, Or, true))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(LogicalBinOpExpr(true, Or, false))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(LogicalBinOpExpr(false, Or, false))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(LogicalBinOpExpr(false, Or, true))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(LogicalBinOpExpr(true, Xor, true))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(LogicalBinOpExpr(true, Xor, false))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(LogicalBinOpExpr(false, Xor, false))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(LogicalBinOpExpr(false, Xor, true))) << '\n'
        << '\n'
        << LVD_REFLECT(evaluate_expr_data(LogicalUnOpExpr(Not, true))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(LogicalUnOpExpr(Not, false))) << '\n'
        << '\n';

    return 0;
}
