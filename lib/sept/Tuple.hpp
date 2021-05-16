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

inline bool constexpr inhabits (TupleTerm_c const &a, Tuple_c const &t) { return true; }

inline bool inhabits (Data const &value, Tuple_c const &t) {
    return value.can_cast<TupleTerm_c>() && inhabits(value.cast<TupleTerm_c const &>(), t);
}

} // end namespace sept

namespace std {

template <>
struct hash<sept::Tuple_c> {
    size_t operator () (sept::Tuple_c const &t) const {
        return lvd::hash(typeid(sept::Tuple_c));
    }
};

} // end namespace std
