// 2021.03.27 - Victor Dods

#pragma once

#include "sept/core.hpp"
#include "sept/NPType.hpp"
#include "sept/TupleTerm.hpp"

namespace sept {

using TupleType_c = NonParametricType_t<NPTerm::TUPLE_TYPE>;
using Tuple_c = NonParametricType_t<NPTerm::TUPLE,TupleTerm_c>;

extern Tuple_c Tuple;
extern TupleType_c TupleType;

inline constexpr NonParametricType_c const &abstract_type_of (TupleType_c const &) { return NonParametricType; }
inline constexpr TupleType_c const &abstract_type_of (Tuple_c const &) { return TupleType; }
inline constexpr Tuple_c const &abstract_type_of (TupleTerm_c const &) { return Tuple; }

// TODO: This should be abstract_type_of, and concrete_type_of should be what returns Tuple.
inline TupleTerm_c tuple_type_of (TupleTerm_c const &t) {
    DataVector element_types;
    element_types.reserve(t.size());
    for (auto const &element : t.elements()) {
        element_types.emplace_back(abstract_type_of_data(element));
    }
    return TupleTerm_c(std::move(element_types));
}

inline bool inhabits (TupleTerm_c const &a, Tuple_c const &t) { return true; }

// A TupleTerm_c can inhabit another TupleTerm_c if there is element-wise inhabitation,
// e.g. (Float(1.23), Uint32(400)) inhabits (Float, Uint32).
inline bool inhabits (TupleTerm_c const &value, TupleTerm_c const &type) {
    if (value.size() != type.size())
        return false;
    for (size_t i = 0; i < value.size(); ++i)
        if (!inhabits_data(value[i], type[i]))
            return false;
    return true;
}

inline bool inhabits (Data const &value, Tuple_c const &t) {
    return value.can_cast<TupleTerm_c>() && inhabits(value.cast<TupleTerm_c const &>(), t);
}

} // end namespace sept

namespace std {

template <>
struct hash<sept::Tuple_c> {
    size_t operator () (sept::Tuple_c const &t) const {
        return sept::hash(typeid(sept::Tuple_c));
    }
};

} // end namespace std
