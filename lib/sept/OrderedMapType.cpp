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

SEPT_DATA_PRINTING_REGISTER_TYPE(OrderedMapDCTerm_c)
SEPT_DATA_PRINTING_REGISTER_TYPE(OrderedMapDTerm_c)
SEPT_DATA_PRINTING_REGISTER_TYPE(OrderedMapCTerm_c)
SEPT_DATA_PRINTING_REGISTER_TYPE(OrderedMapDC_c)
SEPT_DATA_PRINTING_REGISTER_TYPE(OrderedMapD_c)
SEPT_DATA_PRINTING_REGISTER_TYPE(OrderedMapC_c)
SEPT_DATA_PRINTING_REGISTER_TYPE(OrderedMap_c)
SEPT_DATA_PRINTING_REGISTER_TYPE(OrderedMapType_c)

SEPT_EQ_DATA_REGISTER_TYPE(OrderedMapDCTerm_c)
SEPT_EQ_DATA_REGISTER_TYPE(OrderedMapDTerm_c)
SEPT_EQ_DATA_REGISTER_TYPE(OrderedMapCTerm_c)
SEPT_EQ_DATA_REGISTER_TYPE(OrderedMapDC_c)
SEPT_EQ_DATA_REGISTER_TYPE(OrderedMapD_c)
SEPT_EQ_DATA_REGISTER_TYPE(OrderedMapC_c)
SEPT_EQ_DATA_REGISTER_TYPE(OrderedMap_c)
SEPT_EQ_DATA_REGISTER_TYPE(OrderedMapType_c)


SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_DEFAULT(OrderedMapDC_c)
SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_DEFAULT(OrderedMapD_c)
SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_DEFAULT(OrderedMapC_c)
SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_DEFAULT(OrderedMap_c)

SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_DEFAULT(OrderedMapType_c)

SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_DEFAULT(OrderedMapDCTerm_c)
SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_DEFAULT(OrderedMapDTerm_c)
SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_DEFAULT(OrderedMapCTerm_c)

SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_DEFAULT(OrderedMapTerm_c)


SEPT_INHABITS_DATA_REGISTER_TYPE_TRIVIAL(OrderedMapDC_c, Type_c)
SEPT_INHABITS_DATA_REGISTER_TYPE_TRIVIAL(OrderedMapD_c, Type_c)
SEPT_INHABITS_DATA_REGISTER_TYPE_TRIVIAL(OrderedMapC_c, Type_c)
SEPT_INHABITS_DATA_REGISTER_TYPE_TRIVIAL(OrderedMap_c, Type_c)

SEPT_INHABITS_DATA_REGISTER_TYPE_TRIVIAL(OrderedMapDC_c, OrderedMapType_c)
SEPT_INHABITS_DATA_REGISTER_TYPE_TRIVIAL(OrderedMapD_c, OrderedMapType_c)
SEPT_INHABITS_DATA_REGISTER_TYPE_TRIVIAL(OrderedMapC_c, OrderedMapType_c)
SEPT_INHABITS_DATA_REGISTER_TYPE_TRIVIAL(OrderedMap_c, OrderedMapType_c)

SEPT_INHABITS_DATA_REGISTER_TYPE_TRIVIAL(OrderedMapDCTerm_c, OrderedMapDC_c)
SEPT_INHABITS_DATA_REGISTER_TYPE_TRIVIAL(OrderedMapDTerm_c, OrderedMapD_c)
SEPT_INHABITS_DATA_REGISTER_TYPE_TRIVIAL(OrderedMapCTerm_c, OrderedMapC_c)

// NOTE: This is a type-widening inhabitation; it's more accurate to say that there's a lossless, but type-widening conversion.
SEPT_INHABITS_DATA_REGISTER_TYPE(OrderedMapTerm_c, OrderedMapDCTerm_c, return type.constraint_is_satisfied(value.pairs()); )
SEPT_INHABITS_DATA_REGISTER_TYPE(OrderedMapTerm_c, OrderedMapDTerm_c, return type.constraint_is_satisfied(value.pairs()); )
SEPT_INHABITS_DATA_REGISTER_TYPE(OrderedMapTerm_c, OrderedMapCTerm_c, return type.constraint_is_satisfied(value.pairs()); )
SEPT_INHABITS_DATA_REGISTER_TYPE(OrderedMapTerm_c, OrderedMap_c, return type.constraint_is_satisfied(value.pairs()); )


SEPT_COMPARE_DATA_REGISTER_SINGLETON_TYPE(OrderedMapType_c)
SEPT_COMPARE_DATA_REGISTER_SINGLETON_TYPE(OrderedMapDC_c)
SEPT_COMPARE_DATA_REGISTER_SINGLETON_TYPE(OrderedMapD_c)
SEPT_COMPARE_DATA_REGISTER_SINGLETON_TYPE(OrderedMapC_c)
SEPT_COMPARE_DATA_REGISTER_SINGLETON_TYPE(OrderedMap_c)
SEPT_COMPARE_DATA_REGISTER_DEFAULT_TYPE(OrderedMapDCTerm_c, OrderedMapDCTerm_c)
SEPT_COMPARE_DATA_REGISTER_DEFAULT_TYPE(OrderedMapDTerm_c, OrderedMapDTerm_c)
SEPT_COMPARE_DATA_REGISTER_DEFAULT_TYPE(OrderedMapCTerm_c, OrderedMapCTerm_c)


SEPT_SERIALIZE_DATA_REGISTER_TYPE_DEFAULT(OrderedMapType_c)
SEPT_SERIALIZE_DATA_REGISTER_TYPE_DEFAULT(OrderedMapDC_c)
SEPT_SERIALIZE_DATA_REGISTER_TYPE_DEFAULT(OrderedMapD_c)
SEPT_SERIALIZE_DATA_REGISTER_TYPE_DEFAULT(OrderedMapC_c)
SEPT_SERIALIZE_DATA_REGISTER_TYPE_DEFAULT(OrderedMap_c)
SEPT_SERIALIZE_DATA_REGISTER_TYPE_DEFAULT(OrderedMapDCTerm_c)
SEPT_SERIALIZE_DATA_REGISTER_TYPE_DEFAULT(OrderedMapDTerm_c)
SEPT_SERIALIZE_DATA_REGISTER_TYPE_DEFAULT(OrderedMapCTerm_c)

} // end namespace sept
