// 2021.05.14 - Victor Dods

#include "ast.hpp"

#include "sept/DataVector.hpp"
#include "sept/FormalTypeOf.hpp"
#include "sept/Tuple.hpp"

sept::TupleTerm_c const SubjVerbObj = sept::Tuple(sept::Term, Verb, sept::Term);
sept::TupleTerm_c const Implication = sept::Tuple(sept::Term, sept::FormalTypeOf(Implies), sept::Term);
sept::TupleTerm_c const Predicate_Not = sept::Tuple(sept::FormalTypeOf(Not), sept::Term);
sept::TupleTerm_c const Predicate_And = sept::Tuple(sept::FormalTypeOf(And), sept::Tuple);
sept::TupleTerm_c const Predicate_Or = sept::Tuple(sept::FormalTypeOf(Or), sept::Tuple);
sept::TupleTerm_c const Predicate_Xor = sept::Tuple(sept::FormalTypeOf(Xor), sept::Tuple);
sept::TupleTerm_c const Predicate_BoolBinOp = sept::Tuple(BoolBinOp, sept::Tuple);

ThinkyNPTerm demorganize_bool_bin_op (ThinkyNPTerm t) {
    switch (t) {
        case And: return Or;
        case Or:  return And;
        case Xor: return Xor;
        default: LVD_ABORT("demorganize_bool_bin_op; this is not a boolean bin_op");
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
    } else if (inhabits_data(predicate, Predicate_BoolBinOp)) {
        auto bool_bin_op = predicate[0].cast<ThinkyNPTerm>();
        auto operand_tuple = predicate[1].move_cast<sept::TupleTerm_c>();
        // Distribute the negation over the And, changing it into Or in the process.
        sept::DataVector negated_elements;
        negated_elements.reserve(operand_tuple.elements().size());
        for (auto const &operand : operand_tuple.elements())
            negated_elements.emplace_back(negated_demorganize_data(operand));

        // demorganize_bool_bin_op switches the op as necessary.
        return sept::Tuple(demorganize_bool_bin_op(bool_bin_op), sept::TupleTerm_c(std::move(negated_elements)));
    } else {
        // Simply apply the negation to the predicate.
        return Predicate_Not(Not, predicate);
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
    } else if (inhabits_data(predicate, Predicate_BoolBinOp)) {
        auto bool_bin_op = predicate[0].cast<ThinkyNPTerm>();
        if (bool_bin_op == Xor) {
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

        return sept::Tuple(bool_bin_op, sept::TupleTerm_c(std::move(elements)));
    } else {
        // If it's not a logical predicate, then there's no transform to do.
        return predicate;
    }
}
