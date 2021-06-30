// 2021.05.18 - Victor Dods

#include "logic.hpp"

#include "pattern.hpp"

bool is_positive_logical_literal (sept::Data const &term) {
    return !sept::inhabits_data(term, Predicate_LogicalOp);
}

bool is_negative_logical_literal (sept::Data const &term) {
    return sept::inhabits_data(term, Predicate_Not) && is_positive_logical_literal(term[1]);
}

bool is_conjunction_of_logical_literals (sept::Data const &term) {
    if (!sept::inhabits_data(term, Predicate_And))
        return false;

    auto operand_tuple = term[1].cast<sept::TupleTerm_c>();
    for (auto const &operand : operand_tuple)
        if (!is_logical_literal(operand))
            return false;
    return true;
}

bool is_disjunction_of_logical_literals (sept::Data const &term) {
    if (!sept::inhabits_data(term, Predicate_Or))
        return false;

    auto operand_tuple = term[1].cast<sept::TupleTerm_c>();
    for (auto const &operand : operand_tuple)
        if (!is_logical_literal(operand))
            return false;
    return true;
}

std::unordered_set<sept::FreeVarTerm_c> concludable_free_var_set__data (sept::Data const &term) {
    std::unordered_set<sept::FreeVarTerm_c> retval;
    if (inhabits_data(term, Predicate_And) || inhabits_data(term, Predicate_Xor)) {
        // Take the union; union of no sets is defined to be empty.
        auto operand_tuple = term[1].cast<sept::TupleTerm_c>();
        if (operand_tuple.size() == 0) {
            // retval is already empty, so nothing to do.
        } else {
            retval = concludable_free_var_set__data(operand_tuple[0]);
            for (size_t i = 0; i < operand_tuple.size(); ++i)
                retval = unordered_set_union(std::move(retval), concludable_free_var_set__data(operand_tuple[i]));
        }
    } else if (inhabits_data(term, Predicate_Or)) {
        // Take the intersection; intersection of no sets is defined to be empty here (this is
        // different than the ordinary mathematical convention where an intersection of zero
        // sets is the "universe" (i.e. biggest) set).
        auto operand_tuple = term[1].cast<sept::TupleTerm_c>();
        if (operand_tuple.size() == 0) {
            // retval is already empty, so nothing to do.
        } else {
            retval = concludable_free_var_set__data(operand_tuple[0]);
            for (size_t i = 0; i < operand_tuple.size(); ++i)
                retval = unordered_set_intersection(std::move(retval), concludable_free_var_set__data(operand_tuple[i]));
        }
    } else if (inhabits_data(term, Predicate_Not)) {
        // E.g. `(Not, (And, (X, Y))` is equivalent to `(Or, ((Not, X), (Not, Y)))`, so it should use intersection.
        // E.g. `(Not, (Or, (X, Y))` is equivalent to `(And, ((Not, X), (Not, Y)))`, so it should use union.
        // Similar with Xor.  Thus it should demorganize before computing concludable_free_var_set.
        retval = concludable_free_var_set__data(demorganize_data(term[1]));
    } else {
        // term is logically opaque, no processing besides determining free var set.
        retval = free_var_collection__data(term);
    }
    return retval;
}
