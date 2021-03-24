// 2020.03.13 - Victor Dods

#pragma once

#include <lvd/fmt.hpp>
#include <ostream>

namespace std {

class type_info;

// This could be considered a no-no, adding something to std, but hey.  It seems like if you add operator overloads
// for types into a namespace different from that the type is defined in, you run into trouble trying to use
// them in other namespaces due to the myriad ADL and other overload resolution rules.
ostream &operator << (ostream &out, type_info const &ti);

} // end namespace std

namespace sept {

// This is a sentinel value for use in the template parameters of NonParametricTerm_t to indicate that
// the "true derived class" is the same as the NonParametricTerm_t being defined.
struct DerivedNone {
    DerivedNone () = delete;
};

} // end namespace sept
