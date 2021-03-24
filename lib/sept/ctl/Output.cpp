// 2020.08.04 - Victor Dods

#include "sept/ctl/Output.hpp"

#include "sept/deserialize.hpp"

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

} // end namespace sept
