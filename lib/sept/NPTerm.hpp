// 2020.04.16 - Victor Dods

#pragma once

#include <lvd/static_if.hpp>
#include "sept/core.hpp"
#include "sept/Data.hpp"
#include "sept/hash.hpp"
#include <type_traits>

namespace sept {

using NPTermRepr = uint8_t;

// NPTerm is "NonParametricType", meaning types requiring no parameters to instantiate.
// Each ParametricType corresponds to a class (e.g. ArrayESTerm_c) or built-in type (e.g. int32_t).
enum class NPTerm : NPTermRepr {
    // The most basic Types.
    TERM = 0, // Literally everything is a Term (this could be called Any)
    NON_PARAMETRIC_TERM, // Inhabitants are Terms requiring no parameters to instantiate (all members of the NPTerm enum)
    PARAMETRIC_TERM, // Inhabitants are Terms requiring parameters to instantiate, e.g. 10.25, 'x', 999000

    // Type Types.
    TYPE, // A Term which has an inhabitation predicate.
    NON_TYPE, // A Term which is not a Type (this could be called Value)
    NON_PARAMETRIC_TYPE, // Inhabitants are types requiring no parameters to instantiate (all members of the NPType enum)
    PARAMETRIC_TYPE, // Inhabitants are types requiring parameters to instantiate, e.g. ArrayESTerm_c(T,N)

    // NonParametricTerm && NonType
    VOID, // Void is a NonType that conveys no information
    TRUE, // The truthier of the two inhabitants of Bool
    FALSE, // The lying inhabitant of Bool

    // A few natural Types.
    VOID_TYPE, // Sole inhabitant is Void
    TRUE_TYPE, // Sole inhabitant is True
    FALSE_TYPE, // Sole inhabitant is False
    EMPTY_TYPE, // A Type defined to have no inhabitants

    // POD Types
    BOOL, // Isomorphic to Union(TrueType,FalseType)
    SINT8,
    SINT16,
    SINT32,
    SINT64,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    FLOAT32,
    FLOAT64,
//     ASCII_CHAR, // TODO: Add UnicodeChar later and whatever else -- TODO: Maybe Ascii should be an abstract type

    // POD Type Types
    BOOL_TYPE, // Sole inhabitant is Bool
    SINT8_TYPE, // Sole inhabitant is Sint8
    SINT16_TYPE, // Sole inhabitant is Sint16
    SINT32_TYPE, // Sole inhabitant is Sint32
    SINT64_TYPE, // Sole inhabitant is Sint64
    UINT8_TYPE, // Sole inhabitant is Uint8
    UINT16_TYPE, // Sole inhabitant is Uint16
    UINT32_TYPE, // Sole inhabitant is Uint32
    UINT64_TYPE, // Sole inhabitant is Uint64
    FLOAT32_TYPE, // Sole inhabitant is Float32
    FLOAT64_TYPE, // Sole inhabitant is Float64
//     ASCII_CHAR_TYPE, // Sole inhabitant is AsciiChar

    // Other Types related to POD Types
    SINT_TYPE, // Isomorphic to Union(Sint8Type,Sint16Type,Sint32Type,Sint64Type)
    SINT, // Isomorphic to Union(Sint8,Sint16,Sint32,Sint64)
    UINT_TYPE, // Isomorphic to Union(Uint8Type,Uint16Type,Uint32Type,Uint64Type)
    UINT, // Isomorphic to Union(Uint8,Uint16,Uint32,Uint64)
    FLOAT_TYPE, // Isomorphic to Union(Float32Type,Float64Type)
    FLOAT, // Isomorphic to Union(Float32,Float64)
    // TODO: Add CHAR types
    POD_TYPE, // Isomorphic to Union(BoolType,SintType,UintType,FloatType) (TODO: Somehow add POD as an inhabitant)
    POD, // Isomorphic to Union(Bool,Sint,Uint,Float).  Inhabitants are POD values.  POD : PODType.
    // TODO: Add stuff like Positive, Negative, NonPositive, NonNegative, Zero

    UNION, // Inhabitants have the form Union(T1,...,TN)
    INTERSECTION, // Inhabitants have the form Intersection(T1,...,TN)
    NEGATION, // Inhabitants have the form Negation(T)
    DIFFERENCE, // Inhabitants have the form Difference(T,U1,...,UN)

    // TODO: UnionType, IntersectionType, etc.

    ARRAY_TYPE, // Inhabitants are ArrayES, ArrayE, ArrayS, Array.
    ARRAY_ES, // Inhabitants have the form ArrayES(T,N) -- implemented by class ArrayESTerm_c
    ARRAY_E, // Inhabitants have the form ArrayE(T) -- implemented by class ArrayETerm_c
    ARRAY_S, // Inhabitants have the form ArrayS(N) -- implemented by class ArraySTerm_c
    ARRAY, // Inhabitants have the form Array(...) -- implemented by class ArrayTerm_c

    ORDERED_MAP_TYPE, // Inhabitants are OrderedMapDC, OrderedMapD, OrderedMapC, OrderedMap.
    ORDERED_MAP_DC, // Inhabitants have the form OrderedMapDC(Domain,Codomain) -- implemented by class OrderedMapDCTerm_c
    ORDERED_MAP_D, // Inhabitants have the form OrderedMapD(Domain) -- implemented by class OrderedMapDTerm_c
    ORDERED_MAP_C, // Inhabitants have the form OrderedMapC(Codomain) -- implemented by class OrderedMapCTerm_c
    ORDERED_MAP, // Inhabitants have the form OrderedMap(...) -- implemented by class OrderedMapTerm_c

    TUPLE_TYPE, // Inhabitant is Tuple.
    TUPLE, // Inhabitants have the form Tuple(...) -- implemented by class TupleTerm_c.

    PLACEHOLDER_TYPE,
    PLACEHOLDER,

    //
    // Control terms
    //

    OUTPUT_TYPE, // Sole inhabitant is Output
    OUTPUT, // Inhabitants have the form Output(V) for some value V
    CLEAR_OUTPUT_TYPE, // Sole inhabitant is ClearOutput
    CLEAR_OUTPUT, // Singleton
    END_OF_FILE_TYPE, // Sole inhabitant is EndOfFile
    END_OF_FILE, // Singleton
    REQUEST_SYNC_INPUT_TYPE, // Sole inhabitant is RequestSyncInput
    REQUEST_SYNC_INPUT, // Inhabitants have the form RequestSyncInput(T) for some type T

    __HIGHEST__ = REQUEST_SYNC_INPUT
};

std::string const &as_string (NPTerm t);

inline void serialize (NPTerm const &v, std::ostream &out) {
    static_assert(sizeof(NPTerm) == 1);
    out.put(uint8_t(v));
}

class Data;

template <typename Derived_>
class SingletonBase_t {
public:

    // TODO: Somehow check that Derived_ is a trivial class.  Can't put that check in the constructor, because then
    // it becomes a non-trivial class.  Can't put a static_assert in the body of this class because Derived_ isn't
    // fully defined yet.
//     static_assert(std::is_trivial_v<Derived_>, "Derived_ must be a trivial class (see C++ reference for std::is_trivial)");

    // Because there is no information in the singleton object (all its information is present
    // in its type), just need to compare the types.
    bool operator == (Data const &rhs) { return rhs.type() == typeid(Derived_); }
    bool operator != (Data const &rhs) { return rhs.type() != typeid(Derived_); }
};

template <typename Derived_>
std::ostream &operator << (std::ostream &out, SingletonBase_t<Derived_> const &) {
    return out << Derived_::as_string();
}

template <NPTerm ENUM_VALUE_, typename Derived_ = DerivedNone>
class NonParametricTerm_t : public SingletonBase_t<lvd::static_if_t<std::is_same_v<Derived_,DerivedNone>,NonParametricTerm_t<ENUM_VALUE_>,Derived_>> {
public:

    using Derived = lvd::static_if_t<std::is_same_v<Derived_,DerivedNone>,NonParametricTerm_t,Derived_>;

    constexpr NPTerm npterm () const { return ENUM_VALUE_; }

    Derived const & as_derived () const & { return static_cast<Derived const &>(*this); }
    Derived &as_derived () & { return static_cast<Derived &>(*this); }
    operator Derived const & () const & { return as_derived(); }
    operator Derived & () & { return as_derived(); }

    static std::string const &as_string () { return sept::as_string(ENUM_VALUE_); }
};

using NonParametricTerm_c = NonParametricTerm_t<NPTerm::NON_PARAMETRIC_TERM>;
// NOTE: The definition of abstract_type_of(NonParametricTerm_c const &) is in NPType.hpp

extern NonParametricTerm_c NonParametricTerm;

template <NPTerm ENUM_VALUE_, typename Derived_>
constexpr NonParametricTerm_c const &concrete_type_of (NonParametricTerm_t<ENUM_VALUE_,Derived_> const &) {
    return NonParametricTerm;
}

// Void_c implements Void, which is the sole inhabitant of VoidType.
using Void_c = NonParametricTerm_t<NPTerm::VOID>;

extern Void_c Void;

class True_c;
class False_c;

// BoolTerm_c implements the terms of Bool, which can be true or false.  Note that True and False
// are not instances of BoolTerm_c in a concrete sense, but they are inhabitants of Bool.  However,
// all the expected constructors, conversion operators, and comparators exist.
// NOTE: This is a ParametricTerm
class BoolTerm_c {
public:

    BoolTerm_c () = delete; // Maybe allow this?  Maybe don't bother initializing?
    constexpr BoolTerm_c (BoolTerm_c const &other) : m_value(other.m_value) { }
    constexpr BoolTerm_c (BoolTerm_c &&other) : m_value(other.m_value) { }
    constexpr BoolTerm_c (bool value) : m_value(value) { }
    constexpr explicit BoolTerm_c (True_c const &) : m_value(true) { }
    constexpr explicit BoolTerm_c (False_c const &) : m_value(false) { }

    constexpr bool as_bool () const { return m_value; }
    constexpr operator bool () const { return m_value; }

private:

    bool m_value;
};

inline constexpr BoolTerm_c operator == (BoolTerm_c const &lhs, BoolTerm_c const &rhs) {
    return lhs.as_bool() == rhs.as_bool();
}

inline constexpr BoolTerm_c operator != (BoolTerm_c const &lhs, BoolTerm_c const &rhs) {
    return lhs.as_bool() != rhs.as_bool();
}

inline constexpr BoolTerm_c operator == (bool lhs, BoolTerm_c const &rhs) {
    return lhs == rhs.as_bool();
}

inline constexpr BoolTerm_c operator != (bool lhs, BoolTerm_c const &rhs) {
    return lhs != rhs.as_bool();
}

inline constexpr BoolTerm_c operator == (BoolTerm_c const &lhs, bool rhs) {
    return lhs.as_bool() == rhs;
}

inline constexpr BoolTerm_c operator != (BoolTerm_c const &lhs, bool rhs) {
    return lhs.as_bool() != rhs;
}

// TODO: Maybe implement bitwise operators for logic (shouldn't overload logical operators due
// to short-circuiting behavior that is lost when overloading the logical operators)

template <typename T_, typename = std::enable_if_t<std::is_integral_v<T_> || std::is_floating_point_v<T_>>>
constexpr int compare (T_ lhs, T_ rhs) {
    return lhs < rhs ? -1 : (lhs == rhs ? 0 : 1);
}

inline constexpr int compare (BoolTerm_c const &lhs, BoolTerm_c const &rhs) {
    return compare(lhs.as_bool(), rhs.as_bool());
}

class True_c : public NonParametricTerm_t<NPTerm::TRUE,True_c> {
public:

    constexpr bool as_bool () const { return true; }
    constexpr BoolTerm_c as_BoolTerm () const { return BoolTerm_c(true); }
    constexpr operator bool () const { return true; }
//     constexpr operator BoolTerm_c () const { return BoolTerm_c(true); }
};

class False_c : public NonParametricTerm_t<NPTerm::FALSE,False_c> {
public:

    constexpr bool as_bool () const { return false; }
    constexpr BoolTerm_c as_BoolTerm () const { return BoolTerm_c(false); }
    constexpr operator bool () const { return false; }
//     constexpr operator BoolTerm_c () const { return BoolTerm_c(false); }
};

extern True_c True;
extern False_c False;

inline constexpr False_c operator == (True_c const &, False_c const &) {
    return False;
}

inline constexpr False_c operator == (False_c const &, True_c const &) {
    return False;
}

inline constexpr True_c operator == (True_c const &, True_c const &) {
    return True;
}

inline constexpr True_c operator == (False_c const &, False_c const &) {
    return True;
}

inline constexpr True_c operator != (True_c const &, False_c const &) {
    return True;
}

inline constexpr True_c operator != (False_c const &, True_c const &) {
    return True;
}

inline constexpr False_c operator != (True_c const &, True_c const &) {
    return False;
}

inline constexpr False_c operator != (False_c const &, False_c const &) {
    return False;
}

inline constexpr BoolTerm_c operator == (BoolTerm_c const &lhs, True_c const &) {
    return lhs.as_bool() == true;
}

inline constexpr BoolTerm_c operator == (BoolTerm_c const &lhs, False_c const &) {
    return lhs.as_bool() == false;
}

inline constexpr BoolTerm_c operator == (True_c const &, BoolTerm_c const &rhs) {
    return rhs.as_bool() == true;
}

inline constexpr BoolTerm_c operator == (False_c const &, BoolTerm_c const &rhs) {
    return rhs.as_bool() == false;
}

inline constexpr BoolTerm_c operator != (BoolTerm_c const &lhs, True_c const &) {
    return lhs.as_bool() != true;
}

inline constexpr BoolTerm_c operator != (BoolTerm_c const &lhs, False_c const &) {
    return lhs.as_bool() != false;
}

inline constexpr BoolTerm_c operator != (True_c const &, BoolTerm_c const &rhs) {
    return rhs.as_bool() != true;
}

inline constexpr BoolTerm_c operator != (False_c const &, BoolTerm_c const &rhs) {
    return rhs.as_bool() != false;
}

// TODO: Maybe implement bitwise operators for logic (shouldn't overload logical operators due
// to short-circuiting behavior that is lost when overloading the logical operators)

template <typename LhsDerived_, typename RhsDerived_>
constexpr lvd::static_if_t<std::is_same_v<LhsDerived_,RhsDerived_>,True_c,False_c> operator == (SingletonBase_t<LhsDerived_> const &, SingletonBase_t<RhsDerived_> const &) {
    // Because there is no information in the singleton object (all its information is present
    // in its type), just need to compare the types.
    return lvd::static_if_t<std::is_same_v<LhsDerived_,RhsDerived_>,True_c,False_c>();
}

template <typename LhsDerived_, typename RhsDerived_>
constexpr lvd::static_if_t<std::is_same_v<LhsDerived_,RhsDerived_>,False_c,True_c> operator != (SingletonBase_t<LhsDerived_> const &lhs, SingletonBase_t<RhsDerived_> const &rhs) {
    // Because there is no information in the singleton object (all its information is present
    // in its type), just need to compare the types.
    return lvd::static_if_t<std::is_same_v<LhsDerived_,RhsDerived_>,False_c,True_c>();
}

template <typename Derived_>
constexpr int compare (SingletonBase_t<Derived_> const &, SingletonBase_t<Derived_> const &) {
    // Because there is no information in the singleton object (all its information is present
    // in its type), just need to compare the types.
    return 0;
}

// TODO: Use names to define comparison on differently-typed singletons.

inline constexpr int compare (BoolTerm_c const &lhs, True_c const &rhs) {
    return compare(lhs.as_bool(), rhs.as_bool());
}

inline constexpr int compare (BoolTerm_c const &lhs, False_c const &rhs) {
    return compare(lhs.as_bool(), rhs.as_bool());
}

inline constexpr int compare (True_c const &lhs, BoolTerm_c const &rhs) {
    return compare(lhs.as_bool(), rhs.as_bool());
}

inline constexpr int compare (False_c const &lhs, BoolTerm_c const &rhs) {
    return compare(lhs.as_bool(), rhs.as_bool());
}

inline constexpr int compare (True_c const &lhs, False_c const &rhs) {
    return compare(lhs.as_bool(), rhs.as_bool());
}

inline constexpr int compare (False_c const &lhs, True_c const &rhs) {
    return compare(lhs.as_bool(), rhs.as_bool());
}

//
// serialize and deserialize definitions
//

using SerializedTopLevelCodeRepr = uint8_t;

// TODO: Could put a lot more into here, in particular commonly-used values/types.
enum class SerializedTopLevelCode : SerializedTopLevelCodeRepr {
    NON_PARAMETRIC_TERM = 0,    // A term having no parameters, and therefore fixed size.
    PARAMETRIC_TERM,            // A term that has parameters.
};

inline void serialize (SerializedTopLevelCode const &v, std::ostream &out) {
    static_assert(sizeof(SerializedTopLevelCode) == 1);
    out.put(uint8_t(v));
}

template <NPTerm ENUM_VALUE_, typename Derived_>
void serialize (NonParametricTerm_t<ENUM_VALUE_,Derived_> const &value, std::ostream &out) {
    serialize(SerializedTopLevelCode::NON_PARAMETRIC_TERM, out);
    serialize(ENUM_VALUE_, out);
}

} // end namespace sept

namespace std {

// Template specialization to define std::hash<sept::SingletonBase_t<...>>.
template <typename Derived_>
struct hash<sept::SingletonBase_t<Derived_>> {
    size_t operator () (sept::SingletonBase_t<Derived_> const &) const {
        return sept::hash(typeid(Derived_));
    }
};

// Template specialization to define std::hash<sept::NonParametricTerm_t<...>>.
template <sept::NPTerm ENUM_VALUE_, typename Derived_>
struct hash<sept::NonParametricTerm_t<ENUM_VALUE_,Derived_>> {
    size_t operator () (sept::NonParametricTerm_t<ENUM_VALUE_,Derived_> const &) const {
        return sept::hash(typeid(Derived_), ENUM_VALUE_);
    }
};

// Template specialization to define std::hash<sept::True_c>.
template <>
struct hash<sept::True_c> {
    size_t operator () (sept::True_c const &) const {
        return sept::hash(typeid(sept::True_c));
    }
};

// Template specialization to define std::hash<sept::False_c>.
template <>
struct hash<sept::False_c> {
    size_t operator () (sept::False_c const &) const {
        return sept::hash(typeid(sept::False_c));
    }
};

} // end namespace std
