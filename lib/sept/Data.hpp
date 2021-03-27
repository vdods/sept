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
Data element_of_data (Data const &param);

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

    //
    // Data model methods
    // TODO: Potentially include things like abstract_type(), serialize(), etc.
    //

    // This calls element_of_data(param).
    Data operator[] (Data const &param) const { return element_of_data(param); }
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

using DataPrintFunction = std::function<void(std::ostream &, Data const &)>;
using DataPrintFunctionMap = std::unordered_map<std::type_index,DataPrintFunction>;
LVD_STATIC_ASSOCIATION_DEFINE(_Data_Print, DataPrintFunctionMap)

#define SEPT__REGISTER__PRINT__GIVE_ID(Type, unique_id) \
    LVD_STATIC_ASSOCIATION_REGISTER( \
        _Data_Print, \
        unique_id, \
        std::type_index(typeid(Type)), \
        [](std::ostream &out, Data const &data){ \
            out << data.cast<Type const &>(); \
        } \
    )
#define SEPT__REGISTER__PRINT(Type) \
    SEPT__REGISTER__PRINT__GIVE_ID(Type, Type)

inline void print_data (std::ostream &out, Data const &data) {
    // Look up the type in the predicate map.
    auto const &data_printing_function_map = lvd::static_association_singleton<sept::_Data_Print>();
    auto it = data_printing_function_map.find(std::type_index(data.type()));
    if (it == data_printing_function_map.end())
        throw std::runtime_error(LVD_FMT("Data type " << data.type().name() << " not registered in _Data_Print for use in print_data"));

    it->second(out, data);
}

//
// StaticAssociation_t for eq_data
//

using DataPredicateUnary = std::function<bool(Data const &)>;
using DataPredicateBinary = std::function<bool(Data const &, Data const &)>;

using DataEqPredicateMap = std::unordered_map<std::type_index,DataPredicateBinary>;
LVD_STATIC_ASSOCIATION_DEFINE(_Data_Eq, DataEqPredicateMap)

#define SEPT__REGISTER__EQ__GIVE_ID(Type, unique_id) \
    LVD_STATIC_ASSOCIATION_REGISTER( \
        _Data_Eq, \
        unique_id, \
        std::type_index(typeid(Type)), \
        [](Data const &lhs, Data const &rhs){ \
            return lhs.cast<Type const &>() == rhs.cast<Type const &>(); \
        } \
    )
#define SEPT__REGISTER__EQ(Type) \
    SEPT__REGISTER__EQ__GIVE_ID(Type, Type)

// Only allow definitions of equality where the data are the same type.  This means that for values of
// different types to be equal (i.e. different int sizes), their conversion has to be explicit.
inline bool eq_data (Data const &lhs, Data const &rhs) {
    // If the types differ, they can't be equal.
    if (lhs.type() != rhs.type())
        return false;

    // Otherwise look up the type in the predicate map.
    auto const &data_operator_eq_predicate_map = lvd::static_association_singleton<sept::_Data_Eq>();
    auto it = data_operator_eq_predicate_map.find(std::type_index(lhs.type()));
    if (it == data_operator_eq_predicate_map.end())
        throw std::runtime_error(LVD_FMT("Data type " << lhs.type().name() << " not registered in _Data_Eq for use in eq_data"));

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

using DataFunction = std::function<Data(Data const &)>;
using DataAbstractTypeOfEvaluatorMap = std::unordered_map<std::type_index,DataFunction>;
LVD_STATIC_ASSOCIATION_DEFINE(_Data_AbstractTypeOf, DataAbstractTypeOfEvaluatorMap)

#define SEPT__REGISTER__ABSTRACT_TYPE_OF__GIVE_ID__EVALUATOR(Value, unique_id, evaluator) \
    LVD_STATIC_ASSOCIATION_REGISTER( \
        _Data_AbstractTypeOf, \
        unique_id, \
        std::type_index(typeid(Value)), \
        evaluator \
    )
#define SEPT__REGISTER__ABSTRACT_TYPE_OF__EVALUATOR(Value, evaluator) \
    SEPT__REGISTER__ABSTRACT_TYPE_OF__GIVE_ID__EVALUATOR(Value, Value, evaluator)
#define SEPT__REGISTER__ABSTRACT_TYPE_OF__GIVE_ID__EVALUATOR_BODY(Value, unique_id, evaluator_body) \
    SEPT__REGISTER__ABSTRACT_TYPE_OF__GIVE_ID__EVALUATOR( \
        Value, \
        unique_id, \
        [](Data const &value_data)->Data{ \
            auto const &value = value_data.cast<Value const &>(); \
            std::ignore = value; \
            evaluator_body \
        } \
    )
#define SEPT__REGISTER__ABSTRACT_TYPE_OF___GIVE_ID(Value, unique_id) \
    SEPT__REGISTER__ABSTRACT_TYPE_OF__GIVE_ID__EVALUATOR_BODY(Value, unique_id, return abstract_type_of(value);)
#define SEPT__REGISTER__ABSTRACT_TYPE_OF(Value) \
    SEPT__REGISTER__ABSTRACT_TYPE_OF__GIVE_ID__EVALUATOR_BODY(Value, Value, return abstract_type_of(value);)

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

using DataInhabitsPredicateMap = std::unordered_map<TypeIndexPair,DataPredicateBinary>;
LVD_STATIC_ASSOCIATION_DEFINE(_Data_Inhabits, DataInhabitsPredicateMap)

#define SEPT__REGISTER__INHABITS__GIVE_ID__EVALUATOR(Value, Type, unique_id, evaluator) \
    LVD_STATIC_ASSOCIATION_REGISTER( \
        _Data_Inhabits, \
        unique_id, \
        TypeIndexPair{std::type_index(typeid(Value)), std::type_index(typeid(Type))}, \
        evaluator \
    )
#define SEPT__REGISTER__INHABITS__EVALUATOR(Value, Type, evaluator) \
    SEPT__REGISTER__INHABITS__GIVE_ID__EVALUATOR(Value, Type, __##Value##___##Type##__, evaluator)
#define SEPT__REGISTER__INHABITS__GIVE_ID__EVALUATOR_BODY(Value, Type, unique_id, evaluator_body) \
    SEPT__REGISTER__INHABITS__GIVE_ID__EVALUATOR( \
        Value, \
        Type, \
        unique_id, \
        [](Data const &value_data, Data const &type_data)->bool{ \
            auto const &value = value_data.cast<Value const &>(); \
            auto const &type = type_data.cast<Type const &>(); \
            std::ignore = value; \
            std::ignore = type; \
            evaluator_body \
        } \
    )
#define SEPT__REGISTER__INHABITS__EVALUATOR_BODY(Value, Type, evaluator_body) \
    SEPT__REGISTER__INHABITS__GIVE_ID__EVALUATOR_BODY(Value, Type, __##Value##___##Type##__, evaluator_body )
#define SEPT__REGISTER__INHABITS__GIVE_ID(Value, Type, unique_id) \
    SEPT__REGISTER__INHABITS__GIVE_ID__EVALUATOR(Value, Type, unique_id, DataPredicateBinary{nullptr})
#define SEPT__REGISTER__INHABITS(Value, Type) \
    SEPT__REGISTER__INHABITS__EVALUATOR(Value, Type, DataPredicateBinary{nullptr})

bool inhabits_data (Data const &value_data, Data const &type_data);

//
// StaticAssociation_t for compare_data -- compare_data(lhs, rhs) should return:
// -    A negative value if lhs < rhs
// -    0 if lhs == rhs
// -    A positive value if lhs > rhs
//
// TODO: Change this into TotalOrder, and also implement PartialOrder
//

using CompareFunction = std::function<int(Data const &,Data const &)>;
using DataCompareEvaluatorMap = std::unordered_map<TypeIndexPair,CompareFunction>;
LVD_STATIC_ASSOCIATION_DEFINE(_Data_Compare, DataCompareEvaluatorMap)

#define SEPT__REGISTER__COMPARE__GIVE_ID__EVALUATOR(Lhs, Rhs, unique_id, evaluator) \
    LVD_STATIC_ASSOCIATION_REGISTER( \
        _Data_Compare, \
        unique_id, \
        TypeIndexPair{std::type_index(typeid(Lhs)), std::type_index(typeid(Rhs))}, \
        evaluator \
    )
#define SEPT__REGISTER__COMPARE__EVALUATOR(Lhs, Rhs, evaluator) \
    SEPT__REGISTER__COMPARE__GIVE_ID__EVALUATOR(Lhs, Rhs, __##Lhs##___##Rhs##__, evaluator)
#define SEPT__REGISTER__COMPARE__GIVE_ID__SINGLETON(Type, unique_id) \
    SEPT__REGISTER__COMPARE__GIVE_ID__EVALUATOR(Type, Type, unique_id, nullptr)
#define SEPT__REGISTER__COMPARE__SINGLETON(Type) \
    SEPT__REGISTER__COMPARE__GIVE_ID__EVALUATOR(Type, Type, Type, nullptr)
#define SEPT__REGISTER__COMPARE__GIVE_ID(Lhs, Rhs, unique_id) \
    SEPT__REGISTER__COMPARE__GIVE_ID__EVALUATOR( \
        Lhs, \
        Rhs, \
        unique_id, \
        [](Data const &lhs_data, Data const &rhs_data)->int{ \
            return compare(lhs_data.cast<Lhs const &>(), rhs_data.cast<Rhs const &>()); \
        } \
    )
#define SEPT__REGISTER__COMPARE(Lhs, Rhs) \
    SEPT__REGISTER__COMPARE__GIVE_ID(Lhs, Rhs, __##Lhs##___##Rhs##__)

int compare_data (Data const &lhs, Data const &rhs);

//
// StaticAssociation_t for serialize_data
//

using SerializeProcedure = std::function<void(Data const &value, std::ostream &out)>;
using DataSerializeProcedureMap = std::unordered_map<std::type_index,SerializeProcedure>;
LVD_STATIC_ASSOCIATION_DEFINE(_Data_Serialize, DataSerializeProcedureMap)

#define SEPT__REGISTER_SERIALIZE__GIVE_ID__EVALUATOR(Type, unique_id, evaluator) \
    LVD_STATIC_ASSOCIATION_REGISTER( \
        _Data_Serialize, \
        unique_id, \
        std::type_index(typeid(Type)), \
        evaluator \
    )
#define SEPT__REGISTER__SERIALIZE__EVALUATOR(Type, evaluator) \
    SEPT__REGISTER_SERIALIZE__GIVE_ID__EVALUATOR(Type, __##Lhs##___##Rhs##__, evaluator)
#define SEPT__REGISTER__SERIALIZE__GIVE_ID(Type, unique_id) \
    SEPT__REGISTER_SERIALIZE__GIVE_ID__EVALUATOR( \
        Type, \
        unique_id, \
        [](Data const &value_data, std::ostream &out){ \
            Type const &value = value_data.cast<Type const &>(); \
            std::ignore = value; \
            serialize(value, out); \
        } \
    )
#define SEPT__REGISTER__SERIALIZE(Type) \
    SEPT__REGISTER__SERIALIZE__GIVE_ID(Type, Type)

void serialize_data (Data const &value, std::ostream &out);

// This causes anything but an explicit Data to be passed into serialize_data.
template <typename T_>
void serialize_data (T_ const &, std::ostream &) = delete;

//
// StaticAssociation_t for deserialize_data
//

using DeserializeProcedure = std::function<Data(Data &&type, std::istream &in)>;
using DataDeserializeProcedureMap = std::unordered_map<std::type_index,DeserializeProcedure>;
LVD_STATIC_ASSOCIATION_DEFINE(DeserializeData, DataDeserializeProcedureMap)

#define SEPT__REGISTER__DESERIALIZE__GIVE_ID__EVALUATOR(Type, unique_id, evaluator) \
    LVD_STATIC_ASSOCIATION_REGISTER( \
        DeserializeData, \
        unique_id, \
        std::type_index(typeid(Type)), \
        evaluator \
    )
#define SEPT__REGISTER__DESERIALIZE__EVALUATOR(Type, evaluator) \
    SEPT__REGISTER__DESERIALIZE__GIVE_ID__EVALUATOR(Type, Type, evaluator)
#define SEPT__REGISTER__DESERIALIZE__GIVE_ID__POD(Value, Type, unique_id) \
    SEPT__REGISTER__DESERIALIZE__GIVE_ID__EVALUATOR( \
        Type, \
        unique_id, \
        [](Data &&abstract_type, std::istream &in) -> Data { \
            return SerializationForPOD<Value>::deserialize_value(in); \
        } \
    )
#define SEPT__REGISTER__DESERIALIZE__POD(Value, Type) \
    SEPT__REGISTER__DESERIALIZE__GIVE_ID__POD(Value, Type, Type)
// TODO: Maybe define a template that is specialized for each type, similar to SerializationForPOD
#define SEPT__REGISTER__DESERIALIZE(Type, evaluator_body) \
    SEPT__REGISTER__DESERIALIZE__EVALUATOR( \
        Type, \
        [](Data &&abstract_type, std::istream &in) -> Data { evaluator_body } \
    )

Data deserialize_data (std::istream &in);

//
// StaticAssociation_t for element_of_data
//

using ElementOfDataFunction = std::function<Data(Data const &container_data, Data const &param_data)>;
using DataElementOfFunctionMap = std::unordered_map<TypeIndexPair,ElementOfDataFunction>;
LVD_STATIC_ASSOCIATION_DEFINE(ElementOfData, DataElementOfFunctionMap)

#define SEPT__REGISTER__ELEMENT_OF__GIVE_ID__EVALUATOR(ContainerType, ParamType, unique_id, evaluator) \
    LVD_STATIC_ASSOCIATION_REGISTER( \
        ElementOfData, \
        unique_id, \
        TypeIndexPair{std::type_index(typeid(ContainerType)), std::type_index(typeid(ParamType))}, \
        evaluator \
    )
#define SEPT__REGISTER__ELEMENT_OF__EVALUATOR(ContainerType, ParamType, evaluator) \
    SEPT__REGISTER__ELEMENT_OF__GIVE_ID__EVALUATOR(ContainerType, ParamType, __##ContainerType##___##ParamType##__, evaluator)
// NONDATA indicates that the parameter of element_of_data will be converted to the given ParamType,
// e.g. as in `Data element_of (ArrayTerm_c const &a, uint32_t index)`.
#define SEPT__REGISTER__ELEMENT_OF__GIVE_ID__NONDATA(ContainerType, ParamType, unique_id) \
    SEPT__REGISTER__ELEMENT_OF__GIVE_ID__EVALUATOR( \
        ContainerType, \
        ParamType, \
        unique_id, \
        [](Data const &container_data, Data const &param_data) -> Data { \
            auto const &container = container_data.cast<ContainerType const &>(); \
            static_assert(!std::is_same_v<ParamType,Data>); \
            auto const &param = param_data.cast<ParamType const &>(); \
            std::ignore = container; \
            std::ignore = param; \
            return element_of(container, param); \
        } \
    )
// DATA indicates that the parameter of element_of_data will not be converted, and will be passed as Data,
// e.g. as in `Data element_of (OrderedMapTerm_c const &m, Data const &key)`.
#define SEPT__REGISTER__ELEMENT_OF__GIVE_ID__DATA(ContainerType, ParamType, unique_id) \
    SEPT__REGISTER__ELEMENT_OF__GIVE_ID__EVALUATOR( \
        ContainerType, \
        ParamType, \
        unique_id, \
        [](Data const &container_data, Data const &param_data) -> Data { \
            auto const &container = container_data.cast<ContainerType const &>(); \
            std::ignore = container; \
            static_assert(std::is_same_v<ParamType,Data>); \
            return element_of(container, param_data); \
        } \
    )
// NONDATA indicates that the parameter of element_of_data will be converted to the given ParamType,
// e.g. as in `Data element_of (ArrayTerm_c const &a, uint32_t index)`.
#define SEPT__REGISTER__ELEMENT_OF__NONDATA(ContainerType, ParamType) \
    SEPT__REGISTER__ELEMENT_OF__GIVE_ID__NONDATA(ContainerType, ParamType, __##ContainerType##___##ParamType##__)
// DATA indicates that the parameter of element_of_data will not be converted, and will be passed as Data,
// e.g. as in `Data element_of (OrderedMapTerm_c const &m, Data const &key)`.
#define SEPT__REGISTER__ELEMENT_OF__DATA(ContainerType, ParamType) \
    SEPT__REGISTER__ELEMENT_OF__GIVE_ID__DATA(ContainerType, ParamType, __##ContainerType##___##ParamType##__)

inline Data element_of_data (Data const &container_data, Data const &param_data) {
    // Look up the type pair in the evaluator map.
    auto const &evaluator_map = lvd::static_association_singleton<sept::ElementOfData>();
    auto it = evaluator_map.find(TypeIndexPair{std::type_index(container_data.type()), std::type_index(param_data.type())});
    if (it == evaluator_map.end()) {
        // TEMP HACK: Check if there's an evaluator registered that accepts Data as its param type.
        it = evaluator_map.find(TypeIndexPair{std::type_index(container_data.type()), std::type_index(typeid(Data))});
        if (it == evaluator_map.end())
            throw std::runtime_error(LVD_FMT("Data type pairs (" << container_data.type().name() << ", " << param_data.type().name() << ") and (" << container_data.type().name() << ", " << std::type_index(typeid(Data)).name() << ") are both not registered in ElementOfData for use in element_of_data"));
    }

    return it->second(container_data, param_data);
}

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
