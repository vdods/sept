// 2021.03.27 - Victor Dods

#include "sept/Tuple.hpp" // TODO: Rename to TupleTerm.hpp?

namespace sept {

void serialize (TupleTerm_c const &t, std::ostream &out) {
    serialize(SerializedTopLevelCode::PARAMETRIC_TERM, out);
    serialize(TupleType, out);
    serialize(t.size(), out);
    for (auto const &element : t.elements())
        serialize_data(element, out);
}

TupleTerm_c deserialize_value_TupleTerm (Data &&abstract_type, std::istream &in) {
    size_t element_count = 0;

    auto size = deserialize_data(in);
    // TODO: Make this uint64_t instead, so it's not architecture dependent.
    if (size.type() != typeid(size_t))
        throw std::runtime_error(LVD_FMT("deserialize_value_TupleTerm expected array size to be size_t, but it was " << size.type()));
    element_count = size.cast<size_t>();

    DataVector elements;
    elements.reserve(element_count);
    for (size_t i = 0; i < element_count; ++i)
        elements.emplace_back(deserialize_data(in));

    return TupleTerm_c(std::move(elements));
}

// Do fancy "from the end" indexing for negative numbers.  An index of -1 will be the last element
// and an index of -t.size() will be the first element.  However, an index of -t.size()-1 will
// throw std::out_of_range.
Data element_of (TupleTerm_c const &t, int8_t index) { return index >= 0 ? t[index] : t[t.size()+size_t(index)]; }
Data element_of (TupleTerm_c const &t, int16_t index) { return index >= 0 ? t[index] : t[t.size()+size_t(index)]; }
Data element_of (TupleTerm_c const &t, int32_t index) { return index >= 0 ? t[index] : t[t.size()+size_t(index)]; }
Data element_of (TupleTerm_c const &t, int64_t index) { return index >= 0 ? t[index] : t[t.size()+size_t(index)]; }

Data element_of (TupleTerm_c const &t, uint8_t index) { return t[index]; }
Data element_of (TupleTerm_c const &t, uint16_t index) { return t[index]; }
Data element_of (TupleTerm_c const &t, uint32_t index) { return t[index]; }
Data element_of (TupleTerm_c const &t, uint64_t index) { return t[index]; }

//
// Registrations for Data functions
//

SEPT__REGISTER__PRINT(TupleTerm_c)

SEPT__REGISTER__EQ(TupleTerm_c)

SEPT__REGISTER__COMPARE(TupleTerm_c, TupleTerm_c)

SEPT__REGISTER__SERIALIZE(TupleTerm_c)

SEPT__REGISTER__ELEMENT_OF__NONDATA(TupleTerm_c, int8_t)
SEPT__REGISTER__ELEMENT_OF__NONDATA(TupleTerm_c, int16_t)
SEPT__REGISTER__ELEMENT_OF__NONDATA(TupleTerm_c, int32_t)
SEPT__REGISTER__ELEMENT_OF__NONDATA(TupleTerm_c, int64_t)
SEPT__REGISTER__ELEMENT_OF__NONDATA(TupleTerm_c, uint8_t)
SEPT__REGISTER__ELEMENT_OF__NONDATA(TupleTerm_c, uint16_t)
SEPT__REGISTER__ELEMENT_OF__NONDATA(TupleTerm_c, uint32_t)
SEPT__REGISTER__ELEMENT_OF__NONDATA(TupleTerm_c, uint64_t)

SEPT__REGISTER__CONSTRUCT_INHABITANT_OF(TupleTerm_c, TupleTerm_c)

} // end namespace sept
