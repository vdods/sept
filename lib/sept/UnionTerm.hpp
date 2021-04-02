// 2021.03.30 - Victor Dods

#pragma once

#include <lvd/abort.hpp>
#include <lvd/fmt.hpp>
#include "sept/core.hpp"
#include "sept/BaseArray_t.hpp"

namespace sept {

// This class is almost identical to BaseArray_t<UnionTerm_c> itself.
// NOTE: This might better be called a "weak union" because certain decisions are not possible,
// for example determining equality between unions whose elements represent different coverings
// of the same collection of terms.  But probably there's some poset-based condition that, if
// the element types satisfied, would allow the full, expected functioning of the union.
// TODO: Figure out if it's possible to canonicalize the elements (eliminate subtypes or duplicates).
// Eliminating duplicates would be easy enough.  Subtypes requires a lot more doing.
class UnionTerm_c : public BaseArray_t<UnionTerm_c> {
public:

    using ParentClass = BaseArray_t<UnionTerm_c>;

    UnionTerm_c () = default;
    UnionTerm_c (UnionTerm_c const &other) = default;
    // This is needed to ensure that UnionTerm_c& gets passed to this constructor, and not the
    // variadic template below (which would enter an infinite cycle of Data (aka std::any) construction).
    UnionTerm_c (UnionTerm_c &other) : UnionTerm_c(static_cast<UnionTerm_c const &>(other)) { }
    UnionTerm_c (UnionTerm_c &&other) = default;
    explicit UnionTerm_c (DataVector const &elements) : ParentClass(elements) { }
    // This is needed to ensure that UnionTerm_c& gets passed to this constructor, and not the
    // variadic template below (which would enter an infinite cycle of Data (aka std::any) construction).
    explicit UnionTerm_c (DataVector &elements) : UnionTerm_c(static_cast<DataVector const &>(elements)) { }
    explicit UnionTerm_c (DataVector &&elements) : ParentClass(std::move(elements)) { }
    // Ideally we would just use std::enable_if_t to avoid the need for the non-const reference "copy constructors"
    // above, but hey.
    template <typename... Args_>
    explicit UnionTerm_c (Args_&&... args)
        :   ParentClass(make_DataVector(std::forward<Args_>(args)...))
    { }

    UnionTerm_c &operator = (UnionTerm_c const &other) = default;
    UnionTerm_c &operator = (UnionTerm_c &&other) = default;

    // "Construct" an inhabitant by validating it (this is construction of an abstract type)
    template <typename Argument_>
    Argument_ operator() (Argument_ &&argument) const {
        if (!inhabits(argument, *this))
            throw std::runtime_error(LVD_FMT("argument " << argument << " does not inhabit abstract type " << *this));
        return std::forward<Argument_>(argument);
    }
    // TODO: Make a Data-accepting one, and then ensure the above one disallows Argument_==Data

    operator lvd::OstreamDelegate () const {
        return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
            out << "UnionTerm_c" << elements();
        });
    }
};

// // This is used to construct UnionTerm_c more efficiently (std::initializer_list lacks move semantics for some dumb
// // reason), as well as to avoid a potential infinite loop in constructors between UnionTerm_c, Data, and std::any.
// template <typename... Args_>
// UnionTerm_c make_union (Args_&&... args) {
//     return UnionTerm_c(make_DataVector(std::forward<Args_>(args)...));
// }

// // TODO: This should be abstract_type_of, and concrete_type_of should be what returns Union.
// inline UnionTerm_c _union_type_of (UnionTerm_c const &t) {
//     DataVector element_types;
//     element_types.reserve(t.size());
//     for (auto const &element : t.elements()) {
//         element_types.emplace_back(abstract_type_of_data(element));
//     }
//     return UnionTerm_c(std::move(element_types));
// }

// // NOTE: Currently, serialization of parametric types uses the abstract type instead of the concrete type,
// // and in this case, that would cause an infinite recursion.
// inline UnionTerm_c abstract_type_of (UnionTerm_c const &t) { return _union_type_of(t); }

// // A UnionTerm_c can inhabit another UnionTerm_c if there is element-wise inhabitation,
// // e.g. Union(Float, Uint32) inhabits Union(FloatType, Uint32Type).
// inline bool inhabits (UnionTerm_c const &value, UnionTerm_c const &type) {
//     for (auto const &element : value.elements()) {
//         if (!inhabits_data(element, type))
//             return false;
//     }
//     return true;
// }

// A term must only inhabit a single element of the UnionTerm_c to inhabit the UnionTerm_c.
// Templatized version.
template <typename T_, typename = std::enable_if_t<!std::is_same_v<T_,Data>>>
bool inhabits (T_ const &value, UnionTerm_c const &type) {
    // NOTE: If UnionTerm_t is ever created, then this would call inhabits instead of inhabits_data.
    for (auto const &type_element : type.elements()) {
        if (inhabits_data(value, type_element))
            return true;
    }
    return false;
}

// A term must only inhabit a single element of the UnionTerm_c to inhabit the UnionTerm_c.
// Data version.
inline bool inhabits (Data const &value, UnionTerm_c const &type) {
    for (auto const &type_element : type.elements()) {
        if (inhabits_data(value, type_element))
            return true;
    }
    return false;
}

void serialize (UnionTerm_c const &v, std::ostream &out);

// This assumes that the abstract_type portion following the SerializedTopLevelCode::PARAMETRIC_TERM
// has already been read in; that value is passed in as abstract_type.
UnionTerm_c deserialize_value_UnionTerm (Data &&abstract_type, std::istream &in);

// is_member is provided by the one for BaseArray_t; see BaseArray_t.hpp
// compare is provided by the one for BaseArray_t; see BaseArray_t.hpp

} // end namespace sept

namespace std {

// Template specialization(s) to define std::hash<Key_> for the types defined in this header file.
// This is generally done by combining the hashes of the typeid of the argument and its content.

template <>
struct hash<sept::UnionTerm_c> {
    size_t operator () (sept::UnionTerm_c const &t) const {
        return std::hash<sept::UnionTerm_c::ParentClass>()(t);
    }
};

} // end namespace std
