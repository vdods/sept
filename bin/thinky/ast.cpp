// 2021.05.14 - Victor Dods

#include "ast.hpp"

#include "sept/DataVector.hpp"
#include "sept/FormalTypeOf.hpp"
#include "sept/MemRef.hpp"
#include "sept/Tuple.hpp"

sept::TupleTerm_c const SubjVerbObj = sept::Tuple(sept::Term, Verb, sept::Term);
sept::TupleTerm_c const Implication = sept::Tuple(sept::Term, sept::FormalTypeOf(Implies), sept::Term);
sept::TupleTerm_c const Predicate = sept::Tuple(LogicalOp, sept::Term);
sept::TupleTerm_c const Predicate_Not = sept::Tuple(sept::FormalTypeOf(Not), sept::Term);
sept::TupleTerm_c const Predicate_And = sept::Tuple(sept::FormalTypeOf(And), sept::Tuple);
sept::TupleTerm_c const Predicate_Or = sept::Tuple(sept::FormalTypeOf(Or), sept::Tuple);
sept::TupleTerm_c const Predicate_Xor = sept::Tuple(sept::FormalTypeOf(Xor), sept::Tuple);
sept::TupleTerm_c const Predicate_LogicalNaryOp = sept::Tuple(LogicalNaryOp, sept::Tuple);
sept::TupleTerm_c const Predicate_LogicalUnOp = sept::Tuple(LogicalUnOp, sept::Tuple);

sept::RefTerm_c const LogicalPredicate_as_Ref = sept::MemRef(&LogicalPredicate_as_Data);

sept::TupleTerm_c const LogicalAtom = SubjVerbObj; // TODO: Eventually could be a sept::Union(...)
sept::TupleTerm_c const LogicalLiteral_Positive = LogicalAtom;
sept::TupleTerm_c const LogicalLiteral_Negative = sept::Tuple(sept::FormalTypeOf(Not), LogicalAtom);
sept::UnionTerm_c const LogicalLiteral = sept::Union(LogicalLiteral_Positive, LogicalLiteral_Negative);

sept::ArrayETerm_c const LogicalPredicateArray = sept::ArrayE(LogicalPredicate_as_Ref);
sept::TupleTerm_c const Conjunction = sept::Tuple(sept::FormalTypeOf(And), LogicalPredicateArray);
sept::TupleTerm_c const Disjunction = sept::Tuple(sept::FormalTypeOf(Or), LogicalPredicateArray);
sept::TupleTerm_c const Negation = sept::Tuple(sept::FormalTypeOf(Not), LogicalPredicate_as_Ref);

sept::ArrayETerm_c const LogicalLiteralArray = sept::ArrayE(LogicalLiteral);
sept::TupleTerm_c const ConjunctionOfLogicalLiterals = sept::Tuple(sept::FormalTypeOf(And), LogicalLiteralArray); // Should be a subtype of Conjunction
sept::TupleTerm_c const DisjunctionOfLogicalLiterals = sept::Tuple(sept::FormalTypeOf(Or), LogicalLiteralArray); // Should be a subtype of Conjunction

sept::ArrayETerm_c const ConjunctionOfLogicalLiteralsArray = sept::ArrayE(ConjunctionOfLogicalLiterals);
sept::ArrayETerm_c const DisjunctionOfLogicalLiteralsArray = sept::ArrayE(DisjunctionOfLogicalLiterals);
sept::TupleTerm_c const ConjunctiveNormalForm = sept::Tuple(sept::FormalTypeOf(And), DisjunctionOfLogicalLiteralsArray);
sept::TupleTerm_c const DisjunctiveNormalForm = sept::Tuple(sept::FormalTypeOf(Or), ConjunctionOfLogicalLiteralsArray);

// This can't be const.
// NOTE: It seems that this has to be initialized down here, otherwise the order of initialization is wrong.
sept::Data LogicalPredicate_as_Data{
    sept::Union(
        // These two are listed separately instead of as the single LogicalPredicate so that there's no nested UnionTerm_c.
        LogicalLiteral_Positive,
        LogicalLiteral_Negative,
        Conjunction,
        Disjunction,
        Negation
    )
};
sept::UnionTerm_c const &LogicalPredicate = LogicalPredicate_as_Data.cast<sept::UnionTerm_c const &>();

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
