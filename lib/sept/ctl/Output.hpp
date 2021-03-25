// 2020.08.04 - Victor Dods

#pragma once

#include "sept/BaseArray_S_t.hpp"
#include "sept/core.hpp"
#include "sept/NPTerm.hpp"
#include "sept/NPType.hpp"

namespace sept {
namespace ctl {

class OutputTerm_c;

using OutputType_c = NonParametricType_t<NPTerm::OUTPUT_TYPE>;
using Output_c = NonParametricType_t<NPTerm::OUTPUT,OutputTerm_c>;

extern OutputType_c OutputType;
extern Output_c Output;

// TODO: Implement stuff

class OutputTerm_c : public BaseArray_S_t<1,OutputTerm_c> {
public:

    using ParentClass = BaseArray_S_t<1,OutputTerm_c>;

    OutputTerm_c () = delete;
    OutputTerm_c (OutputTerm_c const &) = default;
    OutputTerm_c (OutputTerm_c &&) = default;
    template <typename... Args_>
    explicit OutputTerm_c (Args_&&... args)
    :   ParentClass(std::forward<Args_>(args)...) {
    }

    OutputTerm_c &operator = (OutputTerm_c const &) = default;
    OutputTerm_c &operator = (OutputTerm_c &&) = default;

    Data const &value () const { return elements()[0]; }

    operator lvd::OstreamDelegate () const {
        return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
            out << "OutputTerm_c(" << value() << ')';
        });
    }
};

// This is used to construct OutputTerm_c more efficiently (std::initializer_list lacks move semantics for some dumb
// reason), as well as to avoid a potential infinite loop in constructors between OutputTerm_c, Data, and std::any.
template <typename... Args_>
OutputTerm_c make_output_term (Args_&&... args) {
    return OutputTerm_c(std::forward<Args_>(args)...);
}

} // end namespace ctl

void serialize (ctl::OutputTerm_c const &v, std::ostream &out);

// This assumes that the abstract_type portion following the SerializedTopLevelCode::PARAMETRIC_TERM
// has already been read in; that value is passed in as abstract_type.
ctl::OutputTerm_c deserialize_value_OutputTerm (Data &&abstract_type, std::istream &in);

inline constexpr True_c inhabits (ctl::Output_c const &, ctl::OutputType_c const &) { return True; }

inline constexpr NonParametricType_c const &abstract_type_of (ctl::OutputType_c const &) { return NonParametricType; }
inline constexpr ctl::OutputType_c const &abstract_type_of (ctl::Output_c const &) { return ctl::OutputType; }
inline constexpr ctl::Output_c const &abstract_type_of (ctl::OutputTerm_c const &) { return ctl::Output; }

} // end namespace sept

namespace std {

// Template specialization(s) to define std::hash<Key_> for the types defined in this header file.
// This is generally done by combining the hashes of the typeid of the argument and its content.

template <>
struct hash<sept::ctl::OutputTerm_c> {
    size_t operator () (sept::ctl::OutputTerm_c const &t) const {
        return std::hash<sept::ctl::OutputTerm_c::ParentClass>()(t);
    }
};

} // end namespace std
