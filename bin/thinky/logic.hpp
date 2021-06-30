// 2021.05.18 - Victor Dods

#pragma once

#include "common.hpp"
#include "ast.hpp"
#include <lvd/cloned.hpp>
#include "sept/Data.hpp"
#include "sept/FreeVar.hpp"
#include "sept/TupleTerm.hpp"
#include <unordered_set>

// Returns true if term is a positive logical literal, which has the form `X` where X is not itself a
// logical expression.  See https://en.wikipedia.org/wiki/Literal_(mathematical_logic)
bool is_positive_logical_literal (sept::Data const &term);
// Returns true if term is a negative logical literal, which has the form `not(X)` where X is not itself a
// logical expression.  See https://en.wikipedia.org/wiki/Literal_(mathematical_logic)
bool is_negative_logical_literal (sept::Data const &term);
// Returns true if term is a logical literal, which has the form `X` or `not(X)` where X is not itself a
// logical expression.  See https://en.wikipedia.org/wiki/Literal_(mathematical_logic)
inline bool is_logical_literal (sept::Data const &term) {
    return is_positive_logical_literal(term) || is_negative_logical_literal(term);
}
// Returns true if term is a conjunction of logical literals (e.g. A and not(B) and C).
// See https://en.wikipedia.org/wiki/Logical_conjunction
bool is_conjunction_of_logical_literals (sept::Data const &term);
// Returns true if term is a disjunction of logical literals (e.g. A or B or not(C)).
// See https://en.wikipedia.org/wiki/Logical_disjunction
bool is_disjunction_of_logical_literals (sept::Data const &term);

std::unordered_set<sept::FreeVarTerm_c> concludable_free_var_set__data (sept::Data const &term);

// Changes the given predicate into its canonical form using deMorgan's laws.
sept::Data demorganize_data (sept::Data const &predicate);

// TODO: Figure out if there's something in std for unordered_set -- it seems not
template <typename T_>
bool is_subset (std::unordered_set<T_> const &lhs, std::unordered_set<T_> const &rhs) {
    for (auto const &lhs_element : lhs)
        if (rhs.find(lhs_element) == rhs.end())
            return false;
    return true;
}

// This version consumes lhs, operates on it in-place, and then returns it.
template <typename T_>
std::unordered_set<T_> unordered_set_union (std::unordered_set<T_> &&lhs, std::unordered_set<T_> const &rhs) {
    // Add all elements of rhs to lhs.
    for (auto const &rhs_element : rhs)
        lhs.emplace(rhs_element);
    return std::move(lhs);
}

template <typename T_>
std::unordered_set<T_> unordered_set_union (std::unordered_set<T_> const &lhs, std::unordered_set<T_> const &rhs) {
    return unordered_set_union(lvd::cloned(lhs), rhs);
}

// This version consumes lhs, operates on it in-place, and then returns it.
template <typename T_>
std::unordered_set<T_> unordered_set_intersection (std::unordered_set<T_> &&lhs, std::unordered_set<T_> const &rhs) {
    // For erasing while iterating, see
    // https://stackoverflow.com/questions/15662412/how-to-remove-multiple-items-from-unordered-map-while-iterating-over-it
    for (auto lhs_it = lhs.begin(); lhs_it != lhs.end(); ) {
        auto const &lhs_element = *lhs_it;
        // If lhs_element is found in rhs, do nothing.  Otherwise remove it from lhs.
        if (rhs.find(lhs_element) != rhs.end())
            ++lhs_it;
        else
            lhs_it = lhs.erase(lhs_it);
    }
    return std::move(lhs);
}

template <typename T_>
std::unordered_set<T_> unordered_set_intersection (std::unordered_set<T_> const &lhs, std::unordered_set<T_> const &rhs) {
    return unordered_set_intersection(lvd::cloned(lhs), rhs);
}

template <typename T_>
std::unordered_set<T_> unordered_set_difference (std::unordered_set<T_> const &lhs, std::unordered_set<T_> const &rhs) {
    std::unordered_set<T_> retval;
    for (auto const &lhs_element : lhs)
        if (rhs.find(lhs_element) == lhs.end())
            retval.emplace(lhs_element);
    return retval;
}

template <typename T_>
std::unordered_set<T_> unordered_set_difference (std::unordered_set<T_> &&lhs, std::unordered_set<T_> const &rhs) {
    for (auto const &rhs_element : rhs)
        if (lhs.find(rhs_element) != lhs.end())
            lhs.erase(rhs_element);
    return std::move(lhs);
}
