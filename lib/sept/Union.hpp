// 2021.03.30 - Victor Dods

#pragma once

#include "sept/core.hpp"
#include "sept/NPType.hpp"
#include "sept/UnionTerm.hpp"

namespace sept {

using UnionType_c = NonParametricType_t<NPTerm::UNION_TYPE>;
using Union_c = NonParametricType_t<NPTerm::UNION,UnionTerm_c>;

extern Union_c Union;
extern UnionType_c UnionType;

inline constexpr NonParametricType_c const &abstract_type_of (UnionType_c const &) { return NonParametricType; }
inline constexpr UnionType_c const &abstract_type_of (Union_c const &) { return UnionType; }
inline constexpr Union_c const &abstract_type_of (UnionTerm_c const &) { return Union; }

inline bool inhabits (UnionTerm_c const &a, Union_c const &t) { return true; }

inline bool inhabits (Data const &value, Union_c const &t) {
    return value.can_cast<UnionTerm_c>() && inhabits(value.cast<UnionTerm_c const &>(), t);
}

} // end namespace sept

namespace std {

template <>
struct hash<sept::Union_c> {
    size_t operator () (sept::Union_c const &t) const {
        return sept::hash(typeid(sept::Union_c));
    }
};

} // end namespace std
