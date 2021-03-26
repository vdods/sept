// 2020.07.25 - Victor Dods

#pragma once

#include <array>
#include <experimental/array> // for std::make_array
#include "sept/core.hpp"
#include "sept/Data.hpp"
#include "sept/hash.hpp"

namespace sept {

template <size_t ELEMENT_COUNT_>
using DataArray_t = std::array<Data,ELEMENT_COUNT_>;

template <size_t ELEMENT_COUNT_, typename... Args_>
DataArray_t<ELEMENT_COUNT_> make_DataArray_t (Args_&&... args) {
    static_assert(ELEMENT_COUNT_ == sizeof...(Args_));
    return std::experimental::make_array<Data>(std::forward<Args_>(args)...);
}

template <size_t ELEMENT_COUNT_>
std::ostream &operator << (std::ostream &out, DataArray_t<ELEMENT_COUNT_> const &a) {
    out << '(';
    for (size_t i = 0; i < ELEMENT_COUNT_; ++i) {
        out << a[i];
        if (i+1 < ELEMENT_COUNT_)
            out << ", ";
    }
    return out << ')';
}

// Returns true iff the given value is equal to at least one of the elements in the container.
template <size_t ELEMENT_COUNT_>
bool is_member (Data const &value, DataArray_t<ELEMENT_COUNT_> const &container) {
    for (Data const &element : container)
        if (value == element)
            return true;
    return false;
}

// Runtime complexity O(size); lexicographical ordering (not shortlex order).
template <size_t ELEMENT_COUNT_>
int compare (DataArray_t<ELEMENT_COUNT_> const &lhs, DataArray_t<ELEMENT_COUNT_> const &rhs) {
    for (size_t i = 0; i < ELEMENT_COUNT_; ++i) {
        auto c = compare_data(lhs[i], rhs[i]);
        if (c != 0)
            return c;
    }
    // If we got this far, then they match.
    return 0;
}

} // end namespace sept

namespace std {

// Template specialization(s) to define std::hash<Key_> for the types defined in this header file.
// This is generally done by combining the hashes of the typeid of the argument and its content.

template <size_t ELEMENT_COUNT_>
struct hash<sept::DataArray_t<ELEMENT_COUNT_>> {
    size_t operator () (sept::DataArray_t<ELEMENT_COUNT_> const &a) const {
        size_t seed = sept::hash(typeid(sept::DataArray_t<ELEMENT_COUNT_>));
        for (auto const &element : a)
            sept::hash_combine(seed, sept::hash(element));
        return seed;
    }
};

} // end namespace std
