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

ThinkyNPTerm demorganize_logical_nary_op (ThinkyNPTerm t) {
    switch (t) {
        case And: return Or;
        case Or:  return And;
        case Xor: return Xor;
        default: LVD_ABORT("demorganize_logical_nary_op; this is not a boolean bin_op");
    }
}

sept::Data negated_demorganize_data (sept::Data const &predicate) {
    if (false) {
        // SPLUNGE
    } else if (inhabits_data(predicate, Implication)) {
        LVD_ABORT("negated_demorganize_data for Implication not yet implemented");
    } else if (inhabits_data(predicate, Predicate_Not)) {
        // The two Nots cancel out.
        auto inner_predicate = predicate[1];
        return demorganize_data(inner_predicate);
    } else if (inhabits_data(predicate, Predicate_LogicalNaryOp)) {
        auto logical_nary_op = predicate[0].cast<ThinkyNPTerm>();
        auto operand_tuple = predicate[1].move_cast<sept::TupleTerm_c>();
        // Distribute the negation over the And, changing it into Or in the process.
        sept::DataVector negated_elements;
        negated_elements.reserve(operand_tuple.elements().size());
        for (auto const &operand : operand_tuple.elements())
            negated_elements.emplace_back(negated_demorganize_data(operand));

        // demorganize_logical_nary_op switches the op as necessary.
        return sept::Tuple(demorganize_logical_nary_op(logical_nary_op), sept::TupleTerm_c(std::move(negated_elements)));
    } else {
        // Simply apply the negation to the predicate.
        return Predicate(Not, predicate);
    }
}

// TODO: Figure out how to subsume nested Predicate_And predicates (and Predicate_Or and Predicate_Xor)
// into a single predicate (i.e. de-parenthesize).
sept::Data demorganize_data (sept::Data const &predicate) {
    if (false) {
        // SPLUNGE
    } else if (inhabits_data(predicate, Implication)) {
        LVD_ABORT("demorganize_data for Implication not yet implemented");
        // TODO: `p => q` becomes `not(p) or q`
    } else if (inhabits_data(predicate, Predicate_Not)) {
        return negated_demorganize_data(predicate[1]);
    } else if (inhabits_data(predicate, Predicate_LogicalNaryOp)) {
        auto logical_nary_op = predicate[0].cast<ThinkyNPTerm>();
        if (logical_nary_op == Xor) {
//             LVD_ABORT("demorganize_data for Xor is not yet supported");
            // Xor (A1, ..., An) is true iff sum(A1, ..., An) is odd, where true is 1 and false is 0.
            // a xor b => (a or b) and not(a and b)
            // a xor b xor c => (a or b or c) -- 7 cases
            //                  and not(a and b and not(c)) -- subtract 1 case
            //                  and not(a and not(b) and c) -- subtract 1 case
            //                  and not(not(a) and b and c) -- subtract 1 case
        }
        auto operand_tuple = predicate[1].move_cast<sept::TupleTerm_c>();
        // Distribute the demorganize_data over the operands.
        sept::DataVector elements;
        elements.reserve(operand_tuple.elements().size());
        for (auto const &operand : operand_tuple.elements())
            elements.emplace_back(demorganize_data(operand));

        return sept::Tuple(logical_nary_op, sept::TupleTerm_c(std::move(elements)));
    } else {
        // If it's not a logical predicate, then there's no transform to do.
        return predicate;
    }
}
