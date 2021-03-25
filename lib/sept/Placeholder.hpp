// 2020.07.25 - Victor Dods

#pragma once

#include "sept/core.hpp"
#include "sept/NPTerm.hpp"
#include "sept/NPType.hpp"
#include "sept/BaseArrayT_t.hpp"
#include "sept/BaseArray_S_t.hpp"

namespace sept {

// PlaceholderType has one inhabitant, which is Placeholder.
// Placeholder has instances Placeholder(0), Placeholder(1), etc (corresponds to instances of PlaceholderTerm_c).
// Placeholder(n) can be instantiated with arbitrary parameters (corresponds to instances of PlaceholderTermTerm_c).

class PlaceholderTerm_c : public BaseArray_S_t<1,PlaceholderTerm_c> {
public:

    explicit PlaceholderTerm_c (Data const &index) : BaseArray_S_t<1,PlaceholderTerm_c>(index) { }

    // Constructs a Term of type Placeholder(n) for some index n, e.g. Placeholder(3)(Float64, Float32, Bool, 100).
    template <typename... Args_>
    BaseArrayT_t<> operator () (Args_&&... args) const {
        return BaseArrayT_t<>(*this, make_DataVector(std::forward<Args_>(args)...));
    }

    Data const &index () const & { return elements()[0]; }
//     Data &index () & { return elements()[0]; }

    operator lvd::OstreamDelegate () const {
        return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
            out << "Placeholder(" << index() << ')';
        });
    }
};

using PlaceholderType_c = NonParametricType_t<NPTerm::PLACEHOLDER_TYPE>;
using Placeholder_c = NonParametricType_t<NPTerm::PLACEHOLDER, PlaceholderTerm_c>;

extern PlaceholderType_c PlaceholderType;
extern Placeholder_c Placeholder;

inline constexpr NonParametricType_c const &abstract_type_of (PlaceholderType_c const &) { return NonParametricType; }
inline constexpr PlaceholderType_c const &abstract_type_of (Placeholder_c const &) { return PlaceholderType; }
inline constexpr Placeholder_c const &abstract_type_of (PlaceholderTerm_c const &) { return Placeholder; }

inline bool inhabits (PlaceholderTerm_c const &a, Placeholder_c const &t) { return true; }
inline bool inhabits (Data_t<PlaceholderTerm_c> const &m, Placeholder_c const &t) { return true; }
inline bool inhabits (Data const &value, Placeholder_c const &t) { return value.type() == typeid(PlaceholderTerm_c); }

} // end namespace sept
