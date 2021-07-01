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
sept::UnionTerm_c const Predicate_LogicalOp = sept::Union(Predicate_LogicalNaryOp, Predicate_LogicalUnOp);

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
