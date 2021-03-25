// 2020.08.04 - Victor Dods

#pragma once

#include "sept/BaseArray_S_t.hpp"
#include "sept/core.hpp"
#include "sept/Data.hpp"
#include "sept/NPTerm.hpp"
#include "sept/NPType.hpp"

namespace sept {
namespace ctl {

class RequestSyncInputTerm_c;

using RequestSyncInputType_c = NonParametricType_t<NPTerm::REQUEST_SYNC_INPUT_TYPE>;
using RequestSyncInput_c = NonParametricType_t<NPTerm::REQUEST_SYNC_INPUT,RequestSyncInputTerm_c>;

extern RequestSyncInputType_c RequestSyncInputType;
extern RequestSyncInput_c RequestSyncInput;

// TODO: Implement stuff

class RequestSyncInputTerm_c : public BaseArray_S_t<1,RequestSyncInputTerm_c> {
public:

    using ParentClass = BaseArray_S_t<1,RequestSyncInputTerm_c>;

    RequestSyncInputTerm_c () = delete;
    RequestSyncInputTerm_c (RequestSyncInputTerm_c const &) = default;
    RequestSyncInputTerm_c (RequestSyncInputTerm_c &&) = default;
    template <typename... Args_>
    explicit RequestSyncInputTerm_c (Args_&&... args)
    :   ParentClass(std::forward<Args_>(args)...) {
    }

    RequestSyncInputTerm_c &operator = (RequestSyncInputTerm_c const &) = default;
    RequestSyncInputTerm_c &operator = (RequestSyncInputTerm_c &&) = default;

    Data const &requested_type () const { return elements()[0]; }

    operator lvd::OstreamDelegate () const {
        return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
            out << "RequestSyncInputTerm_c(" << requested_type().operator lvd::OstreamDelegate() << ')';
        });
    }
};

// This is used to construct RequestSyncInputTerm_c more efficiently (std::initializer_list lacks move semantics for some dumb
// reason), as well as to avoid a potential infinite loop in constructors between RequestSyncInputTerm_c, Data, and std::any.
template <typename... Args_>
RequestSyncInputTerm_c make_request_sync_input_term (Args_&&... args) {
    return RequestSyncInputTerm_c(std::forward<Args_>(args)...);
}

} // end namespace ctl

void serialize (ctl::RequestSyncInputTerm_c const &v, std::ostream &out);

// This assumes that the abstract_type portion following the SerializedTopLevelCode::PARAMETRIC_TERM
// has already been read in; that value is passed in as abstract_type.
ctl::RequestSyncInputTerm_c deserialize_value_RequestSyncInputTerm (Data &&abstract_type, std::istream &in);

inline constexpr True_c inhabits (ctl::RequestSyncInput_c const &, ctl::RequestSyncInputType_c const &) { return True; }

inline constexpr NonParametricType_c const &abstract_type_of (ctl::RequestSyncInputType_c const &) { return NonParametricType; }
inline constexpr ctl::RequestSyncInputType_c const &abstract_type_of (ctl::RequestSyncInput_c const &) { return ctl::RequestSyncInputType; }
inline constexpr ctl::RequestSyncInput_c const &abstract_type_of (ctl::RequestSyncInputTerm_c const &) { return ctl::RequestSyncInput; }

} // end namespace sept

namespace std {

// Template specialization(s) to define std::hash<Key_> for the types defined in this header file.
// This is generally done by combining the hashes of the typeid of the argument and its content.

template <>
struct hash<sept::ctl::RequestSyncInputTerm_c> {
    size_t operator () (sept::ctl::RequestSyncInputTerm_c const &t) const {
        return std::hash<sept::ctl::RequestSyncInputTerm_c::ParentClass>()(t);
    }
};

} // end namespace std
