// 2021.05.08 - Victor Dods

#include "common.hpp"

#include <array>

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
