// 2020.07.15 - Victor Dods

#include "sept/OrderedMapType.hpp"

#include "sept/OrderedMapTerm.hpp"
#include <sstream> // Needed by LVD_FMT

namespace sept {

OrderedMap_c OrderedMap;
OrderedMapType_c OrderedMapType;
OrderedMapDC_c OrderedMapDC;
OrderedMapD_c OrderedMapD;
OrderedMapC_c OrderedMapC;

//
// OrderedMapDCTerm_c
//

bool OrderedMapDCTerm_c::constraint_is_satisfied (DataOrderedMap const &m) const {
    for (auto const &pair : m) {
        if (!inhabits_data(pair.first, m_domain))
            return false;
        if (!inhabits_data(pair.second, m_codomain))
            return false;
    }
    return true;
}

void OrderedMapDCTerm_c::verify_constraint_or_throw (DataOrderedMap const &m) const {
    for (auto const &pair : m) {
        if (!inhabits_data(pair.first, m_domain))
            throw std::runtime_error(LVD_FMT("key " << pair.first << " had type " << abstract_type_of_data(pair.first) << " but was expected to be of type " << m_domain));
        if (!inhabits_data(pair.second, m_codomain))
            throw std::runtime_error(LVD_FMT("value " << pair.second << " had type " << abstract_type_of_data(pair.second) << " but was expected to be of type " << m_codomain));
    }
}

OrderedMapDCTerm_c::operator lvd::OstreamDelegate () const {
    return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
        DataPrintCtx ctx;
        print(out, ctx, *this);
    });
}

//
// OrderedMapDTerm_c
//

bool OrderedMapDTerm_c::constraint_is_satisfied (DataOrderedMap const &m) const {
    for (auto const &pair : m)
        if (!inhabits_data(pair.first, m_domain))
            return false;
    return true;
}

void OrderedMapDTerm_c::verify_constraint_or_throw (DataOrderedMap const &m) const {
    for (auto const &pair : m)
        if (!inhabits_data(pair.first, m_domain))
            throw std::runtime_error(LVD_FMT("key " << pair.first << " had type " << abstract_type_of_data(pair.first) << " but was expected to be of type " << m_domain));
}

OrderedMapDTerm_c::operator lvd::OstreamDelegate () const {
    return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
        DataPrintCtx ctx;
        print(out, ctx, *this);
    });
}

//
// OrderedMapCTerm_c
//

bool OrderedMapCTerm_c::constraint_is_satisfied (DataOrderedMap const &m) const {
    for (auto const &pair : m)
        if (!inhabits_data(pair.second, m_codomain))
            return false;
    return true;
}

void OrderedMapCTerm_c::verify_constraint_or_throw (DataOrderedMap const &m) const {
    for (auto const &pair : m)
        if (!inhabits_data(pair.second, m_codomain))
            throw std::runtime_error(LVD_FMT("value " << pair.second << " had type " << abstract_type_of_data(pair.second) << " but was expected to be of type " << m_codomain));
}

OrderedMapCTerm_c::operator lvd::OstreamDelegate () const {
    return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
        DataPrintCtx ctx;
        print(out, ctx, *this);
    });
}

//
// Other stuff
//

bool inhabits (OrderedMapTerm_c const &m, OrderedMapDCTerm_c const &t) {
    return m.domain() == t.domain() && m.codomain() == t.codomain();
}

bool inhabits (OrderedMapTerm_c const &m, OrderedMapDTerm_c const &t) {
    return m.domain() == t.domain();
}

bool inhabits (OrderedMapTerm_c const &m, OrderedMapCTerm_c const &t) {
    return m.codomain() == t.codomain();
}

bool inhabits (OrderedMapTerm_c const &m, OrderedMap_c const &t) {
    return true;
}

bool inhabits (Data const &value, OrderedMapDCTerm_c const &t) {
    return value.can_cast<OrderedMapTerm_c>() && inhabits(value.cast<OrderedMapTerm_c const &>(), t);
}

bool inhabits (Data const &value, OrderedMapDTerm_c const &t) {
    return value.can_cast<OrderedMapTerm_c>() && inhabits(value.cast<OrderedMapTerm_c const &>(), t);
}

bool inhabits (Data const &value, OrderedMapCTerm_c const &t) {
    return value.can_cast<OrderedMapTerm_c>() && inhabits(value.cast<OrderedMapTerm_c const &>(), t);
}

bool inhabits (Data const &value, OrderedMap_c const &t) {
    return value.can_cast<OrderedMapTerm_c>() && inhabits(value.cast<OrderedMapTerm_c const &>(), t);
}

void serialize (OrderedMapDCTerm_c const &v, std::ostream &out) {
    serialize(SerializedTopLevelCode::PARAMETRIC_TERM, out);
    serialize(OrderedMapDC, out);
    serialize_data(v.domain(), out);
    serialize_data(v.codomain(), out);
}

void serialize (OrderedMapDTerm_c const &v, std::ostream &out) {
    serialize(SerializedTopLevelCode::PARAMETRIC_TERM, out);
    serialize(OrderedMapD, out);
    serialize_data(v.domain(), out);
}

void serialize (OrderedMapCTerm_c const &v, std::ostream &out) {
    serialize(SerializedTopLevelCode::PARAMETRIC_TERM, out);
    serialize(OrderedMapC, out);
    serialize_data(v.codomain(), out);
}

OrderedMapDCTerm_c deserialize_value_OrderedMapDCTerm (Data &&abstract_type, std::istream &in) {
    auto domain = deserialize_data(in);
    auto codomain = deserialize_data(in);
    return OrderedMapDCTerm_c(std::move(domain), std::move(codomain));
}

OrderedMapDTerm_c deserialize_value_OrderedMapDTerm (Data &&abstract_type, std::istream &in) {
    auto domain = deserialize_data(in);
    return OrderedMapDTerm_c(std::move(domain));
}

OrderedMapCTerm_c deserialize_value_OrderedMapCTerm (Data &&abstract_type, std::istream &in) {
    auto codomain = deserialize_data(in);
    return OrderedMapCTerm_c(std::move(codomain));
}

//
// Registrations for Data functions
//

SEPT__REGISTER__PRINT(OrderedMapDCTerm_c)
SEPT__REGISTER__PRINT(OrderedMapDTerm_c)
SEPT__REGISTER__PRINT(OrderedMapCTerm_c)
SEPT__REGISTER__PRINT(OrderedMapDC_c)
SEPT__REGISTER__PRINT(OrderedMapD_c)
SEPT__REGISTER__PRINT(OrderedMapC_c)
SEPT__REGISTER__PRINT(OrderedMap_c)
SEPT__REGISTER__PRINT(OrderedMapType_c)

SEPT__REGISTER__EQ(OrderedMapDCTerm_c)
SEPT__REGISTER__EQ(OrderedMapDTerm_c)
SEPT__REGISTER__EQ(OrderedMapCTerm_c)
SEPT__REGISTER__EQ(OrderedMapDC_c)
SEPT__REGISTER__EQ(OrderedMapD_c)
SEPT__REGISTER__EQ(OrderedMapC_c)
SEPT__REGISTER__EQ(OrderedMap_c)
SEPT__REGISTER__EQ(OrderedMapType_c)


SEPT__REGISTER__ABSTRACT_TYPE_OF(OrderedMapDC_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(OrderedMapD_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(OrderedMapC_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(OrderedMap_c)

SEPT__REGISTER__ABSTRACT_TYPE_OF(OrderedMapType_c)

SEPT__REGISTER__ABSTRACT_TYPE_OF(OrderedMapDCTerm_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(OrderedMapDTerm_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(OrderedMapCTerm_c)

SEPT__REGISTER__ABSTRACT_TYPE_OF(OrderedMapTerm_c)


SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(OrderedMapDC_c, Type_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(OrderedMapD_c, Type_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(OrderedMapC_c, Type_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(OrderedMap_c, Type_c)

SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(OrderedMapDC_c, OrderedMapType_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(OrderedMapD_c, OrderedMapType_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(OrderedMapC_c, OrderedMapType_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(OrderedMap_c, OrderedMapType_c)

SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(OrderedMapDCTerm_c, OrderedMapDC_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(OrderedMapDTerm_c, OrderedMapD_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(OrderedMapCTerm_c, OrderedMapC_c)

// NOTE: These are type-widening inhabitation; it's more accurate to say that there are lossless,
// but type-widening conversions
SEPT__REGISTER__INHABITS__NONDATA(OrderedMapTerm_c, OrderedMapDCTerm_c)
SEPT__REGISTER__INHABITS__NONDATA(OrderedMapTerm_c, OrderedMapDTerm_c)
SEPT__REGISTER__INHABITS__NONDATA(OrderedMapTerm_c, OrderedMapCTerm_c)
// This one isn't type-widening.
SEPT__REGISTER__INHABITS__NONDATA(OrderedMapTerm_c, OrderedMap_c)


SEPT__REGISTER__COMPARE__SINGLETON(OrderedMapType_c)
SEPT__REGISTER__COMPARE__SINGLETON(OrderedMapDC_c)
SEPT__REGISTER__COMPARE__SINGLETON(OrderedMapD_c)
SEPT__REGISTER__COMPARE__SINGLETON(OrderedMapC_c)
SEPT__REGISTER__COMPARE__SINGLETON(OrderedMap_c)
SEPT__REGISTER__COMPARE(OrderedMapDCTerm_c, OrderedMapDCTerm_c)
SEPT__REGISTER__COMPARE(OrderedMapDTerm_c, OrderedMapDTerm_c)
SEPT__REGISTER__COMPARE(OrderedMapCTerm_c, OrderedMapCTerm_c)


SEPT__REGISTER__SERIALIZE(OrderedMapType_c)
SEPT__REGISTER__SERIALIZE(OrderedMapDC_c)
SEPT__REGISTER__SERIALIZE(OrderedMapD_c)
SEPT__REGISTER__SERIALIZE(OrderedMapC_c)
SEPT__REGISTER__SERIALIZE(OrderedMap_c)
SEPT__REGISTER__SERIALIZE(OrderedMapDCTerm_c)
SEPT__REGISTER__SERIALIZE(OrderedMapDTerm_c)
SEPT__REGISTER__SERIALIZE(OrderedMapCTerm_c)


SEPT__REGISTER__DESERIALIZE(OrderedMapDC_c, return deserialize_value_OrderedMapDCTerm(std::move(abstract_type), in);)
SEPT__REGISTER__DESERIALIZE(OrderedMapD_c, return deserialize_value_OrderedMapDTerm(std::move(abstract_type), in);)
SEPT__REGISTER__DESERIALIZE(OrderedMapC_c, return deserialize_value_OrderedMapCTerm(std::move(abstract_type), in);)

SEPT__REGISTER__DESERIALIZE(OrderedMapDCTerm_c, return deserialize_value_OrderedMapTerm(std::move(abstract_type), in);)
SEPT__REGISTER__DESERIALIZE(OrderedMapDTerm_c, return deserialize_value_OrderedMapTerm(std::move(abstract_type), in);)
SEPT__REGISTER__DESERIALIZE(OrderedMapCTerm_c, return deserialize_value_OrderedMapTerm(std::move(abstract_type), in);)
SEPT__REGISTER__DESERIALIZE(OrderedMap_c, return deserialize_value_OrderedMapTerm(std::move(abstract_type), in);)

} // end namespace sept
