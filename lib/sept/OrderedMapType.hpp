// 2020.07.15 - Victor Dods

#pragma once

#include <cassert>
#include <lvd/OstreamDelegate.hpp>
#include <map>
#include "sept/core.hpp"
#include "sept/Data.hpp"
#include "sept/Data_t.hpp"
#include "sept/hash.hpp"
#include "sept/NPType.hpp"

namespace sept {

class OrderedMapTerm_c;

template <typename T_> struct is_an_ordered_map_type : std::false_type { };

template <typename T_> inline constexpr bool is_an_ordered_map_type_v = is_an_ordered_map_type<T_>::value;

struct DataOrder {
    bool operator () (Data const &lhs, Data const &rhs) const {
        return compare_data(lhs, rhs) < 0;
    }
};

using DataOrderedMap = std::map<Data,Data,DataOrder>;

class OrderedMapDCTerm_c {
public:

    explicit OrderedMapDCTerm_c (Data const &domain, Data const &codomain)
    :   m_domain(domain)
    ,   m_codomain(codomain) {
        if (!m_domain.has_value())
            throw std::runtime_error("can't construct OrderedMapDCTerm_c with no domain");
        if (!m_codomain.has_value())
            throw std::runtime_error("can't construct OrderedMapDCTerm_c with no codomain");
    }
    explicit OrderedMapDCTerm_c (Data &&domain, Data &&codomain)
    :   m_domain(std::move(domain))
    ,   m_codomain(std::move(codomain)) {
        if (!m_domain.has_value())
            throw std::runtime_error("can't construct OrderedMapDCTerm_c with no domain");
        if (!m_codomain.has_value())
            throw std::runtime_error("can't construct OrderedMapDCTerm_c with no codomain");
    }

    // Constructs OrderedMapTerm_c (abstractly, a Term of type OrderedMap).
    template <typename... Args_>
    OrderedMapTerm_c operator () (Args_&&... args) const;

    bool operator == (Data const &other) const { return other.can_cast<OrderedMapDCTerm_c>() && *this == other.cast<OrderedMapDCTerm_c const &>(); }
    bool operator == (OrderedMapDCTerm_c const &other) const { return m_domain == other.m_domain && m_codomain == other.m_codomain; }

    Data const &domain () const & { return m_domain; }
    Data domain () && { return std::move(m_domain); }
    Data const &codomain () const & { return m_codomain; }
    Data codomain () && { return std::move(m_codomain); }

    bool constraint_is_satisfied (DataOrderedMap const &m) const;
    void verify_constraint_or_throw (DataOrderedMap const &m) const;

    operator lvd::OstreamDelegate () const {
        return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
            out << "OrderedMapDC(" << domain() << ',' << codomain() << ')';
        });
    }

private:

    Data m_domain;
    Data m_codomain;
};

class OrderedMapDTerm_c {
public:

    explicit OrderedMapDTerm_c (Data const &domain)
    :   m_domain(domain) {
        if (!m_domain.has_value())
            throw std::runtime_error("can't construct OrderedMapDTerm_c with no domain");
    }
    explicit OrderedMapDTerm_c (Data &&domain)
    :   m_domain(std::move(domain)) {
        if (!m_domain.has_value())
            throw std::runtime_error("can't construct OrderedMapDTerm_c with no domain");
    }

    // Constructs OrderedMapTerm_c (abstractly, a Term of type OrderedMap).
    template <typename... Args_>
    OrderedMapTerm_c operator () (Args_&&... args) const;

    bool operator == (Data const &other) const { return other.can_cast<OrderedMapDTerm_c>() && *this == other.cast<OrderedMapDTerm_c const &>(); }
    bool operator == (OrderedMapDTerm_c const &other) const { return m_domain == other.m_domain; }

    Data const &domain () const & { return m_domain; }
    Data domain () && { return std::move(m_domain); }
    // There is no constraint on the codomain type, so the codomain type is Term (i.e. Any).
    Term_c const &codomain () const { return Term; }

    bool constraint_is_satisfied (DataOrderedMap const &m) const;
    void verify_constraint_or_throw (DataOrderedMap const &m) const;

    operator lvd::OstreamDelegate () const {
        return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
            out << "OrderedMapD(" << domain() << ')';
        });
    }

private:

    Data m_domain;
};

class OrderedMapCTerm_c {
public:

    explicit OrderedMapCTerm_c (Data const &codomain)
    :   m_codomain(codomain) {
        if (!m_codomain.has_value())
            throw std::runtime_error("can't construct OrderedMapCTerm_c with no codomain");
    }
    explicit OrderedMapCTerm_c (Data &&codomain)
    :   m_codomain(std::move(codomain)) {
        if (!m_codomain.has_value())
            throw std::runtime_error("can't construct OrderedMapCTerm_c with no codomain");
    }

    // Constructs OrderedMapTerm_c (abstractly, a Term of type OrderedMap).
    template <typename... Args_>
    OrderedMapTerm_c operator () (Args_&&... args) const;

    bool operator == (Data const &other) const { return other.can_cast<OrderedMapCTerm_c>() && *this == other.cast<OrderedMapCTerm_c const &>(); }
    bool operator == (OrderedMapCTerm_c const &other) const { return m_codomain == other.m_codomain; }

    // There is no constraint on the domain type, so the domain type is Term (i.e. Any).
    Term_c const &domain () const { return Term; }
    Data const &codomain () const & { return m_codomain; }
    Data codomain () && { return std::move(m_codomain); }

    bool constraint_is_satisfied (DataOrderedMap const &m) const;
    void verify_constraint_or_throw (DataOrderedMap const &m) const;

    operator lvd::OstreamDelegate () const {
        return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
            out << "OrderedMapC(" << codomain() << ')';
        });
    }

private:

    Data m_codomain;
};

class OrderedMapTerm_c;

class OrderedMap_c : public NonParametricType_t<NPTerm::ORDERED_MAP,OrderedMapTerm_c,OrderedMap_c> {
public:

    // Constructs OrderedMapTerm_c (abtractly, a Term of type OrderedMap).
    template <typename... Args_>
    OrderedMapTerm_c operator () (Args_&&... args) const;

    // There is no constraint on the domain type, so the domain type is Term (i.e. Any).
    Term_c const &domain () const { return Term; }
    // There is no constraint on the codomain type, so the codomain type is Term (i.e. Any).
    Term_c const &codomain () const { return Term; }

    bool constraint_is_satisfied (DataOrderedMap const &m) const { return true; } // No constraints.
    void verify_constraint_or_throw (DataOrderedMap const &m) const { } // No constraints.
};

using OrderedMapType_c = NonParametricType_t<NPTerm::ORDERED_MAP_TYPE>;
// OrderedMapDC_c, OrderedMapD_c, and OrderedMapC_c really just provide a syntactic way to access their respective term constructors.
using OrderedMapDC_c = NonParametricType_t<NPTerm::ORDERED_MAP_DC, OrderedMapDCTerm_c>;
using OrderedMapD_c = NonParametricType_t<NPTerm::ORDERED_MAP_D, OrderedMapDTerm_c>;
using OrderedMapC_c = NonParametricType_t<NPTerm::ORDERED_MAP_C, OrderedMapCTerm_c>;

extern OrderedMapType_c OrderedMapType;
extern OrderedMapDC_c OrderedMapDC;
extern OrderedMapD_c OrderedMapD;
extern OrderedMapC_c OrderedMapC;
extern OrderedMap_c OrderedMap;

inline constexpr NonParametricType_c const &abstract_type_of (OrderedMapType_c const &) { return NonParametricType; }
inline constexpr OrderedMapType_c const &abstract_type_of (OrderedMapDC_c const &) { return OrderedMapType; }
inline constexpr OrderedMapType_c const &abstract_type_of (OrderedMapD_c const &) { return OrderedMapType; }
inline constexpr OrderedMapType_c const &abstract_type_of (OrderedMapC_c const &) { return OrderedMapType; }
inline constexpr OrderedMapType_c const &abstract_type_of (OrderedMap_c const &) { return OrderedMapType; }

template <> struct is_an_ordered_map_type<OrderedMapDCTerm_c> : public std::true_type { };
template <> struct is_an_ordered_map_type<OrderedMapDTerm_c> : public std::true_type { };
template <> struct is_an_ordered_map_type<OrderedMapCTerm_c> : public std::true_type { };
template <> struct is_an_ordered_map_type<OrderedMap_c> : public std::true_type { };

bool inhabits (OrderedMapTerm_c const &a, OrderedMapDCTerm_c const &t);
bool inhabits (OrderedMapTerm_c const &a, OrderedMapDTerm_c const &t);
bool inhabits (OrderedMapTerm_c const &a, OrderedMapCTerm_c const &t);
bool inhabits (OrderedMapTerm_c const &a, OrderedMap_c const &t);

template <typename OrderedMapType_, typename = std::enable_if_t<is_an_ordered_map_type_v<OrderedMapType_>>>
bool inhabits (Data_t<OrderedMapTerm_c> const &m, OrderedMapType_ const &t) {
    return inhabits(m.value(), t);
}

bool inhabits (Data const &value, OrderedMapDCTerm_c const &t);
bool inhabits (Data const &value, OrderedMapDTerm_c const &t);
bool inhabits (Data const &value, OrderedMapCTerm_c const &t);
bool inhabits (Data const &value, OrderedMap_c const &t);

void serialize (OrderedMapDCTerm_c const &v, std::ostream &out);
void serialize (OrderedMapDTerm_c const &v, std::ostream &out);
void serialize (OrderedMapCTerm_c const &v, std::ostream &out);

// These assume that the abstract_type portion following the SerializedTopLevelCode::PARAMETRIC_TERM
// has already been read in; that value is passed in as abstract_type.
OrderedMapDCTerm_c deserialize_value_OrderedMapDCTerm (Data &&abstract_type, std::istream &in);
OrderedMapDTerm_c deserialize_value_OrderedMapDTerm (Data &&abstract_type, std::istream &in);
OrderedMapCTerm_c deserialize_value_OrderedMapCTerm (Data &&abstract_type, std::istream &in);

inline constexpr OrderedMapDC_c const &abstract_type_of (OrderedMapDCTerm_c const &) { return OrderedMapDC; }
inline constexpr OrderedMapD_c const &abstract_type_of (OrderedMapDTerm_c const &) { return OrderedMapD; }
inline constexpr OrderedMapC_c const &abstract_type_of (OrderedMapCTerm_c const &) { return OrderedMapC; }

inline Data const &domain_of (OrderedMapDCTerm_c const &ordered_map_type) { return ordered_map_type.domain(); }
inline Data const &domain_of (OrderedMapDTerm_c const &ordered_map_type) { return ordered_map_type.domain(); }
inline Term_c const &domain_of (OrderedMapCTerm_c const &ordered_map_type) { return ordered_map_type.domain(); }
inline Term_c const &domain_of (OrderedMap_c const &ordered_map_type) { return ordered_map_type.domain(); }

inline Data domain_of (OrderedMapDCTerm_c &&ordered_map_type) { return std::move(ordered_map_type).domain(); }
inline Data domain_of (OrderedMapDTerm_c &&ordered_map_type) { return std::move(ordered_map_type).domain(); }

inline Data const &codomain_of (OrderedMapDCTerm_c const &ordered_map_type) { return ordered_map_type.codomain(); }
inline Term_c const &codomain_of (OrderedMapDTerm_c const &ordered_map_type) { return ordered_map_type.codomain(); }
inline Data const &codomain_of (OrderedMapCTerm_c const &ordered_map_type) { return ordered_map_type.codomain(); }
inline Term_c const &codomain_of (OrderedMap_c const &ordered_map_type) { return ordered_map_type.codomain(); }

inline Data codomain_of (OrderedMapDCTerm_c &&ordered_map_type) { return std::move(ordered_map_type).codomain(); }
inline Data codomain_of (OrderedMapDTerm_c &&ordered_map_type) { return std::move(ordered_map_type).codomain(); }

inline int compare (OrderedMapDCTerm_c const &lhs, OrderedMapDCTerm_c const &rhs) {
    auto c = compare_data(lhs.domain(), rhs.domain());
    if (c != 0)
        return c;

    return compare_data(lhs.codomain(), rhs.codomain());
}

inline int compare (OrderedMapDTerm_c const &lhs, OrderedMapDTerm_c const &rhs) {
    return compare_data(lhs.domain(), rhs.domain());
}

inline int compare (OrderedMapCTerm_c const &lhs, OrderedMapCTerm_c const &rhs) {
    return compare_data(lhs.codomain(), rhs.codomain());
}

} // end namespace sept

namespace std {

// Template specialization(s) to define std::hash for the OrderedMap types.
// This is generally done by combining the hashes of the typeid of the argument and its content.

template <>
struct hash<sept::OrderedMapDCTerm_c> {
    size_t operator () (sept::OrderedMapDCTerm_c const &t) const {
        return sept::hash(typeid(sept::OrderedMapDCTerm_c), t.domain(), t.codomain());
    }
};

template <>
struct hash<sept::OrderedMapDTerm_c> {
    size_t operator () (sept::OrderedMapDTerm_c const &t) const {
        return sept::hash(typeid(sept::OrderedMapDTerm_c), t.domain());
    }
};

template <>
struct hash<sept::OrderedMapCTerm_c> {
    size_t operator () (sept::OrderedMapCTerm_c const &t) const {
        return sept::hash(typeid(sept::OrderedMapCTerm_c), t.codomain());
    }
};

template <>
struct hash<sept::OrderedMap_c> {
    size_t operator () (sept::OrderedMap_c const &t) const {
        return sept::hash(typeid(sept::OrderedMap_c));
    }
};

} // end namespace std
