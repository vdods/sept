// 2021.05.14 - Victor Dods

#include "ast.hpp"
#include "belief.hpp"
#include "common.hpp"
#include "sept/ArrayTerm.hpp"
#include "sept/ArrayType.hpp"
#include "sept/Data.hpp"
#include "sept/FreeVar.hpp"
#include "sept/NPTerm.hpp"
#include "sept/OrderedMapType.hpp"
#include "sept/SymbolTable.hpp"
#include "sept/Tuple.hpp"
#include "sept/Union.hpp"
#include "sept/UnionTerm.hpp"

//
// sept data model registrations
//

namespace sept {
SEPT__REGISTER__PRINT(Adjective_c)
SEPT__REGISTER__PRINT(Animal_c)
SEPT__REGISTER__PRINT(BinOp_c)
SEPT__REGISTER__PRINT(BoolBinOp_c)
SEPT__REGISTER__PRINT(Color_c)
SEPT__REGISTER__PRINT(Entity_c)
// SEPT__REGISTER__PRINT__GIVE_ID(sem::Expr_Term_c, __sem__Expr_Term_c__)
SEPT__REGISTER__PRINT(Object_c)
SEPT__REGISTER__PRINT(Person_c)
SEPT__REGISTER__PRINT(ThinkyNPTerm)
SEPT__REGISTER__PRINT(UnOp_c)
SEPT__REGISTER__PRINT(Verb_c)
SEPT__REGISTER__PRINT__GIVE_ID(char const *, __char_const_ptr__)

SEPT__REGISTER__HASH(Adjective_c)
SEPT__REGISTER__HASH(Animal_c)
SEPT__REGISTER__HASH(BinOp_c)
SEPT__REGISTER__HASH(BoolBinOp_c)
SEPT__REGISTER__HASH(Color_c)
SEPT__REGISTER__HASH(Entity_c)
// SEPT__REGISTER__HASH__GIVE_ID(sem::Expr_Term_c, __sem__Expr_Term_c__)
SEPT__REGISTER__HASH(Object_c)
SEPT__REGISTER__HASH(Person_c)
SEPT__REGISTER__HASH(ThinkyNPTerm)
SEPT__REGISTER__HASH(UnOp_c)
SEPT__REGISTER__HASH(Verb_c)

SEPT__REGISTER__EQ(Adjective_c)
SEPT__REGISTER__EQ(Animal_c)
SEPT__REGISTER__EQ(BinOp_c)
SEPT__REGISTER__EQ(BoolBinOp_c)
SEPT__REGISTER__EQ(Color_c)
SEPT__REGISTER__EQ(Entity_c)
SEPT__REGISTER__EQ(Object_c)
SEPT__REGISTER__EQ(Person_c)
SEPT__REGISTER__EQ(ThinkyNPTerm)
SEPT__REGISTER__EQ(UnOp_c)
SEPT__REGISTER__EQ(Verb_c)

SEPT__REGISTER__ABSTRACT_TYPE_OF(Adjective_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(Animal_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(BinOp_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(BoolBinOp_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(Color_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(Entity_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(Object_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(Person_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(UnOp_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(Verb_c)

SEPT__REGISTER__INHABITS__NONDATA(ThinkyNPTerm, Adjective_c)
SEPT__REGISTER__INHABITS__NONDATA(ThinkyNPTerm, Animal_c)
SEPT__REGISTER__INHABITS__NONDATA(ThinkyNPTerm, BinOp_c)
SEPT__REGISTER__INHABITS__NONDATA(ThinkyNPTerm, BoolBinOp_c)
SEPT__REGISTER__INHABITS__NONDATA(ThinkyNPTerm, Color_c)
SEPT__REGISTER__INHABITS__NONDATA(ThinkyNPTerm, Entity_c)
SEPT__REGISTER__INHABITS__NONDATA(ThinkyNPTerm, Object_c)
SEPT__REGISTER__INHABITS__NONDATA(ThinkyNPTerm, Person_c)
SEPT__REGISTER__INHABITS__NONDATA(ThinkyNPTerm, UnOp_c)
SEPT__REGISTER__INHABITS__NONDATA(ThinkyNPTerm, Verb_c)
SEPT__REGISTER__INHABITS__GIVE_ID__NONDATA(ThinkyNPTerm, FormalTypeOf_Term_c, __ThinkyNPTerm___sem__FormalTypeOf_Term_c__)
// TODO there are probably some missing

SEPT__REGISTER__COMPARE__SINGLETON(Adjective_c)
SEPT__REGISTER__COMPARE__SINGLETON(Animal_c)
SEPT__REGISTER__COMPARE__SINGLETON(BinOp_c)
SEPT__REGISTER__COMPARE__SINGLETON(BoolBinOp_c)
SEPT__REGISTER__COMPARE__SINGLETON(Color_c)
SEPT__REGISTER__COMPARE__SINGLETON(Entity_c)
SEPT__REGISTER__COMPARE__SINGLETON(Object_c)
SEPT__REGISTER__COMPARE__SINGLETON(Person_c)
SEPT__REGISTER__COMPARE__SINGLETON(UnOp_c)
SEPT__REGISTER__COMPARE__SINGLETON(Verb_c)
SEPT__REGISTER__COMPARE(ThinkyNPTerm, ThinkyNPTerm)

SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(Adjective_c, ThinkyNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(Animal_c, ThinkyNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(BinOp_c, ThinkyNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(BoolBinOp_c, ThinkyNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(Color_c, ThinkyNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(Entity_c, ThinkyNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(Object_c, ThinkyNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(Person_c, ThinkyNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(UnOp_c, ThinkyNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(Verb_c, ThinkyNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(FormalTypeOf_Term_c, ThinkyNPTerm)

// TEMP HACK
SEPT__REGISTER__INHABITS__NONDATA(ThinkyNPTerm, UnionTerm_c)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(UnionTerm_c, ThinkyNPTerm)
} // end namespace sept

//
// BEGIN TESTING
//

class Match {
public:

    Match (sept::Data const &matched_content, sept::SymbolTable &&symbol_assignment)
        :   m_matched_content(matched_content)
        ,   m_symbol_assignment(std::move(symbol_assignment))
    { }

    sept::Data const &matched_content () const { return m_matched_content; }
    sept::SymbolTable const &symbol_assignment () const { return m_symbol_assignment; }

private:

    sept::Data m_matched_content;
    sept::SymbolTable m_symbol_assignment;
};

std::ostream &operator<< (std::ostream &out, Match const &match) {
    lvd::Log log(out);
    log << "Match(\n";
    {
        auto ig = lvd::IndentGuard(log);
        log << "matched_content = " << match.matched_content() << '\n'
            << "symbol_assignment = " << match.symbol_assignment() << '\n';
    }
    log << ')';
    return out;
}

// This assumes that the gross pattern matches, but must still verify that duplicated symbols must match.
// It will return false only if the match failed because of mismatching symbols.
bool attempt_pattern_match__data (sept::Data const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log &match_failure_log);

// This assumes that the gross pattern matches, but must still verify that duplicated symbols must match.
// It will return false only if the match failed because of mismatching symbols.
bool attempt_pattern_match__FreeVarTerm_c (sept::FreeVarTerm_c const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log &match_failure_log) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    assert(sept::inhabits_data(term, pattern)); // Should always be true since FreeVar matches anything.
    if (symbol_assignment.symbol_is_defined(pattern.free_var_id__as_string())) {
        sept::Data const &sa = symbol_assignment.resolve_symbol_const(pattern.free_var_id__as_string());
        if (sa != term) {
            // This represents a repeated pattern-match symbol (e.g. Tuple(FreeVar("X"), FreeVar("X"))) where
            // the second match of FreeVar("X") doesn't have the same value as the first.
            match_failure_log << "pattern " << pattern << " had [at least] two distinct matches -- the first was " << sa << " and the second was " << term << '\n';
            return false;
        }
    } else {
        symbol_assignment.define_symbol(pattern.free_var_id__as_string(), term);
    }
    return true;
}

// This assumes that the gross pattern matches, but must still verify that duplicated symbols must match.
// It will return false only if the match failed because of mismatching symbols.
bool attempt_pattern_match__TupleTerm_c (sept::TupleTerm_c const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log &match_failure_log) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    assert(sept::inhabits_data(pattern, sept::Tuple));
    assert(sept::inhabits_data(term, pattern));
    auto const &tuple_term = term.cast<sept::TupleTerm_c const &>();
    assert(pattern.size() == tuple_term.size());
    // Match on the components of each -- don't early out via logic short-circuit..
    bool retval = true;
    for (size_t i = 0; i < pattern.size(); ++i)
        retval = attempt_pattern_match__data(pattern[i], tuple_term[i], symbol_assignment, match_failure_log) && retval;
    return retval;
}

// This assumes that the gross pattern matches, but must still verify that duplicated symbols must match.
// It will return false only if the match failed because of mismatching symbols.
bool attempt_pattern_match__ArrayETerm_c (sept::ArrayETerm_c const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log &match_failure_log) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    assert(sept::inhabits_data(pattern, sept::ArrayE));
    assert(sept::inhabits_data(term, pattern));
    auto const &array_e_term = term.cast<sept::ArrayETerm_c const &>();
    return attempt_pattern_match__data(pattern.element_type(), array_e_term.element_type(), symbol_assignment, match_failure_log);
}

// This assumes that the gross pattern matches, but must still verify that duplicated symbols must match.
// It will return false only if the match failed because of mismatching symbols.
bool attempt_pattern_match__ArrayTerm_c (sept::ArrayTerm_c const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log &match_failure_log) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    assert(sept::inhabits_data(pattern, sept::Array));
    assert(sept::inhabits_data(term, pattern));
    auto const &array_term = term.cast<sept::ArrayTerm_c const &>();
    bool retval = true;
    // Don't short-circuit.
    retval = attempt_pattern_match__data(pattern.size(), array_term.size(), symbol_assignment, match_failure_log) && retval;
    for (size_t i = 0; i < pattern.size(); ++i)
        retval = attempt_pattern_match__data(pattern[i], array_term[i], symbol_assignment, match_failure_log) && retval;
    return retval;
}

// This assumes that the gross pattern matches, but must still verify that duplicated symbols must match.
// It will return false only if the match failed because of mismatching symbols.
bool attempt_pattern_match__OrderedMapDCTerm_c (sept::OrderedMapDCTerm_c const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log &match_failure_log) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    assert(sept::inhabits_data(pattern, sept::OrderedMapDC));
    assert(sept::inhabits_data(term, pattern));
    auto const &ordered_map_dc_term = term.cast<sept::OrderedMapDCTerm_c const &>();
    bool retval = true;
    // Don't short-circuit.
    retval = attempt_pattern_match__data(pattern.domain(), ordered_map_dc_term.domain(), symbol_assignment, match_failure_log) && retval;
    retval = attempt_pattern_match__data(pattern.codomain(), ordered_map_dc_term.codomain(), symbol_assignment, match_failure_log) && retval;
    return retval;
}

// This assumes that the gross pattern matches, but must still verify that duplicated symbols must match.
// It will return false only if the match failed because of mismatching symbols.
bool attempt_pattern_match__OrderedMapDTerm_c (sept::OrderedMapDTerm_c const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log &match_failure_log) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    assert(sept::inhabits_data(pattern, sept::OrderedMapD));
    assert(sept::inhabits_data(term, pattern));
    auto const &ordered_map_d_term = term.cast<sept::OrderedMapDTerm_c const &>();
    bool retval = true;
    // Don't short-circuit.
    retval = attempt_pattern_match__data(pattern.domain(), ordered_map_d_term.domain(), symbol_assignment, match_failure_log) && retval;
    return retval;
}

// This assumes that the gross pattern matches, but must still verify that duplicated symbols must match.
// It will return false only if the match failed because of mismatching symbols.
bool attempt_pattern_match__OrderedMapCTerm_c (sept::OrderedMapCTerm_c const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log &match_failure_log) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    assert(sept::inhabits_data(pattern, sept::OrderedMapC));
    assert(sept::inhabits_data(term, pattern));
    auto const &ordered_map_c_term = term.cast<sept::OrderedMapCTerm_c const &>();
    bool retval = true;
    // Don't short-circuit.
    retval = attempt_pattern_match__data(pattern.codomain(), ordered_map_c_term.codomain(), symbol_assignment, match_failure_log) && retval;
    return retval;
}

// This assumes that the gross pattern matches, but must still verify that duplicated symbols must match.
// It will return false only if the match failed because of mismatching symbols.
bool attempt_pattern_match__UnionTerm_c (sept::UnionTerm_c const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log &match_failure_log) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    assert(sept::inhabits_data(pattern, sept::Union));
    assert(sept::inhabits_data(term, pattern));
    auto const &union_term = term.cast<sept::UnionTerm_c const &>();
    bool retval = true;
    // Don't short-circuit.
    retval = attempt_pattern_match__data(pattern.size(), union_term.size(), symbol_assignment, match_failure_log) && retval;
    for (size_t i = 0; i < pattern.size(); ++i)
        retval = attempt_pattern_match__data(pattern[i], union_term[i], symbol_assignment, match_failure_log) && retval;
    return retval;
}

// This assumes that the gross pattern matches, but must still verify that duplicated symbols must match.
// It will return false only if the match failed because of mismatching symbols.
bool attempt_pattern_match__FormalTypeOf_Term_c (sept::FormalTypeOf_Term_c const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log &match_failure_log) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    assert(sept::inhabits_data(pattern, sept::FormalTypeOf));
    assert(sept::inhabits_data(term, pattern));
    auto const &formal_type_of__term = term.cast<sept::FormalTypeOf_Term_c const &>();
    return attempt_pattern_match__data(pattern.term(), formal_type_of__term.term(), symbol_assignment, match_failure_log);
}

bool attempt_pattern_match__direct_equality (sept::Data const &pattern, sept::Data const &term, lvd::Log &match_failure_log) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    if (!sept::eq_data(term, pattern)) {
        match_failure_log << "pattern " << pattern << " failed direct equality match against term " << term << '\n';
        return false;
    }
    return true;
}

bool attempt_pattern_match__data (sept::Data const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log &match_failure_log) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    // Dispatch based on type.
    // TODO: This should use StaticAssociation_t
    if (false) {
        // SPLUNGE
    } else if (sept::inhabits_data(pattern, sept::FreeVar)) {
        return attempt_pattern_match__FreeVarTerm_c(pattern.cast<sept::FreeVarTerm_c const &>(), term, symbol_assignment, match_failure_log);
    } else if (sept::inhabits_data(pattern, sept::Tuple)) {
        return attempt_pattern_match__TupleTerm_c(pattern.cast<sept::TupleTerm_c const &>(), term, symbol_assignment, match_failure_log);
    } else if (sept::inhabits_data(pattern, sept::ArrayE)) {
        return attempt_pattern_match__ArrayETerm_c(pattern.cast<sept::ArrayETerm_c const &>(), term, symbol_assignment, match_failure_log);
    } else if (sept::inhabits_data(pattern, sept::Array)) {
        return attempt_pattern_match__ArrayTerm_c(pattern.cast<sept::ArrayTerm_c const &>(), term, symbol_assignment, match_failure_log);
    } else if (sept::inhabits_data(pattern, sept::OrderedMapDC)) {
        return attempt_pattern_match__OrderedMapDCTerm_c(pattern.cast<sept::OrderedMapDCTerm_c const &>(), term, symbol_assignment, match_failure_log);
    } else if (sept::inhabits_data(pattern, sept::OrderedMapD)) {
        return attempt_pattern_match__OrderedMapDTerm_c(pattern.cast<sept::OrderedMapDTerm_c const &>(), term, symbol_assignment, match_failure_log);
    } else if (sept::inhabits_data(pattern, sept::OrderedMapC)) {
        return attempt_pattern_match__OrderedMapCTerm_c(pattern.cast<sept::OrderedMapCTerm_c const &>(), term, symbol_assignment, match_failure_log);
    } else if (sept::inhabits_data(pattern, sept::Union)) {
        return attempt_pattern_match__UnionTerm_c(pattern.cast<sept::UnionTerm_c const &>(), term, symbol_assignment, match_failure_log);
    } else if (sept::inhabits_data(pattern, sept::FormalTypeOf)) {
        return attempt_pattern_match__FormalTypeOf_Term_c(pattern.cast<sept::FormalTypeOf_Term_c const &>(), term, symbol_assignment, match_failure_log);
    } else {
        // Anything else has to match with direct equality.
        return attempt_pattern_match__direct_equality(pattern, term, match_failure_log);
    }
}

std::optional<Match> attempt_pattern_match (sept::Data const &pattern, sept::Data const &term) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    if (!inhabits_data(term, pattern))
        return std::nullopt;

    // If the term inhabits pattern, then we have a match (unless there are failed constraints on
    // the free variables).  Now it's just a matter of determining the value of the variables in
    // the pattern.
    sept::SymbolTable symbol_assignment;
    std::ostringstream match_failure_out;
    lvd::Log match_failure_log(match_failure_out);
    if (!attempt_pattern_match__data(pattern, term, symbol_assignment, match_failure_log)) {
        lvd::g_log << lvd::Log::dbg() << "attempt_pattern_match failure:\n" << lvd::IndentGuard() << match_failure_out.str() << '\n';
        return std::nullopt;
    }
    return std::make_optional<Match>(term, std::move(symbol_assignment));
}

// // TEMP -- TODO: Return all matches
// // TODO: Eventually make a stochastic version that only checks some fraction of beliefs.
// Match first_match (BeliefSystem const &bs, sept::Data const &pattern) {
//     for (auto const &belief : bs.belief_set()) {
//         // Do only full match for now.
//         if (inhabits_data(belief, pattern)) {
//             // Now for each FreeVarTerm_c in pattern, assign a symbol.
//             TODO start here
//             return Match(
//         }
//     }
// }

//
// END TESTING
//


// TEMP HACK
namespace std {

template <typename T_>
inline ostream &operator << (ostream &out, optional<T_> const &x) {
    out << "Optional(";
    if (x.has_value())
        out << x.value();
    else
        out << "<no-value>";
    return out << ')';
}

} // end namespace std

int main (int argc, char **argv) {
//     lvd::g_log.set_log_level_threshold(lvd::LogLevel::DBG);
    lvd::g_log.out().precision(std::numeric_limits<double>::max_digits10+1);
    lvd::g_log.out().setf(std::ios_base::boolalpha, std::ios_base::boolalpha);

    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(Adjective) << '\n'
               << LVD_REFLECT(BinOp) << '\n'
               << LVD_REFLECT(Color) << '\n'
               << LVD_REFLECT(Entity) << '\n'
               << LVD_REFLECT(Object) << '\n'
               << LVD_REFLECT(Person) << '\n'
               << LVD_REFLECT(UnOp) << '\n'
               << LVD_REFLECT(Verb) << '\n'
               << LVD_REFLECT(SubjVerbObj) << '\n'
               << '\n';

    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(sept::inhabits_data(Not, BoolBinOp)) << '\n'
               << LVD_REFLECT(sept::inhabits_data(And, BoolBinOp)) << '\n'
               << LVD_REFLECT(sept::inhabits_data(Or, BoolBinOp)) << '\n'
               << LVD_REFLECT(sept::inhabits_data(Xor, BoolBinOp)) << '\n'
               << '\n';

    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(demorganize_data(Bob)) << '\n'
               << LVD_REFLECT(demorganize_data(Predicate_Not(Not, SubjVerbObj(Bob, LikesEntity, Box)))) << '\n'
               << LVD_REFLECT(demorganize_data(Predicate_And(And, sept::Tuple(SubjVerbObj(Bob, LikesEntity, Box), SubjVerbObj(Bob, LikesEntity, Cup))))) << '\n'
               << LVD_REFLECT(demorganize_data(Predicate_Not(Not, Predicate_And(And, sept::Tuple(SubjVerbObj(Bob, LikesEntity, Box), SubjVerbObj(Bob, LikesEntity, Cup)))))) << '\n'
               << '\n';
//     demorganize_data(Predicate_Not(Not, Predicate_And(And, sept::Tuple(SubjVerbObj(Bob, LikesEntity, Box), SubjVerbObj(Bob, LikesEntity, Cup)))));

    BeliefSystem bs;
    bs.add_belief(SubjVerbObj(Alice, LikesEntity, Bob));
    bs.add_belief(Predicate_Not(Not, SubjVerbObj(Bob, LikesEntity, Alice)));
    bs.add_belief(SubjVerbObj(Box, HasProperty, Red));
    bs.add_belief(SubjVerbObj(Box, HasProperty, Big));
    bs.add_belief(Predicate_And(And, sept::Tuple(SubjVerbObj(Cup, HasProperty, Blue), SubjVerbObj(Cup, HasProperty, Smart))));
    bs.add_belief(Predicate_Or(Or, sept::Tuple(SubjVerbObj(Hat, HasProperty, Green), SubjVerbObj(Hat, HasProperty, Small))));
    bs.add_belief(SubjVerbObj(Charlie, Says, SubjVerbObj(Dave, LikesA, Cat)));
    lvd::g_log << lvd::Log::dbg() << bs << '\n';

    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(bs.evaluate_predicate(SubjVerbObj(Alice, LikesEntity, Bob))) << '\n'
               << LVD_REFLECT(bs.evaluate_predicate(SubjVerbObj(Alice, LikesEntity, Charlie))) << '\n'
               << LVD_REFLECT(bs.evaluate_predicate(SubjVerbObj(Bob, LikesEntity, Alice))) << '\n'
               << LVD_REFLECT(bs.evaluate_predicate(SubjVerbObj(Box, HasProperty, Red))) << '\n'
               << LVD_REFLECT(bs.evaluate_predicate(SubjVerbObj(Box, HasProperty, Big))) << '\n'
               << LVD_REFLECT(sept::Tuple(SubjVerbObj(Box, HasProperty, Red), SubjVerbObj(Box, HasProperty, Big))) << '\n'
               << LVD_REFLECT(bs.evaluate_predicate(Predicate_And(And, sept::Tuple(SubjVerbObj(Box, HasProperty, Red), SubjVerbObj(Box, HasProperty, Big))))) << '\n'
               << LVD_REFLECT(bs.evaluate_predicate(Predicate_And(And, sept::Tuple(SubjVerbObj(Box, HasProperty, Red), SubjVerbObj(Box, HasProperty, Stupid))))) << '\n'
               << LVD_REFLECT(bs.evaluate_predicate(Predicate_And(And, sept::Tuple(SubjVerbObj(Box, HasProperty, Red), Predicate_Not(Not, SubjVerbObj(Box, HasProperty, Big)))))) << '\n'
               << '\n';

    auto rule0 = Implication(SubjVerbObj(Charlie, HasProperty, Smart), Implies, SubjVerbObj(Charlie, LikesA, Cat));
    auto rule1 = Implication(SubjVerbObj(Dave, HasProperty, Smart), Implies, SubjVerbObj(Dave, LikesA, Cat));

    lvd::g_log << lvd::Log::dbg() << "testing non-actionable implication " << rule0 << " ...\n";
    bs.derive_beliefs(rule0);
    lvd::g_log << lvd::Log::dbg() << "no action should have been taken.\n\n";

    lvd::g_log << lvd::Log::dbg() << "adding belief...\n";
    bs.add_belief(SubjVerbObj(Charlie, HasProperty, Smart));
    lvd::g_log << lvd::Log::dbg() << "testing actionable (direct) implication...\n";
    bs.derive_beliefs(rule0);
    lvd::g_log << lvd::Log::dbg() << '\n';

    lvd::g_log << lvd::Log::dbg() << "testing non-actionable implication " << rule1 << " ...\n";
    bs.derive_beliefs(rule1);
    lvd::g_log << lvd::Log::dbg() << "no action should have been taken.\n\n";

    lvd::g_log << lvd::Log::dbg() << "adding belief...\n";
    bs.add_belief(Predicate_Not(Not, SubjVerbObj(Dave, LikesA, Cat)));
    lvd::g_log << lvd::Log::dbg() << "testing actionable (contrapositive) implication\n";
    bs.derive_beliefs(rule1);
    lvd::g_log << lvd::Log::dbg() << '\n';

    // Testing

    auto X = sept::FreeVar("X");
    auto Y = sept::FreeVar("Y");
    auto rule2 = Implication(SubjVerbObj(X, LikesEntity, Y), Implies, SubjVerbObj(X, IsA, Person));
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(rule2) << '\n';

    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(attempt_pattern_match(X, sept::Uint32(123))) << '\n';

    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(attempt_pattern_match(sept::Tuple(X), sept::Tuple(sept::Uint32(123)))) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(attempt_pattern_match(sept::Tuple(X, Y), sept::Tuple(sept::Uint32(123), sept::Float64(456.75)))) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(attempt_pattern_match(sept::Tuple(X, X), sept::Tuple(sept::Uint32(123), sept::Float64(456.75)))) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(attempt_pattern_match(sept::Tuple(X, X), sept::Tuple(sept::Uint32(123), sept::Uint32(123)))) << '\n';

    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(attempt_pattern_match(sept::ArrayE(X), sept::ArrayE(sept::Uint32))) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(attempt_pattern_match(sept::Array(X,Y), sept::Array(sept::Uint32(123), sept::Float64(456.75)))) << '\n';

    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(attempt_pattern_match(sept::OrderedMapDC(X,Y), sept::OrderedMapDC(sept::Uint32, sept::Float64(456.75)))) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(attempt_pattern_match(sept::OrderedMapD(X), sept::OrderedMapD(sept::Uint32))) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(attempt_pattern_match(sept::OrderedMapC(Y), sept::OrderedMapC(sept::Uint32))) << '\n';

    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(attempt_pattern_match(sept::Union(X), sept::Union(sept::Uint32))) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(attempt_pattern_match(sept::Union(X,Y), sept::Union(sept::Uint32, sept::Float64))) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(attempt_pattern_match(sept::Union(X,X), sept::Union(sept::Uint32, sept::Uint32))) << '\n';

    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(attempt_pattern_match(sept::FormalTypeOf(X), sept::FormalTypeOf(sept::Uint32(3)))) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(attempt_pattern_match(sept::FormalTypeOf(X), sept::FormalTypeOf(sept::Void))) << '\n';

    // Now test some nested ones.
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(attempt_pattern_match(sept::Tuple(sept::ArrayE(X), sept::OrderedMapDC(X,Y)), sept::Tuple(sept::ArrayE(sept::Uint32), sept::OrderedMapDC(sept::Uint32,sept::Float64)))) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(attempt_pattern_match(sept::OrderedMapDC(sept::ArrayE(X),sept::ArrayE(X)), sept::OrderedMapDC(sept::ArrayE(sept::Uint32),sept::ArrayE(sept::Uint32)))) << '\n';

    return 0;
}
