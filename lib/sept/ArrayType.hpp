// 2020.03.13 - Victor Dods

#pragma once

#include <cassert>
#include <lvd/OstreamDelegate.hpp>
#include "sept/core.hpp"
#include "sept/Data.hpp"
#include "sept/Data_t.hpp"
#include "sept/DataVector.hpp"
#include "sept/hash.hpp"
#include "sept/NPType.hpp"
#include <vector>

namespace sept {

class ArrayTerm_c;

template <typename T_> struct is_an_array_type : std::false_type { };

template <typename T_> inline constexpr bool is_an_array_type_v = is_an_array_type<T_>::value;

// TODO: Make this inherit ArrayETerm_c and ArraySTerm_c
class ArrayESTerm_c {
public:

    explicit ArrayESTerm_c (Data const &element_type, size_t size)
        :   m_element_type(element_type)
        ,   m_size(size)
    {
        // NOTE: Commented out because otherwise it's problematic constructing a global instance of
        // ArrayESTerm_c that depends on another global Data instance.
//         if (!m_element_type.has_value())
//             throw std::runtime_error("can't construct ArrayESTerm_c with no value");
    }
    explicit ArrayESTerm_c (Data &&element_type, size_t size)
        :   m_element_type(std::move(element_type))
        ,   m_size(size)
    {
        // NOTE: Commented out because otherwise it's problematic constructing a global instance of
        // ArrayESTerm_c that depends on another global Data instance.
//         if (!m_element_type.has_value())
//             throw std::runtime_error("can't construct ArrayESTerm_c with no value");
    }

    // Constructs ArrayTerm_c (abstractly, a Term of type Array).
    template <typename... Args_>
    ArrayTerm_c operator () (Args_&&... args) const;

    bool operator == (Data const &other) const { return other.can_cast<ArrayESTerm_c>() && *this == other.cast<ArrayESTerm_c const &>(); }
    bool operator == (ArrayESTerm_c const &other) const { return m_element_type == other.m_element_type && m_size == other.m_size; }

    Data const &element_type () const & { return m_element_type; }
    Data element_type () && { return std::move(m_element_type); }
    size_t size () const { return m_size; }

    bool constraint_is_satisfied (DataVector const &elements) const;
    void verify_constraint_or_throw (DataVector const &elements) const;

    operator lvd::OstreamDelegate () const {
        return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
            out << "ArrayES(" << element_type() << ',' << size() << ')';
        });
    }

private:

    Data m_element_type;
    size_t m_size;
};

class ArrayETerm_c {
public:

    explicit ArrayETerm_c (Data const &element_type)
        :   m_element_type(element_type)
    {
        // NOTE: Commented out because otherwise it's problematic constructing a global instance of
        // ArrayETerm_c that depends on another global Data instance.
//         if (!m_element_type.has_value())
//             throw std::runtime_error("can't construct ArrayETerm_c with no value");
    }
    explicit ArrayETerm_c (Data &&element_type)
        :   m_element_type(std::move(element_type))
    {
        // NOTE: Commented out because otherwise it's problematic constructing a global instance of
        // ArrayETerm_c that depends on another global Data instance.
//         if (!m_element_type.has_value())
//             throw std::runtime_error("can't construct ArrayETerm_c with no value");
    }

    // Constructs ArrayTerm_c (abstractly, a Term of type Array).
    template <typename... Args_>
    ArrayTerm_c operator () (Args_&&... args) const;

    bool operator == (Data const &other) const { return other.can_cast<ArrayETerm_c>() && *this == other.cast<ArrayETerm_c const &>(); }
    bool operator == (ArrayETerm_c const &other) const { return m_element_type == other.m_element_type; }

    Data const &element_type () const & { return m_element_type; }
    Data element_type () && { return std::move(m_element_type); }

    bool constraint_is_satisfied (DataVector const &elements) const;
    void verify_constraint_or_throw (DataVector const &elements) const;

    operator lvd::OstreamDelegate () const {
        return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
            out << "ArrayE(" << element_type() << ')';
        });
    }

private:

    Data m_element_type;
};

class ArraySTerm_c {
public:

    constexpr explicit ArraySTerm_c (size_t size) : m_size(size) { }
    ArraySTerm_c (ArraySTerm_c const &) = default;
    ArraySTerm_c (ArraySTerm_c &&) = default;

    // Constructs ArrayTerm_c (abstractly, a Term of type Array).
    template <typename... Args_>
    ArrayTerm_c operator () (Args_&&... args) const;

    bool operator == (Data const &other) const { return other.can_cast<ArraySTerm_c>() && *this == other.cast<ArraySTerm_c const &>(); }
    bool operator == (ArraySTerm_c const &other) const { return m_size == other.m_size; }

    // There is no constraint on the element type, so the element type is Term (i.e. Any).
    Term_c const &element_type () const { return Term; }
    size_t size () const { return m_size; }

    bool constraint_is_satisfied (DataVector const &elements) const;
    void verify_constraint_or_throw (DataVector const &elements) const;

    operator lvd::OstreamDelegate () const {
        return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
            out << "ArrayS(" << size() << ')';
        });
    }

private:

    size_t m_size;
};

class ArrayTerm_c;

class Array_c : public NonParametricType_t<NPTerm::ARRAY,ArrayTerm_c,Array_c> {
public:

    // Constructs ArrayTerm_c (abtractly, a Term of type Array).
    template <typename... Args_>
    ArrayTerm_c operator () (Args_&&... args) const;

    // There is no constraint on the element type, so the element type is Term (i.e. Any).
    Term_c const &element_type () const { return Term; }

    bool constraint_is_satisfied (DataVector const &elements) const { return true; } // No constraints.
    void verify_constraint_or_throw (DataVector const &elements) const { } // No constraints.
};

using ArrayType_c = NonParametricType_t<NPTerm::ARRAY_TYPE>;
// ArrayES_c, ArrayE_c, and ArrayS_c really just provide a syntactic way to access their respective term constructors.
using ArrayES_c = NonParametricType_t<NPTerm::ARRAY_ES, ArrayESTerm_c>;
using ArrayE_c = NonParametricType_t<NPTerm::ARRAY_E, ArrayETerm_c>;
using ArrayS_c = NonParametricType_t<NPTerm::ARRAY_S, ArraySTerm_c>;

extern ArrayType_c ArrayType;
extern ArrayES_c ArrayES;
extern ArrayE_c ArrayE;
extern ArrayS_c ArrayS;
extern Array_c Array;

inline constexpr NonParametricType_c const &abstract_type_of (ArrayType_c const &) { return NonParametricType; }
inline constexpr ArrayType_c const &abstract_type_of (ArrayES_c const &) { return ArrayType; }
inline constexpr ArrayType_c const &abstract_type_of (ArrayE_c const &) { return ArrayType; }
inline constexpr ArrayType_c const &abstract_type_of (ArrayS_c const &) { return ArrayType; }
inline constexpr ArrayType_c const &abstract_type_of (Array_c const &) { return ArrayType; }

template <> struct is_an_array_type<ArrayESTerm_c> : public std::true_type { };
template <> struct is_an_array_type<ArrayETerm_c> : public std::true_type { };
template <> struct is_an_array_type<ArraySTerm_c> : public std::true_type { };
template <> struct is_an_array_type<Array_c> : public std::true_type { };

bool inhabits (ArrayTerm_c const &a, ArrayESTerm_c const &t);
bool inhabits (ArrayTerm_c const &a, ArrayETerm_c const &t);
bool inhabits (ArrayTerm_c const &a, ArraySTerm_c const &t);
bool inhabits (ArrayTerm_c const &a, Array_c const &t);

template <typename ArrayType_, typename = std::enable_if_t<is_an_array_type_v<ArrayType_>>>
bool inhabits (Data_t<ArrayTerm_c> const &a, ArrayType_ const &t) {
    return inhabits(a.value(), t);
}

bool inhabits (Data const &value, ArrayESTerm_c const &t);
bool inhabits (Data const &value, ArrayETerm_c const &t);
bool inhabits (Data const &value, ArraySTerm_c const &t);
bool inhabits (Data const &value, Array_c const &t);

void serialize (ArrayESTerm_c const &v, std::ostream &out);
void serialize (ArrayETerm_c const &v, std::ostream &out);
void serialize (ArraySTerm_c const &v, std::ostream &out);

// These assume that the abstract_type portion following the SerializedTopLevelCode::PARAMETRIC_TERM
// has already been read in; that value is passed in as abstract_type.
ArrayESTerm_c deserialize_value_ArrayESTerm (Data &&abstract_type, std::istream &in);
ArrayETerm_c deserialize_value_ArrayETerm (Data &&abstract_type, std::istream &in);
ArraySTerm_c deserialize_value_ArraySTerm (Data &&abstract_type, std::istream &in);

inline constexpr ArrayES_c const &abstract_type_of (ArrayESTerm_c const &) { return ArrayES; }
inline constexpr ArrayE_c const &abstract_type_of (ArrayETerm_c const &) { return ArrayE; }
inline constexpr ArrayS_c const &abstract_type_of (ArraySTerm_c const &) { return ArrayS; }

inline Data const &element_type_of (ArrayESTerm_c const &array_type) { return array_type.element_type(); }
inline Data const &element_type_of (ArrayETerm_c const &array_type) { return array_type.element_type(); }
inline Term_c const &element_type_of (ArraySTerm_c const &array_type) { return array_type.element_type(); }
inline Term_c const &element_type_of (Array_c const &array_type) { return array_type.element_type(); }

inline Data element_type_of (ArrayESTerm_c &&array_type) { return std::move(array_type).element_type(); }
inline Data element_type_of (ArrayETerm_c &&array_type) { return std::move(array_type).element_type(); }

inline int compare (ArrayESTerm_c const &lhs, ArrayESTerm_c const &rhs) {
    auto c = compare_data(lhs.element_type(), rhs.element_type());
    if (c != 0)
        return c;

    return lhs.size() < rhs.size() ? -1 : (lhs.size() == rhs.size() ? 0 : 1);
}

inline int compare (ArrayETerm_c const &lhs, ArrayETerm_c const &rhs) {
    return compare_data(lhs.element_type(), rhs.element_type());
}

inline int compare (ArraySTerm_c const &lhs, ArraySTerm_c const &rhs) {
    return lhs.size() < rhs.size() ? -1 : (lhs.size() == rhs.size() ? 0 : 1);
}

} // end namespace sept

namespace std {

// Template specialization(s) to define std::hash for the Array types.
// This is generally done by combining the hashes of the typeid of the argument and its content.

template <>
struct hash<sept::ArrayESTerm_c> {
    size_t operator () (sept::ArrayESTerm_c const &t) const {
        return sept::hash(typeid(sept::ArrayESTerm_c), t.element_type(), t.size());
    }
};

template <>
struct hash<sept::ArrayETerm_c> {
    size_t operator () (sept::ArrayETerm_c const &t) const {
        return sept::hash(typeid(sept::ArrayETerm_c), t.element_type());
    }
};

template <>
struct hash<sept::ArraySTerm_c> {
    size_t operator () (sept::ArraySTerm_c const &t) const {
        return sept::hash(typeid(sept::ArraySTerm_c), t.size());
    }
};

template <>
struct hash<sept::Array_c> {
    size_t operator () (sept::Array_c const &t) const {
        return sept::hash(typeid(sept::Array_c));
    }
};

} // end namespace std
