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
#include "sept/hash.hpp"
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

// Forward declaration of print_data, with necessary forward declaration of Data.
class Data;
void print_data (std::ostream &out, Data const &data);

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
    operator lvd::OstreamDelegate () const {
        assert(this->has_value());
        return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
            print_data(out, *this);
        });
    }
};

// TODO: Implement specialization for std::swap(Data &, Data &)

// This is analogous to std::make_any<T_>
template <typename T_, typename... Args_>
Data make_data (Args_&&... args) {
    return Data(std::in_place_type_t<T_>(), std::forward<Args_>(args)...);
}

//
// StaticAssociation_t for Data::operator lvd::OstreamDelegate
//

using DataPrintingFunction = std::function<void(std::ostream &, Data const &)>;

// This is the type of the map that the Data printing functions are registered into.
using DataPrintingFunctionMap = std::unordered_map<std::type_index,DataPrintingFunction>;
// This defines a static instance of EqDataPredicateMap that the Data equality predicates are registered into.
LVD_STATIC_ASSOCIATION_DEFINE(DataPrinting, DataPrintingFunctionMap)
// This macro must be used when `type` is not a C identifier (meaning that it can't automatically go into
// the static variable name that is used in the registration).
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_DATA_PRINTING_REGISTER_TYPE_EXPLICIT(type, unique_id) LVD_STATIC_ASSOCIATION_REGISTER(DataPrinting, unique_id, std::type_index(typeid(type)), [](std::ostream &out, Data const &data){ out << data.cast<type const &>(); })
// This macro can be used when `type` is a C identifier (meaning that it can automatically go into
// the static variable name that is used in the registration).
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_DATA_PRINTING_REGISTER_TYPE(type) SEPT_DATA_PRINTING_REGISTER_TYPE_EXPLICIT(type, type)

inline void print_data (std::ostream &out, Data const &data) {
    // Look up the type in the predicate map.
    auto const &data_printing_function_map = lvd::static_association_singleton<sept::DataPrinting>();
    auto it = data_printing_function_map.find(std::type_index(data.type()));
    if (it == data_printing_function_map.end())
        throw std::runtime_error(LVD_FMT("Data type " << data.type().name() << " not registered in DataPrinting for use in print_data"));

    it->second(out, data);
}

//
// StaticAssociation_t for eq_data
//

using DataPredicateUnary = std::function<bool(Data const &)>;
using DataPredicateBinary = std::function<bool(Data const &, Data const &)>;

// This is the type of the map that the Data equality predicates are registered into.
using EqDataPredicateMap = std::unordered_map<std::type_index,DataPredicateBinary>;
// This defines a static instance of EqDataPredicateMap that the Data equality predicates are registered into.
LVD_STATIC_ASSOCIATION_DEFINE(EqData, EqDataPredicateMap)
// This macro must be used when `type` is not a C identifier (meaning that it can't automatically go into
// the static variable name that is used in the registration).
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_EQ_DATA_REGISTER_TYPE_EXPLICIT(type, unique_id) LVD_STATIC_ASSOCIATION_REGISTER(EqData, unique_id, std::type_index(typeid(type)), [](Data const &lhs, Data const &rhs){ return lhs.cast<type const &>() == rhs.cast<type const &>(); })
// This macro can be used when `type` is a C identifier (meaning that it can automatically go into
// the static variable name that is used in the registration).
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_EQ_DATA_REGISTER_TYPE(type) SEPT_EQ_DATA_REGISTER_TYPE_EXPLICIT(type, type)

// Only allow definitions of equality where the data are the same type.  This means that for values of
// different types to be equal (i.e. different int sizes), their conversion has to be explicit.
inline bool eq_data (Data const &lhs, Data const &rhs) {
    // If the types differ, they can't be equal.
    if (lhs.type() != rhs.type())
        return false;

    // Otherwise look up the type in the predicate map.
    auto const &data_operator_eq_predicate_map = lvd::static_association_singleton<sept::EqData>();
    auto it = data_operator_eq_predicate_map.find(std::type_index(lhs.type()));
    if (it == data_operator_eq_predicate_map.end())
        throw std::runtime_error(LVD_FMT("Data type " << lhs.type().name() << " not registered in EqData for use in eq_data"));

    return it->second(lhs, rhs);
}

// TODO: Maybe use an explicitly named equals function instead, otherwise the fact that type can be converted into
// Data greatly interferes with finding legitimately missing overloads for operator== for specific types.
inline bool operator == (Data const &lhs, Data const &rhs) { return eq_data(lhs, rhs); }

// TODO: Maybe use an explicitly named not_equals function instead
inline bool operator != (Data const &lhs, Data const &rhs) { return !eq_data(lhs, rhs); }

//
// StaticAssociation_t for abstract_type_of_data -- abstract_type_of_data(x) should return the most-specific
// type that x belongs to.
//

using DataEvaluatorFunction = std::function<Data(Data const &)>;
using AbstractTypeOfDataEvaluatorMap = std::unordered_map<std::type_index,DataEvaluatorFunction>;
// This defines a static instance of EqDataPredicateMap that the Data equality predicates are registered into.
LVD_STATIC_ASSOCIATION_DEFINE(AbstractTypeOf, AbstractTypeOfDataEvaluatorMap)
// This macro must be used when `Value` is not a C identifier (meaning that it can't automatically go into
// the static variable name that is used in the registration).
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_EXPLICIT_IMPL(Value, unique_id, evaluator) \
    LVD_STATIC_ASSOCIATION_REGISTER( \
        AbstractTypeOf, \
        unique_id, \
        std::type_index(typeid(Value)), \
        evaluator \
    )
// This macro can be used `Value` is a C identifier (meaning that it can automatically go into
// the static variable name that is used in the registration).
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_IMPL(Value, evaluator) \
    SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_EXPLICIT_IMPL(Value, Value, evaluator)

// This macro must be used when `Value` is not a C identifier (meaning that it can't automatically go into
// the static variable name that is used in the registration).
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_EXPLICIT(Value, unique_id, evaluator_body) \
    SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_EXPLICIT_IMPL( \
        Value, \
        unique_id, \
        [](Data const &value_data)->Data{ \
            auto const &value = value_data.cast<Value const &>(); \
            std::ignore = value; \
            evaluator_body \
        } \
    )
// This macro can be used `Value` is a C identifier (meaning that it can automatically go into
// the static variable name that is used in the registration).
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE(Value, evaluator_body) \
    SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_EXPLICIT(Value, Value, evaluator_body)
// This macro must be used when `Value` is not a C identifier (meaning that it can't automatically go into
// the static variable name that is used in the registration).
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_EXPLICIT_DEFAULT(Value, unique_id) \
    SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_EXPLICIT(Value, unique_id, return abstract_type_of(value);)
// This macro can be used `Value` is a C identifier (meaning that it can automatically go into
// the static variable name that is used in the registration).
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_DEFAULT(Value) \
    SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_EXPLICIT(Value, Value, return abstract_type_of(value);)

Data abstract_type_of_data (Data const &value_data);

//
// StaticAssociation_t for inhabits_data -- inhabits_data(x, T) means "x inhabits T [as an abstract type]".
// Note that this isn't the same as "abstract_type_of_data(x) == T", since in general x could inhabit
// many different, overlapping abstract types.
//

struct TypeIndexPair {
    std::type_index m_value_ti;
    std::type_index m_type_ti;

    TypeIndexPair (std::type_index value_ti, std::type_index type_ti)
        :   m_value_ti(value_ti)
        ,   m_type_ti(type_ti)
    { }

    bool operator == (TypeIndexPair const &other) const {
        return m_value_ti == other.m_value_ti && m_type_ti == other.m_type_ti;
    }
};

} // end namespace sept

// This has to go before std::unordered_map<TypeIndexPair,...>
namespace std {

// Template specialization to define std::hash<sept::TypeIndexPair>.
template <>
struct hash<sept::TypeIndexPair> {
    size_t operator () (sept::TypeIndexPair const &k) const {
        return sept::hash(k.m_value_ti, k.m_type_ti);
    }
};

} // end namespace std

namespace sept {

// This is the type of the map that the Data equality predicates are registered into.
using InhabitsDataPredicateMap = std::unordered_map<TypeIndexPair,DataPredicateBinary>;
// This defines a static instance of InhabitsDataPredicateMap that the inhabits predicates are registered into.
LVD_STATIC_ASSOCIATION_DEFINE(InhabitsData, InhabitsDataPredicateMap)
// This macro must be used when any of `Value` or `Type` is not a C identifier (meaning that it can't automatically go into
// the static variable name that is used in the registration).
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_INHABITS_DATA_REGISTER_TYPE_EXPLICIT_IMPL(Value, Type, unique_id, inhabits_evaluator) \
    LVD_STATIC_ASSOCIATION_REGISTER( \
        InhabitsData, \
        unique_id, \
        TypeIndexPair{std::type_index(typeid(Value)), std::type_index(typeid(Type))}, \
        inhabits_evaluator \
    )
// This macro can be used when both of `Value` and `Type` are C identifiers (meaning that it can automatically go into
// the static variable name that is used in the registration).
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_INHABITS_DATA_REGISTER_TYPE_IMPL(Value, Type, inhabits_evaluator) \
    SEPT_INHABITS_DATA_REGISTER_TYPE_EXPLICIT_IMPL(Value, Type, __##Value##___##Type##__, inhabits_evaluator)

// This macro must be used when any of `Value` or `Type` is not a C identifier (meaning that it can't automatically go into
// the static variable name that is used in the registration).
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_INHABITS_DATA_REGISTER_TYPE_EXPLICIT(Value, Type, unique_id, inhabits_evaluator_body) \
    SEPT_INHABITS_DATA_REGISTER_TYPE_EXPLICIT_IMPL( \
        Value, \
        Type, \
        unique_id, \
        [](Data const &value_data, Data const &type_data)->bool{ \
            auto const &value = value_data.cast<Value const &>(); \
            auto const &type = type_data.cast<Type const &>(); \
            std::ignore = value; \
            std::ignore = type; \
            inhabits_evaluator_body \
        } \
    )
// This macro can be used when both of `Value` and `Type` are C identifiers (meaning that it can automatically go into
// the static variable name that is used in the registration).
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_INHABITS_DATA_REGISTER_TYPE(Value, Type, inhabits_evaluator_body) \
    SEPT_INHABITS_DATA_REGISTER_TYPE_EXPLICIT(Value, Type, __##Value##___##Type##__, inhabits_evaluator_body )

// This macro must be used when any of `Value` or `Type` is not a C identifier (meaning that it can't automatically go into
// the static variable name that is used in the registration).  This should be used when inhabits_evaluator_body would
// always return true, meaning it doesn't depend on any runtime state of value or type.
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_INHABITS_DATA_REGISTER_TYPE_EXPLICIT_TRIVIAL(Value, Type, unique_id) \
    SEPT_INHABITS_DATA_REGISTER_TYPE_EXPLICIT_IMPL(Value, Type, unique_id, DataPredicateBinary{nullptr})
// This macro can be used when both of `Value` and `Type` are C identifiers (meaning that it can automatically go into
// the static variable name that is used in the registration).  This should be used when inhabits_evaluator_body would
// always return true, meaning it doesn't depend on any runtime state of value or type.
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_INHABITS_DATA_REGISTER_TYPE_TRIVIAL(Value, Type) \
    SEPT_INHABITS_DATA_REGISTER_TYPE_IMPL(Value, Type, DataPredicateBinary{nullptr})

bool inhabits_data (Data const &value_data, Data const &type_data);

//
// StaticAssociation_t for compare_data -- compare_data(lhs, rhs) should return:
// -    A negative value if lhs < rhs
// -    0 if lhs == rhs
// -    A positive value if lhs > rhs
//
// TODO: Change this into TotalOrder, and also implement PartialOrder
//

using CompareEvaluatorFunction = std::function<int(Data const &,Data const &)>;
using CompareDataEvaluatorMap = std::unordered_map<TypeIndexPair,CompareEvaluatorFunction>;
// This defines a static instance of EqDataPredicateMap that the Data equality predicates are registered into.
LVD_STATIC_ASSOCIATION_DEFINE(CompareData, CompareDataEvaluatorMap)
// This macro must be used when `Lhs` or `Rhs` is not a C identifier (meaning that it can't automatically go into
// the static variable name that is used in the registration).
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_COMPARE_DATA_REGISTER_TYPE_EXPLICIT_IMPL(Lhs, Rhs, unique_id, evaluator) \
    LVD_STATIC_ASSOCIATION_REGISTER( \
        CompareData, \
        unique_id, \
        TypeIndexPair{std::type_index(typeid(Lhs)), std::type_index(typeid(Rhs))}, \
        evaluator \
    )
// This macro can be used `Lhs` and `Rhs` is a C identifier (meaning that it can automatically go into
// the static variable name that is used in the registration).
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_COMPARE_DATA_REGISTER_TYPE_IMPL(Lhs, Rhs, evaluator) \
    SEPT_COMPARE_DATA_REGISTER_TYPE_EXPLICIT_IMPL(Lhs, Rhs, __##Lhs##___##Rhs##__, evaluator)

// This macro must be used when `Lhs` or `Rhs` is not a C identifier (meaning that it can't automatically go into
// the static variable name that is used in the registration).
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_COMPARE_DATA_REGISTER_TYPE_SINGLETON_EXPLICIT(Type, unique_id) \
    SEPT_COMPARE_DATA_REGISTER_TYPE_EXPLICIT_IMPL(Type, Type, unique_id, nullptr)
// This macro can be used when `Lhs` and `Rhs` is a C identifier (meaning that it can automatically go into
// the static variable name that is used in the registration).
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_COMPARE_DATA_REGISTER_SINGLETON_TYPE(Type) \
    SEPT_COMPARE_DATA_REGISTER_TYPE_EXPLICIT_IMPL(Type, Type, Type, nullptr)

// This macro must be used when `Lhs` or `Rhs` is not a C identifier (meaning that it can't automatically go into
// the static variable name that is used in the registration).
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_COMPARE_DATA_REGISTER_TYPE_DEFAULT_EXPLICIT(Lhs, Rhs, unique_id) \
    SEPT_COMPARE_DATA_REGISTER_TYPE_EXPLICIT_IMPL( \
        Lhs, \
        Rhs, \
        unique_id, \
        [](Data const &lhs_data, Data const &rhs_data)->int{ \
            return sept::compare(lhs_data.cast<Lhs const &>(), rhs_data.cast<Rhs const &>()); \
        } \
    )
// This macro can be used when `Lhs` and `Rhs` is a C identifier (meaning that it can automatically go into
// the static variable name that is used in the registration).
// NOTE: Usage of this macro must be within a cpp file, not an hpp file (otherwise there will be a double-registration error at runtime init)
#define SEPT_COMPARE_DATA_REGISTER_DEFAULT_TYPE(Lhs, Rhs) \
    SEPT_COMPARE_DATA_REGISTER_TYPE_DEFAULT_EXPLICIT(Lhs, Rhs, __##Lhs##___##Rhs##__)

int compare_data (Data const &lhs, Data const &rhs);

//
// Other stuff
//

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
