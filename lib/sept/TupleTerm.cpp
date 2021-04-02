// 2021.03.27 - Victor Dods

#include "sept/TupleTerm.hpp"

#include "sept/NPTerm.hpp"
#include "sept/Tuple.hpp"

namespace sept {

void serialize (TupleTerm_c const &t, std::ostream &out) {
    serialize(SerializedTopLevelCode::PARAMETRIC_TERM, out);
    serialize(TupleType, out);
    serialize(t.size(), out);
    for (auto const &element : t.elements())
        serialize_data(element, out);
}

TupleTerm_c deserialize_value_TupleTerm (Data &&abstract_type, std::istream &in) {
    // TODO: Delegate to BaseArray_t

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
