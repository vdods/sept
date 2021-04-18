// 2020.03.13 - Victor Dods

#include "sept/ArrayType.hpp"

#include <lvd/OstreamDelegate.hpp>
#include "sept/ArrayTerm.hpp"
#include <sstream> // Needed by LVD_FMT

namespace sept {

Array_c Array;
ArrayType_c ArrayType;
ArrayES_c ArrayES;
ArrayE_c ArrayE;
ArrayS_c ArrayS;

//
// ArrayESTerm_c
//

bool ArrayESTerm_c::constraint_is_satisfied (DataVector const &elements) const {
    if (m_size != elements.size())
        return false;
    for (size_t i = 0; i < elements.size(); ++i)
        if (!inhabits_data(elements[i], m_element_type))
            return false;
    return true;
}

void ArrayESTerm_c::verify_constraint_or_throw (DataVector const &elements) const {
    if (m_size != elements.size())
        throw std::runtime_error(LVD_FMT("expected " << m_size << " elements, but there were " << elements.size()));
    for (size_t i = 0; i < elements.size(); ++i)
        if (!inhabits_data(elements[i], m_element_type))
            throw std::runtime_error(LVD_FMT("element " << i << " had type " << abstract_type_of_data(elements[i]) << " but was expected to be of type " << m_element_type));
}

ArrayESTerm_c::operator lvd::OstreamDelegate () const {
    return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
        DataPrintCtx ctx;
        print(out, ctx, *this);
    });
}

//
// ArrayETerm_c
//

bool ArrayETerm_c::constraint_is_satisfied (DataVector const &elements) const {
    for (size_t i = 0; i < elements.size(); ++i)
        if (!inhabits_data(elements[i], m_element_type))
            return false;
    return true;
}

void ArrayETerm_c::verify_constraint_or_throw (DataVector const &elements) const {
    for (size_t i = 0; i < elements.size(); ++i)
        if (!inhabits_data(elements[i], m_element_type))
            throw std::runtime_error(LVD_FMT("element " << i << " had type " << abstract_type_of_data(elements[i]) << " but was expected to be of type " << m_element_type));
}

ArrayETerm_c::operator lvd::OstreamDelegate () const {
    return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
        DataPrintCtx ctx;
        print(out, ctx, *this);
    });
}

//
// ArraySTerm_c
//

bool ArraySTerm_c::constraint_is_satisfied (DataVector const &elements) const {
    return m_size == elements.size();
}

void ArraySTerm_c::verify_constraint_or_throw (DataVector const &elements) const {
    if (m_size != elements.size())
        throw std::runtime_error(LVD_FMT("expected " << m_size << " elements, but there were " << elements.size()));
}

ArraySTerm_c::operator lvd::OstreamDelegate () const {
    return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
        DataPrintCtx ctx;
        print(out, ctx, *this);
    });
}

//
// Other stuff
//

bool inhabits (ArrayTerm_c const &a, ArrayESTerm_c const &t) {
    if (a.size() != t.size())
        return false;
    // Check the constraint -- if that's satisfied, then we're good.
    if (a.element_type() == t.element_type())
        return true;
    // Otherwise check inhabitation of elements.
    for (auto const &a_element : a.elements())
        if (!inhabits_data(a_element, t.element_type()))
            return false;
    // Passed all checks
    return true;
}

bool inhabits (ArrayTerm_c const &a, ArrayETerm_c const &t) {
    // Check the constraint -- if that's satisfied, then we're good.
    if (a.element_type() == t.element_type())
        return true;
    // Otherwise check inhabitation of elements.
    for (auto const &a_element : a.elements())
        if (!inhabits_data(a_element, t.element_type()))
            return false;
    // Passed all checks
    return true;
}

bool inhabits (ArrayTerm_c const &a, ArraySTerm_c const &t) {
    return a.size() == t.size();
}

bool inhabits (ArrayTerm_c const &a, Array_c const &t) {
    return true;
}

bool inhabits (Data const &value, ArrayESTerm_c const &t) {
    return value.can_cast<ArrayTerm_c>() && inhabits(value.cast<ArrayTerm_c const &>(), t);
}

bool inhabits (Data const &value, ArrayETerm_c const &t) {
    return value.can_cast<ArrayTerm_c>() && inhabits(value.cast<ArrayTerm_c const &>(), t);
}

bool inhabits (Data const &value, ArraySTerm_c const &t) {
    return value.can_cast<ArrayTerm_c>() && inhabits(value.cast<ArrayTerm_c const &>(), t);
}

bool inhabits (Data const &value, Array_c const &t) {
    return value.can_cast<ArrayTerm_c>() && inhabits(value.cast<ArrayTerm_c const &>(), t);
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

//
// Registrations for Data functions
//

SEPT__REGISTER__PRINT(ArrayESTerm_c)
SEPT__REGISTER__PRINT(ArrayETerm_c)
SEPT__REGISTER__PRINT(ArraySTerm_c)
SEPT__REGISTER__PRINT(ArrayES_c)
SEPT__REGISTER__PRINT(ArrayE_c)
SEPT__REGISTER__PRINT(ArrayS_c)
SEPT__REGISTER__PRINT(Array_c)
SEPT__REGISTER__PRINT(ArrayType_c)

SEPT__REGISTER__EQ(ArrayESTerm_c)
SEPT__REGISTER__EQ(ArrayETerm_c)
SEPT__REGISTER__EQ(ArraySTerm_c)
SEPT__REGISTER__EQ(ArrayES_c)
SEPT__REGISTER__EQ(ArrayE_c)
SEPT__REGISTER__EQ(ArrayS_c)
SEPT__REGISTER__EQ(Array_c)
SEPT__REGISTER__EQ(ArrayType_c)


SEPT__REGISTER__ABSTRACT_TYPE_OF(ArrayES_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(ArrayE_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(ArrayS_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(Array_c)

SEPT__REGISTER__ABSTRACT_TYPE_OF(ArrayType_c)

SEPT__REGISTER__ABSTRACT_TYPE_OF(ArrayESTerm_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(ArrayETerm_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(ArraySTerm_c)

SEPT__REGISTER__ABSTRACT_TYPE_OF(ArrayTerm_c)


SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(ArrayES_c, Type_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(ArrayE_c, Type_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(ArrayS_c, Type_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(Array_c, Type_c)

SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(ArrayES_c, ArrayType_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(ArrayE_c, ArrayType_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(ArrayS_c, ArrayType_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(Array_c, ArrayType_c)

SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(ArrayESTerm_c, ArrayES_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(ArrayETerm_c, ArrayE_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(ArraySTerm_c, ArrayS_c)

// NOTE: These are type-widening inhabitation; it's more accurate to say that there are lossless,
// but type-widening conversions
SEPT__REGISTER__INHABITS__NONDATA(ArrayTerm_c, ArrayESTerm_c)
SEPT__REGISTER__INHABITS__NONDATA(ArrayTerm_c, ArrayETerm_c)
SEPT__REGISTER__INHABITS__NONDATA(ArrayTerm_c, ArraySTerm_c)
// This one isn't type-widening.
SEPT__REGISTER__INHABITS__NONDATA(ArrayTerm_c, Array_c)


SEPT__REGISTER__COMPARE__SINGLETON(ArrayType_c)
SEPT__REGISTER__COMPARE__SINGLETON(ArrayES_c)
SEPT__REGISTER__COMPARE__SINGLETON(ArrayE_c)
SEPT__REGISTER__COMPARE__SINGLETON(ArrayS_c)
SEPT__REGISTER__COMPARE__SINGLETON(Array_c)
SEPT__REGISTER__COMPARE(ArrayESTerm_c, ArrayESTerm_c)
SEPT__REGISTER__COMPARE(ArrayETerm_c, ArrayETerm_c)
SEPT__REGISTER__COMPARE(ArraySTerm_c, ArraySTerm_c)


SEPT__REGISTER__SERIALIZE(ArrayType_c)
SEPT__REGISTER__SERIALIZE(ArrayES_c)
SEPT__REGISTER__SERIALIZE(ArrayE_c)
SEPT__REGISTER__SERIALIZE(ArrayS_c)
SEPT__REGISTER__SERIALIZE(Array_c)
SEPT__REGISTER__SERIALIZE(ArrayESTerm_c)
SEPT__REGISTER__SERIALIZE(ArrayETerm_c)
SEPT__REGISTER__SERIALIZE(ArraySTerm_c)


SEPT__REGISTER__DESERIALIZE(ArrayES_c, return deserialize_value_ArrayESTerm(std::move(abstract_type), in);)
SEPT__REGISTER__DESERIALIZE(ArrayE_c, return deserialize_value_ArrayETerm(std::move(abstract_type), in);)
SEPT__REGISTER__DESERIALIZE(ArrayS_c, return deserialize_value_ArraySTerm(std::move(abstract_type), in);)

SEPT__REGISTER__DESERIALIZE(ArrayESTerm_c, return deserialize_value_ArrayTerm(std::move(abstract_type), in);)
SEPT__REGISTER__DESERIALIZE(ArrayETerm_c, return deserialize_value_ArrayTerm(std::move(abstract_type), in);)
SEPT__REGISTER__DESERIALIZE(ArraySTerm_c, return deserialize_value_ArrayTerm(std::move(abstract_type), in);)
SEPT__REGISTER__DESERIALIZE(Array_c, return deserialize_value_ArrayTerm(std::move(abstract_type), in);)


SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(ArrayESTerm_c, ArrayTerm_c)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(ArrayETerm_c, ArrayTerm_c)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(ArraySTerm_c, ArrayTerm_c)

} // end namespace sept
