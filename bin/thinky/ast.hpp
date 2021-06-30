// 2021.05.14 - Victor Dods

#pragma once

#include "common.hpp"

#include "sept/ArrayType.hpp"
#include "sept/Tuple.hpp"
#include "sept/Union.hpp"

extern sept::TupleTerm_c const SubjVerbObj;
extern sept::TupleTerm_c const Implication;
extern sept::TupleTerm_c const Predicate;
// TODO: Better names for these
extern sept::TupleTerm_c const Predicate_Not;
extern sept::TupleTerm_c const Predicate_And;
extern sept::TupleTerm_c const Predicate_Or;
extern sept::TupleTerm_c const Predicate_Xor;
extern sept::TupleTerm_c const Predicate_LogicalNaryOp;
extern sept::TupleTerm_c const Predicate_LogicalUnOp;

inline auto const Predicate_LogicalOp = sept::Union(Predicate_LogicalNaryOp, Predicate_LogicalUnOp);

extern sept::Data LogicalPredicate_as_Data;
extern sept::RefTerm_c const LogicalPredicate_as_Ref;
extern sept::UnionTerm_c const &LogicalPredicate;

extern sept::TupleTerm_c const LogicalAtom;
extern sept::TupleTerm_c const LogicalLiteral_Positive;
extern sept::TupleTerm_c const LogicalLiteral_Negative;
extern sept::UnionTerm_c const LogicalLiteral;

extern sept::ArrayETerm_c const LogicalPredicateArray;
extern sept::TupleTerm_c const Conjunction;
extern sept::TupleTerm_c const Disjunction;
extern sept::TupleTerm_c const Negation;

extern sept::ArrayETerm_c const LogicalLiteralArray;
extern sept::TupleTerm_c const ConjunctionOfLogicalLiterals; // Should be a subtype of Conjunction.
extern sept::TupleTerm_c const DisjunctionOfLogicalLiterals; // Should be a subtype of Disjunction.

extern sept::ArrayETerm_c const ConjunctionOfLogicalLiteralsArray;
extern sept::ArrayETerm_c const DisjunctionOfLogicalLiteralsArray;
extern sept::TupleTerm_c const ConjunctiveNormalForm; // Should be a subtype of Conjunction.
extern sept::TupleTerm_c const DisjunctiveNormalForm; // Should be a subtype of Disjunction.

// Changes the given predicate into its canonical form using deMorgan's laws.
sept::Data demorganize_data (sept::Data const &predicate);
