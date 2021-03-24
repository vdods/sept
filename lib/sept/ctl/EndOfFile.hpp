// 2020.08.09 - Victor Dods

#pragma once

#include "sept/core.hpp"
#include "sept/NPTerm.hpp"
#include "sept/NPType.hpp"

namespace sept {
namespace ctl {

using EndOfFileType_c = NonParametricTerm_t<NPTerm::END_OF_FILE_TYPE>;
using EndOfFile_c = NonParametricTerm_t<NPTerm::END_OF_FILE>;

extern EndOfFileType_c EndOfFileType;
extern EndOfFile_c EndOfFile;

} // end namespace ctl

inline constexpr True_c is_instance (ctl::EndOfFile_c const &, ctl::EndOfFileType_c const &) { return True; }

inline constexpr NonParametricType_c const &abstract_type_of (ctl::EndOfFileType_c const &) { return NonParametricType; }
inline constexpr ctl::EndOfFileType_c const &abstract_type_of (ctl::EndOfFile_c const &) { return ctl::EndOfFileType; }

} // end namespace sept
