// 2021.05.16 - Victor Dods

#include "pattern.hpp"
#include "sept/ArrayTerm.hpp"
#include "sept/ArrayType.hpp"
#include "sept/FreeVar.hpp"
#include "sept/NPTerm.hpp"
#include "sept/OrderedMapType.hpp"
#include "sept/Tuple.hpp"
#include "sept/Union.hpp"
#include "sept/UnionTerm.hpp"

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

bool attempt_pattern_match__data (sept::Data const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log &match_failure_log);

// This assumes that the gross pattern matches, but must still verify that duplicated symbols must match.
// It will return false only if the match failed because of mismatching symbols.
bool attempt_pattern_match__FreeVarTerm_c (sept::FreeVarTerm_c const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log &match_failure_log) {
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
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
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
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
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    assert(sept::inhabits_data(pattern, sept::ArrayE));
    assert(sept::inhabits_data(term, pattern));
    auto const &array_e_term = term.cast<sept::ArrayETerm_c const &>();
    return attempt_pattern_match__data(pattern.element_type(), array_e_term.element_type(), symbol_assignment, match_failure_log);
}

// This assumes that the gross pattern matches, but must still verify that duplicated symbols must match.
// It will return false only if the match failed because of mismatching symbols.
bool attempt_pattern_match__ArrayTerm_c (sept::ArrayTerm_c const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log &match_failure_log) {
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
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
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
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
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
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
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
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
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
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
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    assert(sept::inhabits_data(pattern, sept::FormalTypeOf));
    assert(sept::inhabits_data(term, pattern));
    auto const &formal_type_of__term = term.cast<sept::FormalTypeOf_Term_c const &>();
    return attempt_pattern_match__data(pattern.term(), formal_type_of__term.term(), symbol_assignment, match_failure_log);
}

bool attempt_pattern_match__direct_equality (sept::Data const &pattern, sept::Data const &term, lvd::Log &match_failure_log) {
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    if (!sept::eq_data(term, pattern)) {
        match_failure_log << "pattern " << pattern << " failed direct equality match against term " << term << '\n';
        return false;
    }
    return true;
}

bool attempt_pattern_match__data (sept::Data const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log &match_failure_log) {
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
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

std::optional<Match> attempt_pattern_match (sept::Data const &pattern, sept::Data &&term) {
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
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
    return std::make_optional<Match>(std::move(term), std::move(symbol_assignment));
}
