// 2020.03.13 - Victor Dods

#include "sept/ArrayType.hpp"

#include <lvd/OstreamDelegate.hpp>
#include "sept/abstract_type_of.hpp"
#include "sept/ArrayTerm.hpp"
#include "sept/is_instance.hpp"
#include <sstream> // Needed by LVD_FMT

namespace sept {

Array_c Array;
ArrayType_c ArrayType;
ArrayES_c ArrayES;
ArrayE_c ArrayE;
ArrayS_c ArrayS;

bool ArrayESTerm_c::constraint_is_satisfied (DataVector const &elements) const {
    if (m_size != elements.size())
        return false;
    for (size_t i = 0; i < elements.size(); ++i)
        if (!is_instance(elements[i], m_element_type))
            return false;
    return true;
}

void ArrayESTerm_c::verify_constraint_or_throw (DataVector const &elements) const {
    if (m_size != elements.size())
        throw std::runtime_error(LVD_FMT("expected " << m_size << " elements, but there were " << elements.size()));
    for (size_t i = 0; i < elements.size(); ++i)
        if (!is_instance(elements[i], m_element_type))
            throw std::runtime_error(LVD_FMT("element " << i << " had type " << abstract_type_of_data(elements[i]) << " but was expected to be of type " << m_element_type));
}

bool ArrayETerm_c::constraint_is_satisfied (DataVector const &elements) const {
    for (size_t i = 0; i < elements.size(); ++i)
        if (!is_instance(elements[i], m_element_type))
            return false;
    return true;
}

void ArrayETerm_c::verify_constraint_or_throw (DataVector const &elements) const {
    for (size_t i = 0; i < elements.size(); ++i)
        if (!is_instance(elements[i], m_element_type))
            throw std::runtime_error(LVD_FMT("element " << i << " had type " << abstract_type_of_data(elements[i]) << " but was expected to be of type " << m_element_type));
}

bool ArraySTerm_c::constraint_is_satisfied (DataVector const &elements) const {
    return m_size == elements.size();
}

void ArraySTerm_c::verify_constraint_or_throw (DataVector const &elements) const {
    if (m_size != elements.size())
        throw std::runtime_error(LVD_FMT("expected " << m_size << " elements, but there were " << elements.size()));
}

bool is_instance (ArrayTerm_c const &a, ArrayESTerm_c const &t) {
    return a.element_type() == t.element_type() && a.size() == t.size();
}

bool is_instance (ArrayTerm_c const &a, ArrayETerm_c const &t) {
    return a.element_type() == t.element_type();
}

bool is_instance (ArrayTerm_c const &a, ArraySTerm_c const &t) {
    return a.size() == t.size();
}

bool is_instance (ArrayTerm_c const &a, Array_c const &t) {
    return true;
}

bool is_instance (Data const &value, ArrayESTerm_c const &t) {
    return value.can_cast<ArrayTerm_c>() && is_instance(value.cast<ArrayTerm_c const &>(), t);
}

bool is_instance (Data const &value, ArrayETerm_c const &t) {
    return value.can_cast<ArrayTerm_c>() && is_instance(value.cast<ArrayTerm_c const &>(), t);
}

bool is_instance (Data const &value, ArraySTerm_c const &t) {
    return value.can_cast<ArrayTerm_c>() && is_instance(value.cast<ArrayTerm_c const &>(), t);
}

bool is_instance (Data const &value, Array_c const &t) {
    return value.can_cast<ArrayTerm_c>() && is_instance(value.cast<ArrayTerm_c const &>(), t);
}

void serialize (ArrayESTerm_c const &v, std::ostream &out) {
    serialize(SerializedTopLevelCode::PARAMETRIC_TERM, out);
    serialize(ArrayES, out);
    serialize_data(v.element_type(), out);
    serialize(v.size(), out);
}

void serialize (ArrayETerm_c const &v, std::ostream &out) {
    serialize(SerializedTopLevelCode::PARAMETRIC_TERM, out);
    serialize(ArrayE, out);
    serialize_data(v.element_type(), out);
}

void serialize (ArraySTerm_c const &v, std::ostream &out) {
    serialize(SerializedTopLevelCode::PARAMETRIC_TERM, out);
    serialize(ArrayS, out);
    serialize(v.size(), out);
}

ArrayESTerm_c deserialize_value_ArrayESTerm (Data &&abstract_type, std::istream &in) {
    auto element_type = deserialize_data(in);
    auto size = deserialize_data(in);
    // TODO: Make this uint64_t instead, so it's not architecture dependent.
    if (size.type() != typeid(size_t))
        throw std::runtime_error(LVD_FMT("deserialize_value_ArrayESTerm expected array size to be size_t, but it was " << size.type()));

    return ArrayESTerm_c(std::move(element_type), size.cast<size_t>());
}

ArrayETerm_c deserialize_value_ArrayETerm (Data &&abstract_type, std::istream &in) {
    auto element_type = deserialize_data(in);
    return ArrayETerm_c(std::move(element_type));
}

ArraySTerm_c deserialize_value_ArraySTerm (Data &&abstract_type, std::istream &in) {
    auto size = deserialize_data(in);
    // TODO: Make this uint64_t instead, so it's not architecture dependent.
    if (size.type() != typeid(size_t))
        throw std::runtime_error(LVD_FMT("deserialize_value_ArraySTerm expected array size to be size_t, but it was " << size.type()));

    return ArraySTerm_c(size.cast<size_t>());
}

} // end namespace sept
