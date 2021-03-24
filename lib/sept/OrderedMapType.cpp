// 2020.07.15 - Victor Dods

#include "sept/OrderedMapType.hpp"

#include "sept/abstract_type_of.hpp"
#include "sept/is_instance.hpp"
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
        if (!is_instance(pair.first, m_domain))
            return false;
        if (!is_instance(pair.second, m_codomain))
            return false;
    }
    return true;
}

void OrderedMapDCTerm_c::verify_constraint_or_throw (DataOrderedMap const &m) const {
    for (auto const &pair : m) {
        if (!is_instance(pair.first, m_domain))
            throw std::runtime_error(LVD_FMT("key " << pair.first << " had type " << abstract_type_of_data(pair.first) << " but was expected to be of type " << m_domain));
        if (!is_instance(pair.second, m_codomain))
            throw std::runtime_error(LVD_FMT("value " << pair.second << " had type " << abstract_type_of_data(pair.second) << " but was expected to be of type " << m_codomain));
    }
}

bool OrderedMapDTerm_c::constraint_is_satisfied (DataOrderedMap const &m) const {
    for (auto const &pair : m)
        if (!is_instance(pair.first, m_domain))
            return false;
    return true;
}

void OrderedMapDTerm_c::verify_constraint_or_throw (DataOrderedMap const &m) const {
    for (auto const &pair : m)
        if (!is_instance(pair.first, m_domain))
            throw std::runtime_error(LVD_FMT("key " << pair.first << " had type " << abstract_type_of_data(pair.first) << " but was expected to be of type " << m_domain));
}

bool OrderedMapCTerm_c::constraint_is_satisfied (DataOrderedMap const &m) const {
    for (auto const &pair : m)
        if (!is_instance(pair.second, m_codomain))
            return false;
    return true;
}

void OrderedMapCTerm_c::verify_constraint_or_throw (DataOrderedMap const &m) const {
    for (auto const &pair : m)
        if (!is_instance(pair.second, m_codomain))
            throw std::runtime_error(LVD_FMT("value " << pair.second << " had type " << abstract_type_of_data(pair.second) << " but was expected to be of type " << m_codomain));
}

bool is_instance (OrderedMapTerm_c const &m, OrderedMapDCTerm_c const &t) {
    return m.domain() == t.domain() && m.codomain() == t.codomain();
}

bool is_instance (OrderedMapTerm_c const &m, OrderedMapDTerm_c const &t) {
    return m.domain() == t.domain();
}

bool is_instance (OrderedMapTerm_c const &m, OrderedMapCTerm_c const &t) {
    return m.codomain() == t.codomain();
}

bool is_instance (OrderedMapTerm_c const &m, OrderedMap_c const &t) {
    return true;
}

bool is_instance (Data const &value, OrderedMapDCTerm_c const &t) {
    return value.can_cast<OrderedMapTerm_c>() && is_instance(value.cast<OrderedMapTerm_c const &>(), t);
}

bool is_instance (Data const &value, OrderedMapDTerm_c const &t) {
    return value.can_cast<OrderedMapTerm_c>() && is_instance(value.cast<OrderedMapTerm_c const &>(), t);
}

bool is_instance (Data const &value, OrderedMapCTerm_c const &t) {
    return value.can_cast<OrderedMapTerm_c>() && is_instance(value.cast<OrderedMapTerm_c const &>(), t);
}

bool is_instance (Data const &value, OrderedMap_c const &t) {
    return value.can_cast<OrderedMapTerm_c>() && is_instance(value.cast<OrderedMapTerm_c const &>(), t);
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

} // end namespace sept
