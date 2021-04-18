// 2020.08.04 - Victor Dods

#include "sept/ctl/Output.hpp"

namespace sept {
namespace ctl {

OutputTerm_c::operator lvd::OstreamDelegate () const {
    return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
        DataPrintCtx ctx;
        print(out, ctx, *this);
    });
}

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

SEPT__REGISTER__PRINT(OutputType_c)
SEPT__REGISTER__PRINT(Output_c)
SEPT__REGISTER__PRINT(OutputTerm_c)

SEPT__REGISTER__EQ(OutputType_c)
SEPT__REGISTER__EQ(Output_c)
SEPT__REGISTER__EQ(OutputTerm_c)

SEPT__REGISTER__ABSTRACT_TYPE_OF(OutputType_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(Output_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(OutputTerm_c)

SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(Output_c, OutputType_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(OutputTerm_c, Output_c)

SEPT__REGISTER__SERIALIZE(OutputType_c)
SEPT__REGISTER__SERIALIZE(Output_c)
SEPT__REGISTER__SERIALIZE(OutputTerm_c)

SEPT__REGISTER__DESERIALIZE(Output_c, return deserialize_value_OutputTerm(std::move(abstract_type), in);)

} // end namespace ctl
} // end namespace sept
