// 2020.08.04 - Victor Dods

#pragma once

#include "sept/core.hpp"
#include "sept/NPTerm.hpp"
#include "sept/NPType.hpp"

namespace sept {
namespace ctl {

using ClearOutputType_c = NonParametricTerm_t<NPTerm::CLEAR_OUTPUT_TYPE>;
using ClearOutput_c = NonParametricTerm_t<NPTerm::CLEAR_OUTPUT>;

extern ClearOutputType_c ClearOutputType;
extern ClearOutput_c ClearOutput;

} // end namespace ctl

inline constexpr True_c is_instance (ctl::ClearOutput_c const &, ctl::ClearOutputType_c const &) { return True; }

inline constexpr NonParametricType_c const &abstract_type_of (ctl::ClearOutputType_c const &) { return NonParametricType; }
inline constexpr ctl::ClearOutputType_c const &abstract_type_of (ctl::ClearOutput_c const &) { return ctl::ClearOutputType; }

} // end namespace sept
