// 2020.07.25 - Victor Dods

#include "sept/DataVector.hpp"

#include <lvd/comma.hpp>

namespace sept {

void print (std::ostream &out, DataPrintCtx &ctx, DataVector const &value) {
    auto cspace_delim = lvd::make_comma_space_delimiter();
    out << '(';
    for (auto const &element : value) {
        out << cspace_delim;
        print_data(out, ctx, element);
    }
    out << ')';
}

std::ostream &operator << (std::ostream &out, DataVector const &v) {
    DataPrintCtx ctx;
    print(out, ctx, v);
    return out;
}

bool is_member (Data const &value, DataVector const &container) {
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << lvd::IndentGuard() << '\n'
//                << LVD_REFLECT(value) << '\n'
//                << LVD_REFLECT(container) << '\n';
    for (Data const &element : container)
        if (value == element)
            return true;
    return false;
}

bool eq (DataVector const &lhs, DataVector const &rhs) {
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << lvd::IndentGuard() << '\n'
//                << LVD_REFLECT(&lhs) << '\n'
//                << LVD_REFLECT(&rhs) << '\n'
//                << LVD_REFLECT(lhs) << '\n'
//                << LVD_REFLECT(rhs) << '\n';
    if (&lhs == &rhs)
        return true;
    if (lhs.size() != rhs.size())
        return false;
    for (size_t i = 0; i < lhs.size(); ++i)
        if (lhs[i] != rhs[i])
            return false;
    return true;
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

bool are_equal_as_sets__assume_no_duplicates (DataVector const &lhs, DataVector const &rhs) {
    if (lhs.size() != rhs.size())
        return false;
    // Since the sizes are known equal at this point, we just have to check that all lhs elements
    // are present in rhs.
    for (auto const &lhs_element : lhs)
        if (!is_member(lhs_element, rhs))
            return false;
    return true;
}

} // end namespace sept
