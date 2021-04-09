// 2020.07.25 - Victor Dods

#pragma once

#include "sept/core.hpp"
#include "sept/Data.hpp"
#include "sept/hash.hpp"
#include <vector>

namespace sept {

using DataVector = std::vector<Data>;

template <typename... Args_>
DataVector make_DataVector (Args_&&... args) {
    DataVector retval;
    retval.reserve(sizeof...(args));
    // This is a fold expression -- see https://en.cppreference.com/w/cpp/language/fold
    (retval.emplace_back(std::forward<Args_>(args)), ...);
    return retval;
}

std::ostream &operator << (std::ostream &out, DataVector const &v);

// Returns true iff the given value is equal to at least one of the elements in the container.
bool is_member (Data const &value, DataVector const &container);

bool eq (DataVector const &lhs, DataVector const &rhs);

// Runtime complexity O(size); lexicographical ordering (not shortlex order), e.g.
// - "carp"
// - "cars"
// - "cat"
// - "catsup"
int compare (DataVector const &lhs, DataVector const &rhs);

// Returns true iff the DataVector values, considered as unordered sets (and assuming there are no duplicates),
// have the same elements.
bool are_equal_as_sets__assume_no_duplicates (DataVector const &lhs, DataVector const &rhs);

} // end namespace sept

namespace std {

// Template specialization(s) to define std::hash<Key_> for the types defined in this header file.
// This is generally done by combining the hashes of the typeid of the argument and its content.

template <>
struct hash<sept::DataVector> {
    size_t operator () (sept::DataVector const &v) const {
        size_t seed = sept::hash(typeid(sept::DataVector));
        for (auto const &element : v)
            sept::hash_combine(seed, sept::hash(element));
        return seed;
    }
};

} // end namespace std
