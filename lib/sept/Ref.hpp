// 2021.03.31 - Victor Dods

#pragma once

#include "sept/core.hpp"
#include "sept/NPType.hpp"
#include "sept/RefTerm.hpp"

namespace sept {

using RefType_c = NonParametricType_t<NPTerm::REF_TYPE>;
using Ref_c = NonParametricType_t<NPTerm::REF,RefTerm_c>;

extern Ref_c Ref;
extern RefType_c RefType;

inline constexpr NonParametricType_c const &abstract_type_of (RefType_c const &) { return NonParametricType; }
inline constexpr RefType_c const &abstract_type_of (Ref_c const &) { return RefType; }

inline constexpr Ref_c const &concrete_type_of (RefTerm_c const &) { return Ref; }

inline bool inhabits (RefTerm_c const &, Ref_c const &) { return true; }

} // end namespace sept

namespace std {

template <>
struct hash<sept::Ref_c> {
    size_t operator () (sept::Ref_c const &) const {
        return sept::hash(typeid(sept::Ref_c));
    }
};

} // end namespace std
