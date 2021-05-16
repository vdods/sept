// 2020.08.04 - Victor Dods

#include "sept/ctl/RequestSyncInput.hpp"

namespace sept {
namespace ctl {

RequestSyncInputTerm_c::operator lvd::OstreamDelegate () const {
    return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
        DataPrintCtx ctx;
        print(out, ctx, *this);
    });
}

RequestSyncInputType_c RequestSyncInputType;
RequestSyncInput_c RequestSyncInput;

} // end namespace ctl

// TODO: Factor this so that BaseArray_S_t does it.
void serialize (ctl::RequestSyncInputTerm_c const &v, std::ostream &out) {
    serialize(SerializedTopLevelCode::PARAMETRIC_TERM, out);
    serialize(ctl::RequestSyncInput, out);
    serialize_data(v.requested_type(), out);
}

ctl::RequestSyncInputTerm_c deserialize_value_RequestSyncInputTerm (Data &&abstract_type, std::istream &in) {
    assert(abstract_type.type() == typeid(ctl::RequestSyncInput_c));
    return ctl::RequestSyncInputTerm_c(deserialize_data(in));
}

namespace ctl {

//
// Registrations for Data functions
//

SEPT__REGISTER__PRINT(RequestSyncInputType_c)
SEPT__REGISTER__PRINT(RequestSyncInput_c)
SEPT__REGISTER__PRINT(RequestSyncInputTerm_c)

SEPT__REGISTER__HASH(RequestSyncInputType_c)
SEPT__REGISTER__HASH(RequestSyncInput_c)
SEPT__REGISTER__HASH(RequestSyncInputTerm_c)

SEPT__REGISTER__EQ(RequestSyncInputType_c)
SEPT__REGISTER__EQ(RequestSyncInput_c)
SEPT__REGISTER__EQ(RequestSyncInputTerm_c)

SEPT__REGISTER__ABSTRACT_TYPE_OF(RequestSyncInputType_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(RequestSyncInput_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(RequestSyncInputTerm_c)

SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(RequestSyncInput_c, RequestSyncInputType_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(RequestSyncInputTerm_c, RequestSyncInput_c)

SEPT__REGISTER__SERIALIZE(RequestSyncInputType_c)
SEPT__REGISTER__SERIALIZE(RequestSyncInput_c)
SEPT__REGISTER__SERIALIZE(RequestSyncInputTerm_c)

SEPT__REGISTER__DESERIALIZE(RequestSyncInput_c, return deserialize_value_RequestSyncInputTerm(std::move(abstract_type), in);)

} // end namespace ctl
} // end namespace sept
