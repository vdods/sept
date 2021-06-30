// 2021.05.18 - Victor Dods

#pragma once

#include <algorithm>
#include <ostream>
#include <string>

// IMO "Trit" sounds dumb, but it is the most reasonable name (analogous to "bit").
// Reference: https://en.wikipedia.org/wiki/Three-valued_logic
using Trit_CType = int8_t;
enum class Trit : Trit_CType {
    NOPE = -1,
    KWATZ = 0,
    YEP = 1,

    __LOWEST__ = NOPE,
    __HIGHEST__ = YEP,
};

inline Trit constexpr Nope = Trit::NOPE;
inline Trit constexpr Kwatz = Trit::KWATZ;
inline Trit constexpr Yep = Trit::YEP;

std::string const &as_string (Trit t);

//
// The following logical operations are named instead of using operator overloads because it would be too
// easy to read an expression like "a && (b || c)" as an ordinary boolean expression.
//

inline Trit not__trit (Trit t) {
    return Trit(-Trit_CType(t));
}

inline Trit and__trit (Trit lhs, Trit rhs) {
    return Trit(std::min(Trit_CType(lhs), Trit_CType(rhs)));
}

inline Trit or__trit (Trit lhs, Trit rhs) {
    return Trit(std::max(Trit_CType(lhs), Trit_CType(rhs)));
}

inline Trit xor__trit (Trit lhs, Trit rhs) {
    return Trit(-(Trit_CType(lhs) * Trit_CType(rhs)));
}

inline std::ostream &operator<< (std::ostream &out, Trit const &t) {
    return out << as_string(t);
}

