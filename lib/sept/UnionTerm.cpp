// 2021.03.30 - Victor Dods

#include "sept/UnionTerm.hpp"

#include "sept/NPTerm.hpp"
#include "sept/Union.hpp"

namespace sept {

void serialize (UnionTerm_c const &t, std::ostream &out) {
    serialize(SerializedTopLevelCode::PARAMETRIC_TERM, out);
    serialize(UnionType, out);
    serialize(t.size(), out);
    for (auto const &element : t.elements())
        serialize_data(element, out);
}

UnionTerm_c deserialize_value_UnionTerm (Data &&abstract_type, std::istream &in) {
    // TODO: Delegate to BaseArray_t

    size_t element_count = 0;

    auto size = deserialize_data(in);
    // TODO: Make this uint64_t instead, so it's not architecture dependent.
    if (size.type() != typeid(size_t))
        throw std::runtime_error(LVD_FMT("deserialize_value_UnionTerm expected array size to be size_t, but it was " << size.type()));
    element_count = size.cast<size_t>();

    DataVector elements;
    elements.reserve(element_count);
    for (size_t i = 0; i < element_count; ++i)
        elements.emplace_back(deserialize_data(in));

    return UnionTerm_c(std::move(elements));
}

// Do fancy "from the end" indexing for negative numbers.  An index of -1 will be the last element
// and an index of -t.size() will be the first element.  However, an index of -t.size()-1 will
// throw std::out_of_range.
Data element_of (UnionTerm_c const &t, int8_t index) { return index >= 0 ? t[index] : t[t.size()+size_t(index)]; }
Data element_of (UnionTerm_c const &t, int16_t index) { return index >= 0 ? t[index] : t[t.size()+size_t(index)]; }
Data element_of (UnionTerm_c const &t, int32_t index) { return index >= 0 ? t[index] : t[t.size()+size_t(index)]; }
Data element_of (UnionTerm_c const &t, int64_t index) { return index >= 0 ? t[index] : t[t.size()+size_t(index)]; }

Data element_of (UnionTerm_c const &t, uint8_t index) { return t[index]; }
Data element_of (UnionTerm_c const &t, uint16_t index) { return t[index]; }
Data element_of (UnionTerm_c const &t, uint32_t index) { return t[index]; }
Data element_of (UnionTerm_c const &t, uint64_t index) { return t[index]; }

//
// Registrations for Data functions
//

SEPT__REGISTER__PRINT(UnionTerm_c)

SEPT__REGISTER__EQ(UnionTerm_c)

SEPT__REGISTER__COMPARE(UnionTerm_c, UnionTerm_c)

SEPT__REGISTER__SERIALIZE(UnionTerm_c)

SEPT__REGISTER__ELEMENT_OF__NONDATA(UnionTerm_c, int8_t)
SEPT__REGISTER__ELEMENT_OF__NONDATA(UnionTerm_c, int16_t)
SEPT__REGISTER__ELEMENT_OF__NONDATA(UnionTerm_c, int32_t)
SEPT__REGISTER__ELEMENT_OF__NONDATA(UnionTerm_c, int64_t)
SEPT__REGISTER__ELEMENT_OF__NONDATA(UnionTerm_c, uint8_t)
SEPT__REGISTER__ELEMENT_OF__NONDATA(UnionTerm_c, uint16_t)
SEPT__REGISTER__ELEMENT_OF__NONDATA(UnionTerm_c, uint32_t)
SEPT__REGISTER__ELEMENT_OF__NONDATA(UnionTerm_c, uint64_t)

// TODO: This needs the DATA and NONDATA versions of the macro.
// SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(UnionTerm_c, UnionTerm_c)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(UnionTerm_c, Data)

} // end namespace sept
