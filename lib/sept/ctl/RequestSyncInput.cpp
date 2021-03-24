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

} // end namespace sept
