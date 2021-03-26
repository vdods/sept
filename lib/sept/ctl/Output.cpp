// 2020.08.04 - Victor Dods

#include "sept/ctl/Output.hpp"

namespace sept {
namespace ctl {

OutputType_c OutputType;
Output_c Output;

} // end namespace ctl

// TODO: Factor this so that BaseArray_S_t does it.
void serialize (ctl::OutputTerm_c const &v, std::ostream &out) {
    serialize(SerializedTopLevelCode::PARAMETRIC_TERM, out);
    serialize(ctl::Output, out);
    serialize_data(v.value(), out);
}

ctl::OutputTerm_c deserialize_value_OutputTerm (Data &&abstract_type, std::istream &in) {
    assert(abstract_type.type() == typeid(ctl::Output_c));
    return ctl::OutputTerm_c(deserialize_data(in));
}

namespace ctl {

//
// Registrations for Data functions
//

SEPT_DATA_PRINTING_REGISTER_TYPE(OutputType_c)
SEPT_DATA_PRINTING_REGISTER_TYPE(Output_c)
SEPT_DATA_PRINTING_REGISTER_TYPE(OutputTerm_c)

SEPT_EQ_DATA_REGISTER_TYPE(OutputType_c)
SEPT_EQ_DATA_REGISTER_TYPE(Output_c)
SEPT_EQ_DATA_REGISTER_TYPE(OutputTerm_c)

SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_DEFAULT(OutputType_c)
SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_DEFAULT(Output_c)
SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_DEFAULT(OutputTerm_c)

SEPT_INHABITS_DATA_REGISTER_TYPE_TRIVIAL(Output_c, OutputType_c)
SEPT_INHABITS_DATA_REGISTER_TYPE_TRIVIAL(OutputTerm_c, Output_c)

SEPT_SERIALIZE_DATA_REGISTER_TYPE_DEFAULT(OutputType_c)
SEPT_SERIALIZE_DATA_REGISTER_TYPE_DEFAULT(Output_c)
SEPT_SERIALIZE_DATA_REGISTER_TYPE_DEFAULT(OutputTerm_c)

SEPT_DESERIALIZE_DATA_REGISTER_TYPE(Output_c, return deserialize_value_OutputTerm(std::move(abstract_type), in);)

} // end namespace ctl
} // end namespace sept
