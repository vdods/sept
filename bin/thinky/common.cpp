// 2021.05.08 - Victor Dods

#include "common.hpp"

#include <array>
#include <lvd/literal.hpp>
#include <string>

std::string const &as_string (ThinkyNPTerm t) {
    static std::array<std::string,THINKY_NP_TERM_COUNT> const TABLE{
        //
        // Operators
        //

        // BinOp
        "Implies",
        "IsA",
        "HasEntity",
        "HasA",
        "HasEvery",
        "HasProperty",
        "LikesEntity",
        "LikesA",
        "LikesEvery",
        "HatesEntity",
        "HatesA",
        "HatesEvery",
        "Says",
        "Takes",
        "Drops",
        "Gives",

        // UnOp
        "Not",
        "And",
        "Or",
        "Xor",

        "Should",
        "Could",
        "Would",

        "Cool",
        "Lame",
        "Big",
        "Small",
        "Stupid",
        "Smart",
        "Fast",
        "Slow",
        "Loud",
        "Quiet",

        "Red",
        "Orange",
        "Yellow",
        "Green",
        "Blue",
        "Indigo",
        "Violet",

        // Misc
        "If",
        "Then",
        "Otherwise",

        //
        // Terminals
        //

        // Objects
        "Book",
        "Box",
        "Cup",
        "Hat",

        // People
        "Alice",
        "Bob",
        "Charlie",
        "Dave",

        // Animals
        "Cat",
        "Dog",
    };
    return TABLE.at(size_t(t));
}

// This constructs the effective inverse to the function `std::string as_string(ThinkyNPTerm)`
std::unordered_map<std::string,ThinkyNPTerm> make_thinky_np_term_map () {
    std::unordered_map<std::string,ThinkyNPTerm> retval;
    for (ThinkyNPTerm_CType i = ThinkyNPTerm_CType(ThinkyNPTerm::__LOWEST__); i <= ThinkyNPTerm_CType(ThinkyNPTerm::__HIGHEST__); ++i) {
        auto t = ThinkyNPTerm(i);
        retval[as_string(t)] = t;
    }
    return retval;
}

ThinkyNPTerm thinky_np_term_from_string (std::string const &s) noexcept(false) {
    static auto const MAP = make_thinky_np_term_map();
    try {
        return MAP.at(s);
    } catch (std::out_of_range const &e) {
        throw std::runtime_error(LVD_FMT("Unrecognized ThinkyNPTerm: " << lvd::literal_of(s)));
    }
}
