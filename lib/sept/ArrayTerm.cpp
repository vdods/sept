// 2020.08.04 - Victor Dods

#include "sept/ArrayTerm.hpp"

namespace sept {

ArrayTerm_c::operator lvd::OstreamDelegate () const {
    return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
        DataPrintCtx ctx;
        print(out, ctx, *this);
    });
}

void serialize (ArrayTerm_c const &v, std::ostream &out) {
    serialize(SerializedTopLevelCode::PARAMETRIC_TERM, out);
    serialize_data(v.abstract_type(), out);
    // If abstract_type specifies the size, there's no need to encode the runtime size.
    if (v.abstract_type().type() != typeid(ArrayESTerm_c) && v.abstract_type().type() != typeid(ArraySTerm_c))
        serialize(v.size(), out);
    for (auto const &element : v.elements())
        serialize_data(element, out);
}

ArrayTerm_c deserialize_value_ArrayTerm (Data &&abstract_type, std::istream &in) {
    size_t element_count = 0;

    // The runtime size will only be present if it's not present in the abstract_type.
    if (abstract_type.type() == typeid(ArrayESTerm_c)) {
        element_count = abstract_type.cast<ArrayESTerm_c const &>().size();
    } else if (abstract_type.type() == typeid(ArraySTerm_c)) {
        element_count = abstract_type.cast<ArraySTerm_c const &>().size();
    } else {
        auto size = deserialize_data(in);
        // TODO: Make this uint64_t instead, so it's not architecture dependent.
        if (size.type() != typeid(size_t))
            throw std::runtime_error(LVD_FMT("deserialize_value_ArrayTerm expected array size to be size_t, but it was " << size.type()));
        element_count = size.cast<size_t>();
    }

    DataVector elements;
    elements.reserve(element_count);
    for (size_t i = 0; i < element_count; ++i)
        elements.emplace_back(deserialize_data(in));

    return ArrayTerm_c(std::move(elements)).with_constraint(std::move(abstract_type));
}

// Do fancy "from the end" indexing for negative numbers.  An index of -1 will be the last element
// and an index of -a.size() will be the first element.  However, an index of -a.size()-1 will
// throw std::out_of_range.
Data element_of (ArrayTerm_c const &a, int8_t index) { return index >= 0 ? a[index] : a[a.size()+size_t(index)]; }
Data element_of (ArrayTerm_c const &a, int16_t index) { return index >= 0 ? a[index] : a[a.size()+size_t(index)]; }
Data element_of (ArrayTerm_c const &a, int32_t index) { return index >= 0 ? a[index] : a[a.size()+size_t(index)]; }
Data element_of (ArrayTerm_c const &a, int64_t index) { return index >= 0 ? a[index] : a[a.size()+size_t(index)]; }

Data element_of (ArrayTerm_c const &a, uint8_t index) { return a[index]; }
Data element_of (ArrayTerm_c const &a, uint16_t index) { return a[index]; }
Data element_of (ArrayTerm_c const &a, uint32_t index) { return a[index]; }
Data element_of (ArrayTerm_c const &a, uint64_t index) { return a[index]; }

//
// Registrations for Data functions
//

SEPT__REGISTER__PRINT(ArrayTerm_c)

SEPT__REGISTER__HASH(ArrayTerm_c)

SEPT__REGISTER__EQ(ArrayTerm_c)

SEPT__REGISTER__COMPARE(ArrayTerm_c, ArrayTerm_c)

SEPT__REGISTER__SERIALIZE(ArrayTerm_c)

SEPT__REGISTER__ELEMENT_OF__NONDATA(ArrayTerm_c, int8_t)
SEPT__REGISTER__ELEMENT_OF__NONDATA(ArrayTerm_c, int16_t)
SEPT__REGISTER__ELEMENT_OF__NONDATA(ArrayTerm_c, int32_t)
SEPT__REGISTER__ELEMENT_OF__NONDATA(ArrayTerm_c, int64_t)
SEPT__REGISTER__ELEMENT_OF__NONDATA(ArrayTerm_c, uint8_t)
SEPT__REGISTER__ELEMENT_OF__NONDATA(ArrayTerm_c, uint16_t)
SEPT__REGISTER__ELEMENT_OF__NONDATA(ArrayTerm_c, uint32_t)
SEPT__REGISTER__ELEMENT_OF__NONDATA(ArrayTerm_c, uint64_t)

} // end namespace sept
