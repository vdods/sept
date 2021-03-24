// 2020.03.13 - Victor Dods

#pragma once

#include <any>
#include <cassert>
#include <exception>
#include <functional>
#include <iostream>
#include <lvd/OstreamDelegate.hpp>
#include <lvd/StaticAssociation_t.hpp>
#include "sept/core.hpp"
#include <typeindex>
#include <type_traits>

// This template metafunction should be present in std::, but isn't.  There is a private
// implementation of it in the std implementation.
template <typename T_> struct is_in_place_type : std::false_type { };

// Template specialization to make the metafunction return true for std::in_place_type_t<T_>.
template <typename T_> struct is_in_place_type<std::in_place_type_t<T_>> : std::true_type { };

// Convenience alias for is_in_place_type<T_>::value.
template <typename T_> inline constexpr bool is_in_place_type_v = is_in_place_type<T_>::value;

namespace sept {

// This is necessary to get the operator<< based printing of sept::Data to work.
using lvd::operator<<;

// Forward declaration of the type-specified Data.
template <typename T_>
class Data_t;

// Data is the fundamental unit of currency in sept; this is how each data node is represented in C++.
// TODO: Should this be called Data or Node?  Or what?
// TODO: Maybe inherit std::any privately
class Data : public std::any {
public:

    // These are the same constructors std::any has, with the exception of the default constructor.

    // It should be impossible to create a Data without a value.
    Data () = delete;
    Data (Data const &other) : std::any(static_cast<std::any const &>(other)) { }
    Data (Data &&other) noexcept : std::any(std::move(static_cast<std::any &&>(other))) { }

    template <
        typename ValueType_,
        typename U_ = std::decay_t<ValueType_>,
        typename = std::enable_if_t<
            !std::is_base_of_v<Data,U_> &&
            !is_in_place_type_v<ValueType_> &&
            std::is_copy_constructible_v<U_>
        >
    >
    Data (ValueType_ &&value)
    :   std::any(std::forward<ValueType_>(value))
    { }

    template <
        typename ValueType_,
        typename... Args_,
        typename U_ = std::decay_t<ValueType_>,
        typename = std::enable_if_t<
            std::is_constructible_v<U_,Args_...> &&
            std::is_copy_constructible_v<U_> // This one doesn't make sense, it should allow move-construction too.
        >
    >
    explicit Data (std::in_place_type_t<ValueType_>, Args_&&... args)
    :   std::any(std::in_place_type_t<ValueType_>(), std::forward<Args_>(args)...)
    { }

    template <
        typename ValueType_,
        typename I_,
        typename... Args_,
        typename U_ = std::decay_t<ValueType_>,
        typename = std::enable_if_t<
            std::is_constructible_v<U_,std::initializer_list<I_>&, Args_...> &&
            std::is_copy_constructible_v<U_> // Also not 100% sure about this one.
        >
    >
    explicit Data (std::in_place_type_t<ValueType_>, std::initializer_list<I_> il, Args_&&... args)
    :   std::any(std::in_place_type_t<ValueType_>(), il, std::forward<Args_>(args)...)
    { }

    // These are the same operator= overloads as std::any has.
    Data &operator = (Data const &other) {
        std::any::operator=(static_cast<std::any const &>(other));
        return *this;
    }
    Data &operator = (Data &&other) noexcept {
        std::any::operator=(std::move(static_cast<std::any &&>(other)));
        return *this;
    }
    template <
        typename ValueType_,
        typename U_ = std::decay_t<ValueType_>,
        typename = std::enable_if_t<
            !std::is_base_of_v<Data,U_> && // TODO: Change to "is subclass of Data"
            std::is_copy_constructible_v<U_>
        >
    >
    Data &operator = (ValueType_ &&value) {
        std::any::operator=(std::move(value));
        return *this;
    }

    // Returns true iff the given std::any_cast<T_> would succeed (this uses the pointer-semantic version of std::any_cast)
    template <typename T_>
    bool can_cast () const & noexcept {
        return std::any_cast<T_>(static_cast<std::any const *>(this)) != nullptr;
    }
    // Returns true iff the given std::any_cast<T_> would succeed (this uses the pointer-semantic version of std::any_cast)
    template <typename T_>
    bool can_cast () & noexcept {
        return std::any_cast<T_>(static_cast<std::any *>(this)) != nullptr;
    }
    // Returns true iff the given std::any_cast<T_> would succeed (this uses the pointer-semantic version of std::any_cast)
    template <typename T_>
    bool can_cast () && noexcept {
        // Not 100% sure if this can_cast version is necessary or even correct.
        return std::any_cast<T_>(static_cast<std::any *>(this)) != nullptr;
    }

    // Essentially performs std::any_cast<T_> on this object, except with nicer syntax.
    template <typename T_>
    T_ cast () const & { // Using auto for the return type is a copout, but I don't care!
        return std::any_cast<T_>(static_cast<std::any const &>(*this));
    }
    // Essentially performs std::any_cast<T_> on this object, except with nicer syntax.
    template <typename T_>
    T_ cast () & { // Using auto for the return type is a copout, but I don't care!
        return std::any_cast<T_>(static_cast<std::any &>(*this));
    }
    // Essentially performs std::any_cast<T_> on this object, except with nicer syntax.
    template <typename T_>
    T_ cast () && { // Using auto for the return type is a copout, but I don't care!
        return std::any_cast<T_>(static_cast<std::any &&>(*this));
    }

    // This is a run-time type assertion that this Data actually holds T_.  This call then just type-casts this
    // to the more-specific type Data_t<T_> const &.
    template <typename T_>
    Data_t<T_> const &as () const & {
        std::any_cast<T_>(static_cast<std::any const &>(*this)); // This will throw std::bad_any_cast if the cast is invalid.
        return static_cast<Data_t<T_> const &>(*this);
    }
    // This is a run-time type assertion that this Data actually holds T_.  This call then just type-casts this
    // to the more-specific type Data_t<T_> &.
    template <typename T_>
    Data_t<T_> &as () & {
        std::any_cast<T_>(static_cast<std::any &>(*this)); // This will throw std::bad_any_cast if the cast is invalid.
        return static_cast<Data_t<T_>&>(*this);
    }
    // This is a run-time type assertion that this Data actually holds T_.  This call then just type-casts this
    // to the more-specific type Data_t<T_> &&.
    template <typename T_>
    Data_t<T_> &&as () && {
        std::any_cast<T_>(static_cast<std::any &&>(*this)); // This will throw std::bad_any_cast if the cast is invalid.
        return static_cast<Data_t<T_>&&>(*this);
    }

    // Convenient way to get an overload for operator<< on std::ostream.
    operator lvd::OstreamDelegate () const;
};

// TODO: Implement specialization for std::swap(Data &, Data &)

// This is analogous to std::make_any<T_>
template <typename T_, typename... Args_>
Data make_data (Args_&&... args) {
    return Data(std::in_place_type_t<T_>(), std::forward<Args_>(args)...);
}

using DataPredicateUnary = std::function<bool(Data const &)>;
using DataPredicateBinary = std::function<bool(Data const &, Data const &)>;

// This is the type of the map that the Data equality predicates are registered into.
using EqDataPredicateMap = std::unordered_map<std::type_index,DataPredicateBinary>;
// This defines a static instance of EqDataPredicateMap that the Data equality predicates are registered into.
LVD_STATIC_ASSOCIATION_DEFINE(DataOperatorEq, EqDataPredicateMap)
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_DATA_OPERATOR_EQ_REGISTER_TYPE(type) LVD_STATIC_ASSOCIATION_REGISTER(DataOperatorEq, type, std::type_index(typeid(type)), [](Data const &lhs, Data const &rhs){ return lhs.cast<type const &>() == rhs.cast<type const &>(); })
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_DATA_OPERATOR_EQ_REGISTER_TYPE_EXPLICIT(type, unique_id) LVD_STATIC_ASSOCIATION_REGISTER(DataOperatorEq, unique_id, std::type_index(typeid(type)), [](Data const &lhs, Data const &rhs){ return lhs.cast<type const &>() == rhs.cast<type const &>(); })

// Only allow definitions of equality where the data are the same type.  This means that for values of
// different types to be equal (i.e. different int sizes), their conversion has to be explicit.
inline bool eq_data (Data const &lhs, Data const &rhs) {
// inline bool eq_data (Data const &lhs, Data const &rhs) {
    // If the types differ, they can't be equal.
    if (lhs.type() != rhs.type())
        return false;

    // Otherwise look up the type in the predicate map.
    auto const &data_operator_eq_predicate_map = lvd::static_association_singleton<sept::DataOperatorEq>();
    auto it = data_operator_eq_predicate_map.find(std::type_index(lhs.type()));
    if (it == data_operator_eq_predicate_map.end())
        throw std::runtime_error(LVD_FMT("Data type " << lhs.type().name() << " not registered in DataOperatorEq for use in eq_data"));

    return it->second(lhs, rhs);
}

inline bool neq_data (Data const &lhs, Data const &rhs) { return !eq_data(lhs, rhs); }

// TODO: Maybe use an explicitly named equals function instead, otherwise the fact that type can be converted into
// Data greatly interferes with finding legitimately missing overloads for operator== for specific types.
inline bool operator == (Data const &lhs, Data const &rhs) { return eq_data(lhs, rhs); }

// TODO: Maybe use an explicitly named not_equals function instead
inline bool operator != (Data const &lhs, Data const &rhs) { return neq_data(lhs, rhs); }

// Determines set membership.  Doesn't have to be defined for all possible containers.
// TODO: Provide overloads for all the various Data_t<T_> types, so that more compile-time stuff can be done.
bool is_member (Data const &value, Data const &container);

// Produces the element type of an Array_c*.  Might be used for other types in the future as well.
// TODO: Provide overloads for all the various Data_t<T_> types, so that more compile-time stuff can be done.
Data element_type_of (Data const &array_type);
Data element_type_of (Data &&array_type);

Data domain_of (Data const &ordered_map_type);
Data domain_of (Data &&ordered_map_type);

Data codomain_of (Data const &ordered_map_type);
Data codomain_of (Data &&ordered_map_type);

} // end namespace sept

namespace std {

// template <typename Key_> struct hash;

// Template specialization to define std::hash<sept::Data>.
template <>
struct hash<sept::Data> {
    size_t operator () (sept::Data const &data) const;
};

// Overload to define std::swap for sept::Data.  NOTE/TODO: Figure out if this causes a problem
// with Data_t<A> being swappable with Data_t<B>.  Maybe use an std::enable_if?
inline void swap (sept::Data &lhs, sept::Data &rhs) noexcept {
    assert(false && "just testing to see if this is ever called -- maybe it shouldn't be (see comments)");
    swap(static_cast<any&>(lhs), static_cast<any&>(rhs));
}

} // end namespace std
