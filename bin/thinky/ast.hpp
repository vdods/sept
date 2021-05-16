// 2021.05.14 - Victor Dods

#pragma once

#include "common.hpp"

#include "sept/TupleTerm.hpp"

extern sept::TupleTerm_c const SubjVerbObj;
extern sept::TupleTerm_c const Implication;
// TODO: Better names for these
extern sept::TupleTerm_c const Predicate_Not;
extern sept::TupleTerm_c const Predicate_And;
extern sept::TupleTerm_c const Predicate_Or;
extern sept::TupleTerm_c const Predicate_Xor;
extern sept::TupleTerm_c const Predicate_BoolBinOp;

// Changes the given predicate into its canonical form using deMorgan's laws.
sept::Data demorganize_data (sept::Data const &predicate);
