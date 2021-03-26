// 2020.07.25 - Victor Dods

#include "sept/DataVector.hpp"

namespace sept {

std::ostream &operator << (std::ostream &out, DataVector const &v) {
    out << '(';
    for (size_t i = 0; i < v.size(); ++i) {
        out << v[i];
        if (i+1 < v.size())
            out << ", ";
    }
    return out << ')';
}

bool is_member (Data const &value, DataVector const &container) {
    for (Data const &element : container)
        if (value == element)
            return true;
    return false;
}

int compare (DataVector const &lhs, DataVector const &rhs) {
    for (size_t i = 0; i < lhs.size() && i < rhs.size(); ++i) {
        auto c = compare_data(lhs[i], rhs[i]);
        if (c != 0)
            return c;
    }
    // If we got this far, then they match on their common length, so the shorter one is "less".
    return lhs.size() < rhs.size() ? -1 : (lhs.size() == rhs.size() ? 0 : 1);
}

} // end namespace sept
