// 2021.05.18 - Victor Dods

#include "trit.hpp"

#include <array>

std::string const &as_string (Trit t) {
    static std::array<std::string,3> const TABLE{
        "Nope",
        "Kwatz",
        "Yep",
    };
    return TABLE.at(Trit_CType(t) - Trit_CType(Trit::__LOWEST__));
}
