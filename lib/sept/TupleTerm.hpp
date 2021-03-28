// 2021.03.27 - Victor Dods

#pragma once

#include "sept/core.hpp"
#include "sept/BaseArray_t.hpp"

namespace sept {

// This class is almost identical to BaseArray_t<TupleTerm_c> itself.
class TupleTerm_c : public BaseArray_t<TupleTerm_c> {
public:

    using ParentClass = BaseArray_t<TupleTerm_c>;

    TupleTerm_c () = default;
    TupleTerm_c (TupleTerm_c const &other) = default;
    TupleTerm_c (TupleTerm_c &&other) = default;
    explicit TupleTerm_c (DataVector const &elements) : ParentClass(elements) { }
    explicit TupleTerm_c (DataVector &&elements) : ParentClass(std::move(elements)) { }
    template <typename... Args_>
    explicit TupleTerm_c (Args_&&... args)
        :   ParentClass(make_DataVector(std::forward<Args_>(args)...))
    { }

    TupleTerm_c &operator = (TupleTerm_c const &other) = default;
    TupleTerm_c &operator = (TupleTerm_c &&other) = default;

    operator lvd::OstreamDelegate () const {
        return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
            out << "TupleTerm_c" << elements();
        });
    }
};

// This is used to construct TupleTerm_c more efficiently (std::initializer_list lacks move semantics for some dumb
// reason), as well as to avoid a potential infinite loop in constructors between TupleTerm_c, Data, and std::any.
template <typename... Args_>
TupleTerm_c make_tuple (Args_&&... args) {
    return TupleTerm_c(std::forward<Args_>(args)...);
}

void serialize (TupleTerm_c const &v, std::ostream &out);

// This assumes that the abstract_type portion following the SerializedTopLevelCode::PARAMETRIC_TERM
// has already been read in; that value is passed in as abstract_type.
TupleTerm_c deserialize_value_TupleTerm (Data &&abstract_type, std::istream &in);

// is_member is provided by the one for BaseArray_t; see BaseArray_t.hpp
// compare is provided by the one for BaseArray_t; see BaseArray_t.hpp

} // end namespace sept

namespace std {

// Template specialization(s) to define std::hash<Key_> for the types defined in this header file.
// This is generally done by combining the hashes of the typeid of the argument and its content.

template <>
struct hash<sept::TupleTerm_c> {
    size_t operator () (sept::TupleTerm_c const &t) const {
        return std::hash<sept::TupleTerm_c::ParentClass>()(t);
    }
};

} // end namespace std
