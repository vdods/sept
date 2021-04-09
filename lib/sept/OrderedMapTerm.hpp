// 2020.07.15 - Victor Dods

#pragma once

#include <cassert>
#include <initializer_list>
#include <lvd/OstreamDelegate.hpp>
#include "sept/core.hpp"
#include "sept/Data.hpp"
#include "sept/hash.hpp"
#include "sept/OrderedMapType.hpp"
#include <sstream>
#include <vector>

namespace sept {

std::ostream &operator << (std::ostream &out, DataOrderedMap const &m);

//
// OrderedMapConstraint (type constraint for OrderedMapTerm_c)
//

class OrderedMapConstraint {
public:

    // This is effectively no constraint.
    OrderedMapConstraint () : m_ordered_map_type(OrderedMap_c()) {
        assert(m_ordered_map_type.has_value());
    }
    OrderedMapConstraint (OrderedMapConstraint const &) = default;
    OrderedMapConstraint (OrderedMapConstraint &&) = default;
    template <typename Arg_, typename = std::enable_if_t<std::is_base_of_v<Data,Arg_> || is_an_ordered_map_type<std::decay_t<Arg_>>::value>>
    explicit OrderedMapConstraint (Arg_ &&ordered_map_type)
    :   m_ordered_map_type(std::forward<Arg_>(ordered_map_type)) {
        if (m_ordered_map_type.type() != typeid(OrderedMapDCTerm_c) && m_ordered_map_type.type() != typeid(OrderedMapDTerm_c) && m_ordered_map_type.type() != typeid(OrderedMapCTerm_c) && m_ordered_map_type.type() != typeid(OrderedMap_c))
            throw std::runtime_error(LVD_FMT("expecting an ordered map type, but got " << m_ordered_map_type.type()));
    }

    OrderedMapConstraint &operator = (OrderedMapConstraint const &) = default;
    OrderedMapConstraint &operator = (OrderedMapConstraint &&) = default;

    Data const &ordered_map_type () const & { return m_ordered_map_type; }
    Data ordered_map_type () && { return std::move(m_ordered_map_type); }

    void verify_constraint_or_throw (DataOrderedMap const &m) const;

    operator lvd::OstreamDelegate () const {
        return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
            out << "OrderedMapConstraint(" << m_ordered_map_type << ')';
        });
    }

private:

    Data m_ordered_map_type;
};

//
// OrderedMapTerm_c
//

class OrderedMapTerm_c {
public:

    OrderedMapTerm_c ()
    :   m_constraint()
    ,   m_pairs()
    {
        // OrderedMapConstraint is already satisfied by construction.
    }
    OrderedMapTerm_c (OrderedMapTerm_c const &other)
    :   m_constraint(other.m_constraint)
    ,   m_pairs(other.m_pairs)
    {
        // OrderedMapConstraint is already satisfied by construction.
    }
    // TODO: Maybe could add OrderedMapTerm_c(OrderedMapTerm &other) : OrderedMapTerm_c(static_cast<OrderedMapTerm_c const &>(other)) { }
    // in order to then allow the variadic template constructor (that may have been the obstruction from earlier).
    OrderedMapTerm_c (OrderedMapTerm_c &&other)
    :   m_constraint(std::move(other.m_constraint))
    ,   m_pairs(std::move(other.m_pairs))
    {
        // OrderedMapConstraint is already satisfied by construction.
    }
//     // Use this to pass a std::map directly in.  Should be able to pass in a std::map<T,U>
//     // for any T and U, since it will be used to construct the DataOrderedMap-typed pairs.
//     template <typename T_, typename U_>
//     explicit OrderedMapTerm_c (std::map<T_,U_> const &pairs)
//     :   m_constraint() // No constraint yet.
//     ,   m_pairs(pairs)
//     {
//         // OrderedMapConstraint is already satisfied by construction.
//     }
    // Use this to pass a std::vector directly in.  Should be able to pass in a std::vector<T>
    // for any T, since it will be used to construct the DataOrderedMap-typed pairs.
    explicit OrderedMapTerm_c (DataOrderedMap &&pairs)
    :   m_constraint() // No constraint yet.
    ,   m_pairs(std::move(pairs))
    {
        // OrderedMapConstraint is already satisfied by construction.
    }
//     // Use this to pass a std::map directly in.  Should be able to pass in a std::map<T,U>
//     // for any T and U, since it will be used to construct the DataOrderedMap-typed pairs.
//     template <typename T_, typename U_>
//     explicit OrderedMapTerm_c (std::vector<T_,U_> &&pairs)
//     :   m_constraint() // No constraint yet.
//     ,   m_pairs()
//     {
//         m_pairs.reserve(pairs.size());
//         for (auto &&element : pairs)
//             m_pairs.emplace_back(std::move(element));
//         // OrderedMapConstraint is already satisfied by construction.
//     }
    // There is no constructor that just takes arbitrary pairs; use OrderedMap(x, y, z, ...).
    // This uses OrderedMap_c::operator().  NOTE: Couldn't use a templatized, arbitrary-element
    // constructor here because it caused an infinite mutual recursion with the ValueType_-
    // taking constructor of Data.
//     template <typename... Args_>
//     explicit OrderedMapTerm_c (Args_&&... args)
//     :   m_constraint() // No constraint yet.
//     ,   m_pairs()
//     {
//         m_pairs.reserve(sizeof...(args));
//         (m_pairs.emplace_back(std::forward<Args_>(args)), ...);
//     }

    OrderedMapTerm_c &operator = (OrderedMapTerm_c const &other) = default;
    OrderedMapTerm_c &operator = (OrderedMapTerm_c &&other) = default;

    bool operator == (Data const &other) const {
        return (other.type() == typeid(OrderedMapTerm_c)) && this->operator==(other.cast<OrderedMapTerm_c const &>());
    }
    // NOTE: This uses a useful, but weak notion of equality that compares only the pairs.
    bool operator == (OrderedMapTerm_c const &other) const {
        return m_pairs == other.m_pairs;
//         if (m_pairs.size() != other.m_pairs.size())
//             return false;
//         // Not sure why DataOrderedMap doesn't have a working equality operator.
//         for (size_t i = 0; i < m_pairs.size(); ++i)
//             if (m_pairs[i] != other.m_pairs[i])
//                 return false;
//         return true;
    }
    bool operator != (OrderedMapTerm_c const &other) const { return !(*this == other); }

    OrderedMapConstraint const &constraint () const & { return m_constraint; }
    OrderedMapConstraint constraint () && { return std::move(m_constraint); }
    Data const &ordered_map_type () const & { return m_constraint.ordered_map_type(); }
    Data ordered_map_type () && { return std::move(m_constraint).ordered_map_type(); }
    Data domain () const { return domain_of(ordered_map_type()); }
    Data codomain () const { return codomain_of(ordered_map_type()); }
    size_t size () const { return m_pairs.size(); }
    DataOrderedMap const &pairs () const & { return m_pairs; }
    DataOrderedMap &pairs () & { return m_pairs; }

    Data const &operator [] (Data const &key) const { return m_pairs.at(key); }
    Data &operator [] (Data const &key) { return m_pairs.at(key); }

    DataOrderedMap::const_iterator begin () const { return m_pairs.begin(); }
    DataOrderedMap::const_iterator end () const { return m_pairs.end(); }

    DataOrderedMap::iterator begin () { return m_pairs.begin(); }
    DataOrderedMap::iterator end () { return m_pairs.end(); }

    // This makes a copy.
    template <typename... Args_>
    OrderedMapTerm_c with_constraint (Args_&&... args) const & {
        OrderedMapConstraint constraint(std::forward<Args_>(args)...);
        constraint.verify_constraint_or_throw(m_pairs);
        OrderedMapTerm_c retval(*this);
        retval.m_constraint = constraint;
        return retval;
    }
    // This uses move semantics.
    template <typename... Args_>
    OrderedMapTerm_c &&with_constraint (Args_&&... args) && {
        m_constraint = OrderedMapConstraint(std::forward<Args_>(args)...);
        m_constraint.verify_constraint_or_throw(m_pairs);
        return std::move(*this);
    }

    // This makes a copy.
    OrderedMapTerm_c without_constraint () const & {
        OrderedMapTerm_c retval(*this);
        retval.m_constraint = OrderedMapConstraint();
        return retval;
    }
    // This uses move semantics.
    OrderedMapTerm_c &&without_constraint () && {
        m_constraint = OrderedMapConstraint();
        return std::move(*this);
    }

    operator lvd::OstreamDelegate () const {
        return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
            out << "OrderedMapTerm_c<" << domain() << ',' << codomain() << '>' << pairs();
        });
    }

private:

    void verify_constraint_or_throw () const {
        m_constraint.verify_constraint_or_throw(m_pairs);
    }

    OrderedMapConstraint m_constraint;
    // Ideally we would have an map_any, where there is a single type specifier for all keys and all values,
    // instead of having one per key and per value as is in the case of DataOrderedMap (noting that Data
    // derives from std::any).
    DataOrderedMap m_pairs;
};

// This is used to construct OrderedMapTerm_c more efficiently (std::initializer_list lacks move semantics for some dumb
// reason), as well as to avoid a potential infinite loop in constructors between OrderedMapTerm_c, Data, and std::any.
template <typename... Args_>
OrderedMapTerm_c make_ordered_map (Args_&&... args) {
    DataOrderedMap pairs;
//     pairs.reserve(sizeof...(args));
    // This is a fold expression -- see https://en.cppreference.com/w/cpp/language/fold
    (pairs.emplace(std::forward<Args_>(args)), ...);
    return OrderedMapTerm_c(std::move(pairs));
}

void serialize (OrderedMapTerm_c const &v, std::ostream &out);

// This assumes that in.peek() will return SerializedTopLevelCode::PARAMETRIC_TERM.
OrderedMapTerm_c deserialize_value_OrderedMapTerm (Data &&abstract_type, std::istream &in);

inline Data const &abstract_type_of (OrderedMapTerm_c const &a) { return a.constraint().ordered_map_type(); }

// Runtime complexity: O(log(size))
bool is_member_key (Data const &value, OrderedMapTerm_c const &container);

// Runtime complexity O(size*log(size)); Lexicographical order on the ordered sequence of (key,value)
// pairs, where each pair uses lexicographical order as well.
int compare (OrderedMapTerm_c const &lhs, OrderedMapTerm_c const &rhs);

template <typename... Args_>
OrderedMapTerm_c OrderedMapDCTerm_c::operator () (Args_&&... args) const {
    return make_ordered_map(std::forward<Args_>(args)...).with_constraint(*this);
}

template <typename... Args_>
OrderedMapTerm_c OrderedMapDTerm_c::operator () (Args_&&... args) const {
    return make_ordered_map(std::forward<Args_>(args)...).with_constraint(*this);
}

template <typename... Args_>
OrderedMapTerm_c OrderedMapCTerm_c::operator () (Args_&&... args) const {
    return make_ordered_map(std::forward<Args_>(args)...).with_constraint(*this);
}

template <typename... Args_>
OrderedMapTerm_c OrderedMap_c::operator () (Args_&&... args) const {
    return make_ordered_map(std::forward<Args_>(args)...);
}

} // end namespace sept

namespace std {

// Template specialization(s) to define std::hash<Key_> for the types defined in this header file.
// This is generally done by combining the hashes of the typeid of the argument and its content.

template <>
struct hash<sept::OrderedMapConstraint> {
    size_t operator () (sept::OrderedMapConstraint const &c) const {
        return sept::hash(typeid(sept::OrderedMapConstraint), c.ordered_map_type());
    }
};

template <>
struct hash<sept::DataOrderedMap> {
    size_t operator () (sept::DataOrderedMap const &m) const {
        size_t seed = sept::hash(typeid(sept::DataOrderedMap));
        for (auto const &pair : m) {
            sept::hash_combine(seed, sept::hash(pair.first));
            sept::hash_combine(seed, sept::hash(pair.second));
        }
        return seed;
    }
};

template <>
struct hash<sept::OrderedMapTerm_c> {
    size_t operator () (sept::OrderedMapTerm_c const &a) const {
        return sept::hash(typeid(sept::OrderedMapTerm_c), a.constraint(), a.pairs());
    }
};

} // end namespace std
