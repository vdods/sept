// 2020.08.04 - Victor Dods

#include "sept/ArrayTerm.hpp"

namespace sept {

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

//
// Registrations for Data functions
//

SEPT_DATA_OPERATOR_EQ_REGISTER_TYPE(ArrayTerm_c)

} // end namespace sept
