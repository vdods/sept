#include <cmath>
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
    -   ExprKind := Set(LogicalExpr, NumericExpr)
    -   Expr := UnionOfSets(ExprKind)
    -   CondExpr := Tuple(LogicalExpr, X, X) where X:ExprKind
    The latter definition, for CondExpr, is not possible (yet) declaratively, but it is possible
    by a definition of `inhabits`.
#endif

//
// LOGICAL
// LOGICAL
// LOGICAL
//

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
// LogicalCondExpr_c
//

class LogicalCondExpr_c {
public:

    // Really all this does is verify that expr:LogicalCondExpr
    // TODO: Template types, use non-Data version of inhabits
    // TODO: Another way this could work is that it applies the abstract type LogicalCondExpr to the concrete value returned.
    sept::ArrayTerm_c operator() (sept::Data &&conditional, sept::Data &&positive_expr, sept::Data &&negative_expr) const {
        auto retval = sept::Array(std::move(conditional), std::move(positive_expr), std::move(negative_expr));
        if (!inhabits_data(retval, LogicalCondExpr_c{}))
            throw std::runtime_error(LVD_FMT("ill-formed LogicalCondExpr: " << retval));
        return retval;
    }
};

std::ostream &operator<< (std::ostream &out, LogicalCondExpr_c const &) {
    return out << "LogicalCondExpr";
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
// NUMERIC
// NUMERIC
// NUMERIC
//

//
// NumericBinOpTerm_c
//

using NumericBinOpTermRepr = uint8_t;
enum class NumericBinOpTerm_c : NumericBinOpTermRepr {
    ADD = 0,
    SUB,
    MUL,
    DIV,
    POW,

    __LOWEST__ = ADD,
    __HIGHEST__ = POW,
};

std::string const &as_string (NumericBinOpTerm_c t) {
    static std::vector<std::string> const TABLE{
        "Add",
        "Sub",
        "Mul",
        "Div",
        "Pow",
    };
    return TABLE.at(size_t(t));
}

std::ostream &operator<< (std::ostream &out, NumericBinOpTerm_c const &t) {
    return out << as_string(t);
}

//
// NumericBinOp_c
//

class NumericBinOp_c {
public:

    // TODO: Another way this could work is that it applies the abstract type NumericBinOp to the concrete value returned.
    NumericBinOpTerm_c operator() (NumericBinOpTermRepr v) const {
        if (v > NumericBinOpTermRepr(NumericBinOpTerm_c::__HIGHEST__))
            throw std::out_of_range("invalid value for NumericBinOpTerm_c");
        return NumericBinOpTerm_c{v};
    }
};

std::ostream &operator<< (std::ostream &out, NumericBinOp_c const &) {
    return out << "NumericBinOp";
}

//
// NumericBinOpExpr_c
//

class NumericBinOpExpr_c {
public:

    // TODO: Template types, use non-Data version of inhabits
    // TODO: Another way this could work is that it applies the abstract type NumericBinOpExpr to the concrete value returned.
    sept::ArrayTerm_c operator() (sept::Data &&lhs, sept::Data &&bin_op, sept::Data &&rhs) const {
        auto retval = sept::Array(std::move(lhs), std::move(bin_op), std::move(rhs));
        if (!inhabits_data(retval, NumericBinOpExpr_c{}))
            throw std::runtime_error(LVD_FMT("ill-formed NumericBinOpExpr: " << retval));
        return retval;
    }
};

std::ostream &operator<< (std::ostream &out, NumericBinOpExpr_c const &) {
    return out << "NumericBinOpExpr";
}

//
// NumericCondExpr_c
//

class NumericCondExpr_c {
public:

    // Really all this does is verify that expr:NumericCondExpr
    // TODO: Template types, use non-Data version of inhabits
    // TODO: Another way this could work is that it applies the abstract type NumericCondExpr to the concrete value returned.
    sept::ArrayTerm_c operator() (sept::Data &&conditional, sept::Data &&positive_expr, sept::Data &&negative_expr) const {
        auto retval = sept::Array(std::move(conditional), std::move(positive_expr), std::move(negative_expr));
        if (!inhabits_data(retval, NumericCondExpr_c{}))
            throw std::runtime_error(LVD_FMT("ill-formed NumericCondExpr: " << retval));
        return retval;
    }
};

std::ostream &operator<< (std::ostream &out, NumericCondExpr_c const &) {
    return out << "NumericCondExpr";
}

//
// NumericExpr_c
//

class NumericExpr_c {
public:

    // Really all this does is verify that expr:NumericExpr
    // TODO: Template types, use non-Data version of inhabits
    // TODO: Another way this could work is that it applies the abstract type NumericExpr to the concrete value returned.
    sept::Data operator() (sept::Data &&expr) const {
        if (!inhabits_data(expr, NumericExpr_c{}))
            throw std::runtime_error(LVD_FMT("ill-formed NumericExpr: " << expr));
        return expr;
    }
};

std::ostream &operator<< (std::ostream &out, NumericExpr_c const &) {
    return out << "NumericExpr";
}

//
// NumericUnOpTerm_c
//

using NumericUnOpTermRepr = uint8_t;
enum class NumericUnOpTerm_c : NumericUnOpTermRepr {
    NEG = 0,

    __LOWEST__ = NEG,
    __HIGHEST__ = NEG,
};

std::string const &as_string (NumericUnOpTerm_c t) {
    static std::vector<std::string> const TABLE{
        "Neg",
    };
    return TABLE.at(size_t(t));
}

std::ostream &operator<< (std::ostream &out, NumericUnOpTerm_c const &t) {
    return out << as_string(t);
}

//
// NumericUnOp_c
//

class NumericUnOp_c {
public:

    // TODO: Another way this could work is that it applies the abstract type NumericUnOp to the concrete value returned.
    NumericUnOpTerm_c operator() (NumericUnOpTermRepr v) const {
        if (v > NumericUnOpTermRepr(NumericUnOpTerm_c::__HIGHEST__))
            throw std::out_of_range("invalid value for NumericUnOpTerm_c");
        return NumericUnOpTerm_c{v};
    }
};

std::ostream &operator<< (std::ostream &out, NumericUnOp_c const &) {
    return out << "NumericUnOp";
}

//
// NumericUnOpExpr_c
//

class NumericUnOpExpr_c {
public:

    // TODO: Template types, use non-Data version of inhabits
    sept::ArrayTerm_c operator() (sept::Data &&un_op, sept::Data &&operand) const {
        auto retval = sept::Array(std::move(un_op), std::move(operand));
        if (!inhabits_data(retval, NumericUnOpExpr_c{}))
            throw std::runtime_error(LVD_FMT("ill-formed NumericUnOpExpr: " << retval));
        return retval;
    }
};

std::ostream &operator<< (std::ostream &out, NumericUnOpExpr_c const &) {
    return out << "NumericUnOpExpr";
}

//
// Declarations/definitions
//

LogicalBinOp_c const LogicalBinOp;
LogicalBinOpExpr_c const LogicalBinOpExpr;
LogicalCondExpr_c const LogicalCondExpr;
LogicalExpr_c const LogicalExpr;
LogicalUnOp_c const LogicalUnOp;
LogicalUnOpExpr_c const LogicalUnOpExpr;

LogicalBinOpTerm_c const And{LogicalBinOpTerm_c::AND};
LogicalBinOpTerm_c const Or{LogicalBinOpTerm_c::OR};
LogicalBinOpTerm_c const Xor{LogicalBinOpTerm_c::XOR};

LogicalUnOpTerm_c const Not{LogicalUnOpTerm_c::NOT};

NumericBinOp_c const NumericBinOp;
NumericBinOpExpr_c const NumericBinOpExpr;
NumericCondExpr_c const NumericCondExpr;
NumericExpr_c const NumericExpr;
NumericUnOp_c const NumericUnOp;
NumericUnOpExpr_c const NumericUnOpExpr;

NumericBinOpTerm_c const Add{NumericBinOpTerm_c::ADD};
NumericBinOpTerm_c const Sub{NumericBinOpTerm_c::SUB};
NumericBinOpTerm_c const Mul{NumericBinOpTerm_c::MUL};
NumericBinOpTerm_c const Div{NumericBinOpTerm_c::DIV};
NumericBinOpTerm_c const Pow{NumericBinOpTerm_c::POW};

NumericUnOpTerm_c const Neg{NumericUnOpTerm_c::NEG};

inline constexpr LogicalBinOp_c const &abstract_type_of (LogicalBinOpTerm_c const &) { return LogicalBinOp; }
inline constexpr LogicalUnOp_c const &abstract_type_of (LogicalUnOpTerm_c const &) { return LogicalUnOp; }
inline constexpr sept::NonParametricType_c const &abstract_type_of (LogicalBinOp_c const &) { return sept::NonParametricType; }
inline constexpr sept::NonParametricType_c const &abstract_type_of (LogicalBinOpExpr_c const &) { return sept::NonParametricType; }
inline constexpr sept::NonParametricType_c const &abstract_type_of (LogicalCondExpr_c const &) { return sept::NonParametricType; }
inline constexpr sept::NonParametricType_c const &abstract_type_of (LogicalExpr_c const &) { return sept::NonParametricType; }
inline constexpr sept::NonParametricType_c const &abstract_type_of (LogicalUnOp_c const &) { return sept::NonParametricType; }
inline constexpr sept::NonParametricType_c const &abstract_type_of (LogicalUnOpExpr_c const &) { return sept::NonParametricType; }

inline constexpr NumericBinOp_c const &abstract_type_of (NumericBinOpTerm_c const &) { return NumericBinOp; }
inline constexpr NumericUnOp_c const &abstract_type_of (NumericUnOpTerm_c const &) { return NumericUnOp; }
inline constexpr sept::NonParametricType_c const &abstract_type_of (NumericBinOp_c const &) { return sept::NonParametricType; }
inline constexpr sept::NonParametricType_c const &abstract_type_of (NumericBinOpExpr_c const &) { return sept::NonParametricType; }
inline constexpr sept::NonParametricType_c const &abstract_type_of (NumericCondExpr_c const &) { return sept::NonParametricType; }
inline constexpr sept::NonParametricType_c const &abstract_type_of (NumericExpr_c const &) { return sept::NonParametricType; }
inline constexpr sept::NonParametricType_c const &abstract_type_of (NumericUnOp_c const &) { return sept::NonParametricType; }
inline constexpr sept::NonParametricType_c const &abstract_type_of (NumericUnOpExpr_c const &) { return sept::NonParametricType; }

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

int compare (NumericBinOpTerm_c const &lhs, NumericBinOpTerm_c const &rhs) {
    return compare(NumericBinOpTermRepr(lhs), NumericBinOpTermRepr(rhs));
}

int compare (NumericUnOpTerm_c const &lhs, NumericUnOpTerm_c const &rhs) {
    return compare(NumericUnOpTermRepr(lhs), NumericUnOpTermRepr(rhs));
}

bool inhabits (sept::ArrayTerm_c const &value, LogicalBinOpExpr_c const &type) {
    return value.size() == 3
        && inhabits_data(value[0], LogicalExpr)
        && inhabits_data(value[1], LogicalBinOp)
        && inhabits_data(value[2], LogicalExpr);
}

bool inhabits (sept::ArrayTerm_c const &value, LogicalCondExpr_c const &type) {
    return value.size() == 3
        && inhabits_data(value[0], LogicalExpr)
        && inhabits_data(value[1], LogicalExpr)
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
        || inhabits_data(value, LogicalCondExpr)
//         || inhabits_data(value, LogicalParenExpr)
        || inhabits_data(value, LogicalUnOpExpr);
}

bool inhabits (sept::ArrayTerm_c const &value, NumericBinOpExpr_c const &type) {
    return value.size() == 3
        && inhabits_data(value[0], NumericExpr)
        && inhabits_data(value[1], NumericBinOp)
        && inhabits_data(value[2], NumericExpr);
}

bool inhabits (sept::ArrayTerm_c const &value, NumericUnOpExpr_c const &type) {
    return value.size() == 2
        && inhabits_data(value[0], NumericUnOp)
        && inhabits_data(value[1], NumericExpr);
}

bool inhabits (sept::ArrayTerm_c const &value, NumericCondExpr_c const &type) {
    return value.size() == 3
        && inhabits_data(value[0], LogicalExpr)
        && inhabits_data(value[1], NumericExpr)
        && inhabits_data(value[2], NumericExpr);
}

bool inhabits (sept::ArrayTerm_c const &value, NumericExpr_c const &type) {
    return inhabits_data(value, sept::Float64)
        || inhabits_data(value, NumericBinOpExpr)
        || inhabits_data(value, NumericCondExpr)
//         || inhabits_data(value, NumericParenExpr)
        || inhabits_data(value, NumericUnOpExpr);
}

// } // end namespace sept

// NOTE: This is sorta hacky, though it work.  TODO: come up with a better way
namespace sept {
SEPT__REGISTER__PRINT(LogicalBinOp_c)
SEPT__REGISTER__PRINT(LogicalBinOpExpr_c)
SEPT__REGISTER__PRINT(LogicalBinOpTerm_c)
SEPT__REGISTER__PRINT(LogicalCondExpr_c)
SEPT__REGISTER__PRINT(LogicalExpr_c)
SEPT__REGISTER__PRINT(LogicalUnOp_c)
SEPT__REGISTER__PRINT(LogicalUnOpExpr_c)
SEPT__REGISTER__PRINT(LogicalUnOpTerm_c)
SEPT__REGISTER__PRINT(NumericBinOp_c)
SEPT__REGISTER__PRINT(NumericBinOpExpr_c)
SEPT__REGISTER__PRINT(NumericBinOpTerm_c)
SEPT__REGISTER__PRINT(NumericCondExpr_c)
SEPT__REGISTER__PRINT(NumericExpr_c)
SEPT__REGISTER__PRINT(NumericUnOp_c)
SEPT__REGISTER__PRINT(NumericUnOpExpr_c)
SEPT__REGISTER__PRINT(NumericUnOpTerm_c)

SEPT__REGISTER__EQ(LogicalBinOp_c)
SEPT__REGISTER__EQ(LogicalBinOpExpr_c)
SEPT__REGISTER__EQ(LogicalBinOpTerm_c)
SEPT__REGISTER__EQ(LogicalCondExpr_c)
SEPT__REGISTER__EQ(LogicalExpr_c)
SEPT__REGISTER__EQ(LogicalUnOp_c)
SEPT__REGISTER__EQ(LogicalUnOpExpr_c)
SEPT__REGISTER__EQ(LogicalUnOpTerm_c)
SEPT__REGISTER__EQ(NumericBinOp_c)
SEPT__REGISTER__EQ(NumericBinOpExpr_c)
SEPT__REGISTER__EQ(NumericBinOpTerm_c)
SEPT__REGISTER__EQ(NumericCondExpr_c)
SEPT__REGISTER__EQ(NumericExpr_c)
SEPT__REGISTER__EQ(NumericUnOp_c)
SEPT__REGISTER__EQ(NumericUnOpExpr_c)
SEPT__REGISTER__EQ(NumericUnOpTerm_c)

SEPT__REGISTER__ABSTRACT_TYPE_OF(LogicalBinOp_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(LogicalBinOpExpr_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(LogicalBinOpTerm_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(LogicalCondExpr_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(LogicalExpr_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(LogicalUnOp_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(LogicalUnOpExpr_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(LogicalUnOpTerm_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(NumericBinOp_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(NumericBinOpExpr_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(NumericBinOpTerm_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(NumericCondExpr_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(NumericExpr_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(NumericUnOp_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(NumericUnOpExpr_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(NumericUnOpTerm_c)

SEPT__REGISTER__INHABITS(LogicalBinOp_c, NonParametricType_c)
SEPT__REGISTER__INHABITS(LogicalBinOpExpr_c, NonParametricType_c)
SEPT__REGISTER__INHABITS(LogicalBinOpTerm_c, LogicalBinOp_c)
SEPT__REGISTER__INHABITS__EVALUATOR_BODY(ArrayTerm_c, LogicalCondExpr_c, return inhabits(value, type);)
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
SEPT__REGISTER__INHABITS(NumericBinOp_c, NonParametricType_c)
SEPT__REGISTER__INHABITS(NumericBinOpExpr_c, NonParametricType_c)
SEPT__REGISTER__INHABITS(NumericBinOpTerm_c, NumericBinOp_c)
SEPT__REGISTER__INHABITS__EVALUATOR_BODY(ArrayTerm_c, NumericCondExpr_c, return inhabits(value, type);)
SEPT__REGISTER__INHABITS(NumericExpr_c, NonParametricType_c)
// TODO: Could just use `sept::Tuple_c const NumericBinOpExpr = sept::Tuple(NumericExpr, NumericBinOp, NumericExpr);`
SEPT__REGISTER__INHABITS__EVALUATOR_BODY(ArrayTerm_c, NumericBinOpExpr_c, return inhabits(value, type);)
// TODO: Could just use `sept::Tuple_c const NumericUnOpExpr = sept::Tuple(NumericBinOp, NumericExpr);`
SEPT__REGISTER__INHABITS__EVALUATOR_BODY(ArrayTerm_c, NumericUnOpExpr_c, return inhabits(value, type);)
SEPT__REGISTER__INHABITS(double, NumericExpr_c)
// TODO: Could just use `sept::Union_c const NumericExpr = sept::Union(Bool, NumericBinOpExpr, NumericParenExpr, NumericUnOpExpr);`
SEPT__REGISTER__INHABITS__EVALUATOR_BODY(ArrayTerm_c, NumericExpr_c, return inhabits(value, type);)
SEPT__REGISTER__INHABITS(NumericUnOp_c, NonParametricType_c)
SEPT__REGISTER__INHABITS(NumericUnOpExpr_c, NonParametricType_c)
SEPT__REGISTER__INHABITS(NumericUnOpTerm_c, NumericUnOp_c)

SEPT__REGISTER__COMPARE__SINGLETON(LogicalBinOp_c)
SEPT__REGISTER__COMPARE__SINGLETON(LogicalBinOpExpr_c)
SEPT__REGISTER__COMPARE(LogicalBinOpTerm_c, LogicalBinOpTerm_c)
SEPT__REGISTER__COMPARE__SINGLETON(LogicalCondExpr_c)
SEPT__REGISTER__COMPARE__SINGLETON(LogicalExpr_c)
SEPT__REGISTER__COMPARE__SINGLETON(LogicalUnOp_c)
SEPT__REGISTER__COMPARE__SINGLETON(LogicalUnOpExpr_c)
SEPT__REGISTER__COMPARE(LogicalUnOpTerm_c, LogicalUnOpTerm_c)
SEPT__REGISTER__COMPARE__SINGLETON(NumericBinOp_c)
SEPT__REGISTER__COMPARE__SINGLETON(NumericBinOpExpr_c)
SEPT__REGISTER__COMPARE(NumericBinOpTerm_c, NumericBinOpTerm_c)
SEPT__REGISTER__COMPARE__SINGLETON(NumericCondExpr_c)
SEPT__REGISTER__COMPARE__SINGLETON(NumericExpr_c)
SEPT__REGISTER__COMPARE__SINGLETON(NumericUnOp_c)
SEPT__REGISTER__COMPARE__SINGLETON(NumericUnOpExpr_c)
SEPT__REGISTER__COMPARE(NumericUnOpTerm_c, NumericUnOpTerm_c)
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

bool evaluate_expr__as_LogicalCondExpr (sept::ArrayTerm_c const &a) {
    assert(inhabits(a, LogicalCondExpr));

    // This is a bit silly
    bool condition = evaluate_expr_data(a[0]) == sept::True || evaluate_expr_data(a[0]) == sept::Data{true};
    auto evaluation = evaluate_expr_data(a[condition ? 1 : 2]);
    // This is silly.
    return evaluation == sept::True || evaluation == sept::Data{true};
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

double evaluate_expr__as_NumericBinOpExpr (sept::ArrayTerm_c const &a) {
    assert(inhabits(a, NumericBinOpExpr));

    // This is a bit silly
    auto const &lhs = evaluate_expr_data(a[0]).cast<double const &>();
    auto const &rhs = evaluate_expr_data(a[2]).cast<double const &>();
    switch (a[1].cast<NumericBinOpTerm_c const &>()) {
        case NumericBinOpTerm_c::ADD: return lhs + rhs;
        case NumericBinOpTerm_c::SUB: return lhs - rhs;
        case NumericBinOpTerm_c::MUL: return lhs * rhs;
        case NumericBinOpTerm_c::DIV: return lhs / rhs;
        case NumericBinOpTerm_c::POW: return std::pow(lhs, rhs);
        default: LVD_ABORT("invalid NumericBinOpTerm_c");
    }
}

double evaluate_expr__as_NumericCondExpr (sept::ArrayTerm_c const &a) {
    assert(inhabits(a, NumericCondExpr));

    // This is a bit silly
    bool condition = evaluate_expr_data(a[0]) == sept::True || evaluate_expr_data(a[0]) == sept::Data{true};
    return evaluate_expr_data(a[condition ? 1 : 2]).cast<double const &>();
}

double evaluate_expr__as_NumericUnOpExpr (sept::ArrayTerm_c const &a) {
    assert(inhabits(a, NumericUnOpExpr));

    // This is a bit silly
    auto const &operand = evaluate_expr_data(a[1]).cast<double const &>();
    switch (a[0].cast<NumericUnOpTerm_c const &>()) {
        case NumericUnOpTerm_c::NEG: return -operand;
        default: LVD_ABORT("invalid NumericUnOpTerm_c");
    }
}

bool evaluate_expr (bool const &expr) { return expr; }
// bool evaluate_expr (sept::BoolTerm_c const &expr) { return expr; }
sept::BoolTerm_c evaluate_expr (sept::BoolTerm_c const &expr) { return expr; }
double evaluate_expr (double const &expr) { return expr; }

bool evaluate_expr__as_LogicalExpr (sept::ArrayTerm_c const &a) {
    // This is a bit silly, but hey.
    if (inhabits(a, LogicalBinOpExpr))
        return evaluate_expr__as_LogicalBinOpExpr(a);
    else if (inhabits(a, LogicalCondExpr))
        return evaluate_expr__as_LogicalCondExpr(a);
    else if (inhabits(a, LogicalUnOpExpr))
        return evaluate_expr__as_LogicalUnOpExpr(a);
    else
        LVD_ABORT(LVD_FMT("invalid LogicalExpr Array: " << a));
}

double evaluate_expr__as_NumericExpr (sept::ArrayTerm_c const &a) {
    // This is a bit silly, but hey.
    if (inhabits(a, NumericBinOpExpr))
        return evaluate_expr__as_NumericBinOpExpr(a);
    else if (inhabits(a, NumericCondExpr))
        return evaluate_expr__as_NumericCondExpr(a);
    else if (inhabits(a, NumericUnOpExpr))
        return evaluate_expr__as_NumericUnOpExpr(a);
    else
        LVD_ABORT(LVD_FMT("invalid NumericExpr Array: " << a));
}

// TODO: The return type could be improved at best to std::variant<bool,double> here.
// But for now, just use sept::Data.
sept::Data evaluate_expr (sept::ArrayTerm_c const &a) {
    // This is a bit silly, but hey.
    if (inhabits(a, LogicalBinOpExpr))
        return evaluate_expr__as_LogicalBinOpExpr(a);
    else if (inhabits(a, LogicalCondExpr))
        return evaluate_expr__as_LogicalCondExpr(a);
    else if (inhabits(a, LogicalUnOpExpr))
        return evaluate_expr__as_LogicalUnOpExpr(a);
    else if (inhabits(a, NumericBinOpExpr))
        return evaluate_expr__as_NumericBinOpExpr(a);
    else if (inhabits(a, NumericCondExpr))
        return evaluate_expr__as_NumericCondExpr(a);
    else if (inhabits(a, NumericUnOpExpr))
        return evaluate_expr__as_NumericUnOpExpr(a);
    else
        LVD_ABORT(LVD_FMT("invalid LogicalExpr or NumericExpr Array: " << a));
}

SEPT__REGISTER__EVALUATE_EXPR(bool)
SEPT__REGISTER__EVALUATE_EXPR(double)
namespace sept {
SEPT__REGISTER__EVALUATE_EXPR(BoolTerm_c)
SEPT__REGISTER__EVALUATE_EXPR(ArrayTerm_c)
} // end namespace sept

int main (int argc, char **argv) {
    std::cout << std::boolalpha;

    std::cout << LVD_REFLECT(sizeof(std::any)) << lvd::cspace << LVD_REFLECT(sizeof(sept::Data)) << '\n';

    std::cout
        << LVD_REFLECT(And) << '\n'
        << LVD_REFLECT(Or) << '\n'
        << LVD_REFLECT(Xor) << '\n'
        << LVD_REFLECT(Not) << '\n'
        << LVD_REFLECT(LogicalBinOp) << '\n'
        << LVD_REFLECT(LogicalUnOp) << '\n'
        << '\n';
    std::cout
        << LVD_REFLECT(Add) << '\n'
        << LVD_REFLECT(Sub) << '\n'
        << LVD_REFLECT(Mul) << '\n'
        << LVD_REFLECT(Div) << '\n'
        << LVD_REFLECT(Pow) << '\n'
        << LVD_REFLECT(Neg) << '\n'
        << LVD_REFLECT(NumericBinOp) << '\n'
        << LVD_REFLECT(NumericUnOp) << '\n'
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
        << LVD_REFLECT(abstract_type_of(Add)) << '\n'
        << LVD_REFLECT(abstract_type_of(Sub)) << '\n'
        << LVD_REFLECT(abstract_type_of(Mul)) << '\n'
        << LVD_REFLECT(abstract_type_of(Div)) << '\n'
        << LVD_REFLECT(abstract_type_of(Pow)) << '\n'
        << LVD_REFLECT(abstract_type_of(Neg)) << '\n'
        << LVD_REFLECT(abstract_type_of(NumericBinOp)) << '\n'
        << LVD_REFLECT(abstract_type_of(NumericUnOp)) << '\n'
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
        << LVD_REFLECT(compare(Add, Add)) << '\n'
        << LVD_REFLECT(compare(Add, Sub)) << '\n'
        << LVD_REFLECT(compare(Add, Mul)) << '\n'
        << LVD_REFLECT(compare(Add, Div)) << '\n'
        << LVD_REFLECT(compare(Add, Pow)) << '\n'
        << LVD_REFLECT(compare(Sub, Add)) << '\n'
        << LVD_REFLECT(compare(Sub, Sub)) << '\n'
        << LVD_REFLECT(compare(Sub, Mul)) << '\n'
        << LVD_REFLECT(compare(Sub, Div)) << '\n'
        << LVD_REFLECT(compare(Sub, Pow)) << '\n'
        << LVD_REFLECT(compare(Mul, Add)) << '\n'
        << LVD_REFLECT(compare(Mul, Sub)) << '\n'
        << LVD_REFLECT(compare(Mul, Mul)) << '\n'
        << LVD_REFLECT(compare(Mul, Div)) << '\n'
        << LVD_REFLECT(compare(Mul, Pow)) << '\n'
        << LVD_REFLECT(compare(Div, Add)) << '\n'
        << LVD_REFLECT(compare(Div, Sub)) << '\n'
        << LVD_REFLECT(compare(Div, Mul)) << '\n'
        << LVD_REFLECT(compare(Div, Div)) << '\n'
        << LVD_REFLECT(compare(Div, Pow)) << '\n'
        << LVD_REFLECT(compare(Pow, Add)) << '\n'
        << LVD_REFLECT(compare(Pow, Sub)) << '\n'
        << LVD_REFLECT(compare(Pow, Mul)) << '\n'
        << LVD_REFLECT(compare(Pow, Div)) << '\n'
        << LVD_REFLECT(compare(Pow, Pow)) << '\n'
        << '\n';
    std::cout
        << LVD_REFLECT(compare(Not, Not)) << '\n'
        << '\n';
    std::cout
        << LVD_REFLECT(compare(Neg, Neg)) << '\n'
        << '\n';
    std::cout
        << LVD_REFLECT(LogicalExpr(true)) << '\n'
        << LVD_REFLECT(LogicalExpr(false)) << '\n'
        << LVD_REFLECT(LogicalExpr(sept::True)) << '\n'
        << LVD_REFLECT(LogicalExpr(sept::False)) << '\n'
        << LVD_REFLECT(sept::inhabits_data(sept::Term, LogicalExpr)) << '\n'
        << '\n';
    std::cout
        << LVD_REFLECT(NumericExpr(123.5)) << '\n'
        << LVD_REFLECT(NumericExpr(-8.2e-3)) << '\n'
        << LVD_REFLECT(sept::inhabits_data(sept::Term, NumericExpr)) << '\n'
        << '\n';
    std::cout
        << LVD_REFLECT(sept::inhabits_data(And, LogicalBinOp)) << '\n'
        << LVD_REFLECT(LogicalBinOpExpr(true, And, false)) << '\n'
        << LVD_REFLECT(LogicalBinOpExpr(true, Or, LogicalBinOpExpr(true, Xor, false))) << '\n'
        << LVD_REFLECT(LogicalUnOpExpr(Not, true)) << '\n'
        << LVD_REFLECT(LogicalUnOpExpr(Not, LogicalBinOpExpr(true, And, false))) << '\n'
        << '\n';
    std::cout
        << LVD_REFLECT(sept::inhabits_data(Add, NumericBinOp)) << '\n'
        << LVD_REFLECT(NumericBinOpExpr(5.5, Add, 10.25)) << '\n'
        << LVD_REFLECT(NumericBinOpExpr(5.5, Mul, NumericBinOpExpr(5.5, Pow, 10.25))) << '\n'
        << LVD_REFLECT(NumericUnOpExpr(Neg, 5.5)) << '\n'
        << LVD_REFLECT(NumericUnOpExpr(Neg, NumericBinOpExpr(5.5, Pow, 10.25))) << '\n'
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
        << LVD_REFLECT(evaluate_expr(5.5)) << '\n'
        << LVD_REFLECT(evaluate_expr(10.25)) << '\n'
        << '\n'
        << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(5.5, Add, 5.5))) << '\n'
        << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(5.5, Add, 10.25))) << '\n'
        << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(10.25, Add, 10.25))) << '\n'
        << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(10.25, Add, 5.5))) << '\n'
        << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(5.5, Sub, 5.5))) << '\n'
        << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(5.5, Sub, 10.25))) << '\n'
        << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(10.25, Sub, 10.25))) << '\n'
        << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(10.25, Sub, 5.5))) << '\n'
        << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(5.5, Mul, 5.5))) << '\n'
        << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(5.5, Mul, 10.25))) << '\n'
        << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(10.25, Mul, 10.25))) << '\n'
        << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(10.25, Mul, 5.5))) << '\n'
        << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(5.5, Div, 5.5))) << '\n'
        << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(5.5, Div, 10.25))) << '\n'
        << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(10.25, Div, 10.25))) << '\n'
        << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(10.25, Div, 5.5))) << '\n'
        << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(5.5, Pow, 5.5))) << '\n'
        << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(5.5, Pow, 10.25))) << '\n'
        << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(10.25, Pow, 10.25))) << '\n'
        << LVD_REFLECT(evaluate_expr(NumericBinOpExpr(10.25, Pow, 5.5))) << '\n'
        << '\n'
        << LVD_REFLECT(evaluate_expr(NumericUnOpExpr(Neg, 5.5))) << '\n'
        << LVD_REFLECT(evaluate_expr(NumericUnOpExpr(Neg, 10.25))) << '\n'
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
    std::cout
        << LVD_REFLECT(evaluate_expr_data(5.5)) << '\n'
        << LVD_REFLECT(evaluate_expr_data(10.25)) << '\n'
        << '\n'
        << LVD_REFLECT(evaluate_expr_data(NumericBinOpExpr(5.5, Add, 5.5))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(NumericBinOpExpr(5.5, Add, 10.25))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(NumericBinOpExpr(10.25, Add, 10.25))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(NumericBinOpExpr(10.25, Add, 5.5))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(NumericBinOpExpr(5.5, Sub, 5.5))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(NumericBinOpExpr(5.5, Sub, 10.25))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(NumericBinOpExpr(10.25, Sub, 10.25))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(NumericBinOpExpr(10.25, Sub, 5.5))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(NumericBinOpExpr(5.5, Mul, 5.5))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(NumericBinOpExpr(5.5, Mul, 10.25))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(NumericBinOpExpr(10.25, Mul, 10.25))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(NumericBinOpExpr(10.25, Mul, 5.5))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(NumericBinOpExpr(5.5, Div, 5.5))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(NumericBinOpExpr(5.5, Div, 10.25))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(NumericBinOpExpr(10.25, Div, 10.25))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(NumericBinOpExpr(10.25, Div, 5.5))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(NumericBinOpExpr(5.5, Pow, 5.5))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(NumericBinOpExpr(5.5, Pow, 10.25))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(NumericBinOpExpr(10.25, Pow, 10.25))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(NumericBinOpExpr(10.25, Pow, 5.5))) << '\n'
        << '\n'
        << LVD_REFLECT(evaluate_expr_data(NumericUnOpExpr(Neg, 5.5))) << '\n'
        << LVD_REFLECT(evaluate_expr_data(NumericUnOpExpr(Neg, 10.25))) << '\n'
        << '\n';
    std::cout
        << LVD_REFLECT(LogicalCondExpr(true, true, false)) << '\n'
        << LVD_REFLECT(LogicalCondExpr(true, true, sept::False)) << '\n'
        << LVD_REFLECT(LogicalCondExpr(true, sept::True, sept::False)) << '\n'
        << LVD_REFLECT(LogicalCondExpr(true, sept::True, false)) << '\n'
        << LVD_REFLECT(LogicalCondExpr(LogicalBinOpExpr(sept::True, Xor, sept::False), true, false)) << '\n'
        << '\n';
    std::cout
        << LVD_REFLECT(evaluate_expr(LogicalCondExpr(true, true, false))) << '\n'
        << LVD_REFLECT(evaluate_expr(LogicalCondExpr(true, true, sept::False))) << '\n'
        << LVD_REFLECT(evaluate_expr(LogicalCondExpr(true, sept::True, sept::False))) << '\n'
        << LVD_REFLECT(evaluate_expr(LogicalCondExpr(true, sept::True, false))) << '\n'
        << LVD_REFLECT(evaluate_expr(LogicalCondExpr(false, true, false))) << '\n'
        << LVD_REFLECT(evaluate_expr(LogicalCondExpr(false, true, sept::False))) << '\n'
        << LVD_REFLECT(evaluate_expr(LogicalCondExpr(false, sept::True, sept::False))) << '\n'
        << LVD_REFLECT(evaluate_expr(LogicalCondExpr(false, sept::True, false))) << '\n'
        << LVD_REFLECT(evaluate_expr(LogicalCondExpr(LogicalBinOpExpr(sept::True, Xor, sept::False), true, false))) << '\n'
        << '\n';
    std::cout
        << LVD_REFLECT(NumericCondExpr(true, 5.5, 10.25)) << '\n'
        << LVD_REFLECT(NumericCondExpr(LogicalBinOpExpr(sept::True, Xor, sept::False), 5.5, 10.25)) << '\n'
        << '\n';
    std::cout
        << LVD_REFLECT(evaluate_expr(NumericCondExpr(true, 5.5, 10.25))) << '\n'
        << LVD_REFLECT(evaluate_expr(NumericCondExpr(false, 5.5, 10.25))) << '\n'
        << LVD_REFLECT(evaluate_expr(NumericCondExpr(LogicalBinOpExpr(sept::True, Xor, sept::False), 5.5, 10.25))) << '\n'
        << '\n';

    return 0;
}
