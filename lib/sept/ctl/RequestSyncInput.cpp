// 2020.08.04 - Victor Dods

#include "sept/ctl/RequestSyncInput.hpp"

#include "sept/deserialize.hpp"

namespace sept {
namespace ctl {

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

SEPT_DATA_PRINTING_REGISTER_TYPE(RequestSyncInputType_c)
SEPT_DATA_PRINTING_REGISTER_TYPE(RequestSyncInput_c)
SEPT_DATA_PRINTING_REGISTER_TYPE(RequestSyncInputTerm_c)

SEPT_EQ_DATA_REGISTER_TYPE(RequestSyncInputType_c)
SEPT_EQ_DATA_REGISTER_TYPE(RequestSyncInput_c)
SEPT_EQ_DATA_REGISTER_TYPE(RequestSyncInputTerm_c)

SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_DEFAULT(RequestSyncInputType_c)
SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_DEFAULT(RequestSyncInput_c)
SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_DEFAULT(RequestSyncInputTerm_c)

SEPT_INHABITS_DATA_REGISTER_TYPE_TRIVIAL(RequestSyncInput_c, RequestSyncInputType_c)
SEPT_INHABITS_DATA_REGISTER_TYPE_TRIVIAL(RequestSyncInputTerm_c, RequestSyncInput_c)

SEPT_SERIALIZE_DATA_REGISTER_TYPE_DEFAULT(RequestSyncInputType_c)
SEPT_SERIALIZE_DATA_REGISTER_TYPE_DEFAULT(RequestSyncInput_c)
SEPT_SERIALIZE_DATA_REGISTER_TYPE_DEFAULT(RequestSyncInputTerm_c)

} // end namespace ctl
} // end namespace sept
