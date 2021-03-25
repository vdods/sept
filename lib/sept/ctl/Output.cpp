// 2020.08.04 - Victor Dods

#include "sept/ctl/Output.hpp"

#include "sept/deserialize.hpp"

namespace sept {
namespace ctl {

OutputType_c OutputType;
Output_c Output;

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

} // end namespace sept
