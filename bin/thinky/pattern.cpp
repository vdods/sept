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

bool matched_pattern__data (sept::Data const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log *match_failure_log = nullptr);

// This assumes that the gross pattern matches, but must still verify that duplicated symbols must match.
// It will return false only if the match failed because of mismatching symbols.
bool matched_pattern__FreeVarTerm_c (sept::FreeVarTerm_c const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log *match_failure_log = nullptr) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    auto ig = lvd::IndentGuard(lvd::g_log);

    if (symbol_assignment.symbol_is_defined(pattern.free_var_id__as_string())) {
        sept::Data const &sa = symbol_assignment.resolve_symbol_const(pattern.free_var_id__as_string());
        if (sa != term) {
            // This represents a repeated pattern-match symbol (e.g. Tuple(FreeVar("X"), FreeVar("X"))) where
            // the second match of FreeVar("X") doesn't have the same value as the first.
            if (match_failure_log != nullptr)
                *match_failure_log << "pattern " << pattern << " had [at least] two distinct matches -- the first was " << sa << " and the second was " << term << '\n';
            return false;
        }
    } else {
        symbol_assignment.define_symbol(pattern.free_var_id__as_string(), term);
    }
    return true;
}

// This assumes that the gross pattern matches, but must still verify that duplicated symbols must match.
// It will return false only if the match failed because of mismatching symbols.
bool matched_pattern__TupleTerm_c (sept::TupleTerm_c const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log *match_failure_log = nullptr) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    auto ig = lvd::IndentGuard(lvd::g_log);

    auto const &tuple_term = term.cast<sept::TupleTerm_c const &>();
    // A Tuple pattern can only match if there are an equal number of elements (for now).
    if (pattern.size() != tuple_term.size())
        return false;
    // Match on the components of each -- don't early out via logic short-circuit..
    bool retval = true;
    for (size_t i = 0; i < pattern.size(); ++i)
        retval = matched_pattern__data(pattern[i], tuple_term[i], symbol_assignment, match_failure_log) && retval;
    return retval;
}

// This assumes that the gross pattern matches, but must still verify that duplicated symbols must match.
// It will return false only if the match failed because of mismatching symbols.
bool matched_pattern__ArrayETerm_c (sept::ArrayETerm_c const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log *match_failure_log = nullptr) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    auto ig = lvd::IndentGuard(lvd::g_log);

    auto const &array_e_term = term.cast<sept::ArrayETerm_c const &>();
    return matched_pattern__data(pattern.element_type(), array_e_term.element_type(), symbol_assignment, match_failure_log);
}

// This assumes that the gross pattern matches, but must still verify that duplicated symbols must match.
// It will return false only if the match failed because of mismatching symbols.
bool matched_pattern__ArrayTerm_c (sept::ArrayTerm_c const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log *match_failure_log = nullptr) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    auto ig = lvd::IndentGuard(lvd::g_log);

    auto const &array_term = term.cast<sept::ArrayTerm_c const &>();
    bool retval = true;
    // Don't short-circuit.
    retval = matched_pattern__data(pattern.size(), array_term.size(), symbol_assignment, match_failure_log) && retval;
    for (size_t i = 0; i < pattern.size(); ++i)
        retval = matched_pattern__data(pattern[i], array_term[i], symbol_assignment, match_failure_log) && retval;
    return retval;
}

// This assumes that the gross pattern matches, but must still verify that duplicated symbols must match.
// It will return false only if the match failed because of mismatching symbols.
bool matched_pattern__OrderedMapDCTerm_c (sept::OrderedMapDCTerm_c const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log *match_failure_log = nullptr) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    auto ig = lvd::IndentGuard(lvd::g_log);

    auto const &ordered_map_dc_term = term.cast<sept::OrderedMapDCTerm_c const &>();
    bool retval = true;
    // Don't short-circuit.
    retval = matched_pattern__data(pattern.domain(), ordered_map_dc_term.domain(), symbol_assignment, match_failure_log) && retval;
    retval = matched_pattern__data(pattern.codomain(), ordered_map_dc_term.codomain(), symbol_assignment, match_failure_log) && retval;
    return retval;
}

// This assumes that the gross pattern matches, but must still verify that duplicated symbols must match.
// It will return false only if the match failed because of mismatching symbols.
bool matched_pattern__OrderedMapDTerm_c (sept::OrderedMapDTerm_c const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log *match_failure_log = nullptr) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    auto ig = lvd::IndentGuard(lvd::g_log);

    auto const &ordered_map_d_term = term.cast<sept::OrderedMapDTerm_c const &>();
    bool retval = true;
    // Don't short-circuit.
    retval = matched_pattern__data(pattern.domain(), ordered_map_d_term.domain(), symbol_assignment, match_failure_log) && retval;
    return retval;
}

// This assumes that the gross pattern matches, but must still verify that duplicated symbols must match.
// It will return false only if the match failed because of mismatching symbols.
bool matched_pattern__OrderedMapCTerm_c (sept::OrderedMapCTerm_c const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log *match_failure_log = nullptr) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    auto ig = lvd::IndentGuard(lvd::g_log);

    auto const &ordered_map_c_term = term.cast<sept::OrderedMapCTerm_c const &>();
    bool retval = true;
    // Don't short-circuit.
    retval = matched_pattern__data(pattern.codomain(), ordered_map_c_term.codomain(), symbol_assignment, match_failure_log) && retval;
    return retval;
}

// This assumes that the gross pattern matches, but must still verify that duplicated symbols must match.
// It will return false only if the match failed because of mismatching symbols.
bool matched_pattern__UnionTerm_c (sept::UnionTerm_c const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log *match_failure_log = nullptr) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    auto ig = lvd::IndentGuard(lvd::g_log);

    auto const &union_term = term.cast<sept::UnionTerm_c const &>();
    bool retval = true;
    // Don't short-circuit.
    retval = matched_pattern__data(pattern.size(), union_term.size(), symbol_assignment, match_failure_log) && retval;
    for (size_t i = 0; i < pattern.size(); ++i)
        retval = matched_pattern__data(pattern[i], union_term[i], symbol_assignment, match_failure_log) && retval;
    return retval;
}

// Pattern FormalTypeOf(P) will match FormalTypeOf(T) iff T matches pattern P.
// This assumes that the gross pattern matches, but must still verify that duplicated symbols must match.
// It will return false only if the match failed because of mismatching symbols.
bool matched_pattern__FormalTypeOf_Term_c (sept::FormalTypeOf_Term_c const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log *match_failure_log = nullptr) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    auto ig = lvd::IndentGuard(lvd::g_log);

    auto const &formal_type_of__term = term.cast<sept::FormalTypeOf_Term_c const &>();
    return matched_pattern__data(pattern.term(), formal_type_of__term.term(), symbol_assignment, match_failure_log);
}

bool matched_pattern__direct_equality (sept::Data const &pattern, sept::Data const &term, lvd::Log *match_failure_log = nullptr) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    auto ig = lvd::IndentGuard(lvd::g_log);

    if (!sept::eq_data(term, pattern)) {
        if (match_failure_log != nullptr)
            *match_failure_log << "pattern " << pattern << " failed direct equality match against term " << term << '\n';
        return false;
    }
    return true;
}

bool matched_pattern__data (sept::Data const &pattern, sept::Data const &term, sept::SymbolTable &symbol_assignment, lvd::Log *match_failure_log) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    auto ig = lvd::IndentGuard(lvd::g_log);

    // Dispatch based on type.
    // TODO: This should use StaticAssociation_t
    if (false) {
        // SPLUNGE
    } else if (auto *pattern__FreeVar_Term_c = pattern.ptr_cast<sept::FreeVarTerm_c>()) {
        return matched_pattern__FreeVarTerm_c(*pattern__FreeVar_Term_c, term, symbol_assignment, match_failure_log);
    } else if (auto *pattern__TupleTerm_c = pattern.ptr_cast<sept::TupleTerm_c>()) {
        return matched_pattern__TupleTerm_c(*pattern__TupleTerm_c, term, symbol_assignment, match_failure_log);
    } else if (auto *pattern__ArrayETerm_c = pattern.ptr_cast<sept::ArrayETerm_c>()) {
        return matched_pattern__ArrayETerm_c(*pattern__ArrayETerm_c, term, symbol_assignment, match_failure_log);
    } else if (auto *pattern__ArrayTerm_c = pattern.ptr_cast<sept::ArrayTerm_c>()) {
        return matched_pattern__ArrayTerm_c(*pattern__ArrayTerm_c, term, symbol_assignment, match_failure_log);
    } else if (auto *pattern__OrderedMapDCTerm_c = pattern.ptr_cast<sept::OrderedMapDCTerm_c>()) {
        return matched_pattern__OrderedMapDCTerm_c(*pattern__OrderedMapDCTerm_c, term, symbol_assignment, match_failure_log);
    } else if (auto *pattern__OrderedMapDTerm_c = pattern.ptr_cast<sept::OrderedMapDTerm_c>()) {
        return matched_pattern__OrderedMapDTerm_c(*pattern__OrderedMapDTerm_c, term, symbol_assignment, match_failure_log);
    } else if (auto *pattern__OrderedMapCTerm_c = pattern.ptr_cast<sept::OrderedMapCTerm_c>()) {
        return matched_pattern__OrderedMapCTerm_c(*pattern__OrderedMapCTerm_c, term, symbol_assignment, match_failure_log);
    } else if (auto *pattern__UnionTerm_c = pattern.ptr_cast<sept::UnionTerm_c>()) {
        return matched_pattern__UnionTerm_c(*pattern__UnionTerm_c, term, symbol_assignment, match_failure_log);
    } else if (auto *pattern__FormalTypeOf_Term_c = pattern.ptr_cast<sept::FormalTypeOf_Term_c>()) {
        return matched_pattern__FormalTypeOf_Term_c(*pattern__FormalTypeOf_Term_c, term, symbol_assignment, match_failure_log);
    } else {
        // Anything else has to match with direct equality.
        return matched_pattern__direct_equality(pattern, term, match_failure_log);
    }
}

// TODO: If no match is made, then the rvalue ref `term` is discarded, which is a waste.
std::optional<Match> matched_pattern__data (sept::Data const &pattern, sept::Data &&term, lvd::Log *match_failure_log) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(pattern) << ", " << LVD_REFLECT(term) << '\n';
    auto ig = lvd::IndentGuard(lvd::g_log);

    sept::SymbolTable symbol_assignment;
    if (matched_pattern__data(pattern, term, symbol_assignment, match_failure_log))
        return std::make_optional<Match>(std::move(term), std::move(symbol_assignment));
    else
        return std::nullopt;

}

//
// free_var_substitution
//

sept::Data free_var_substitution__FreeVarTerm_c (sept::FreeVarTerm_c const &term, sept::SymbolTable const &symbol_assignment) noexcept {
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(term) << '\n';
    // If the symbol is defined, replace it.  Otherwise, just return the FreeVarTerm_c unmodified.
    if (symbol_assignment.symbol_is_defined(term.free_var_id__as_string()))
        return symbol_assignment.resolve_symbol_const(term.free_var_id__as_string());
    else
        return term;
}

sept::Data free_var_substitution__TupleTerm_c (sept::TupleTerm_c const &term, sept::SymbolTable const &symbol_assignment) noexcept {
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(term) << '\n';
    sept::DataVector elements;
    elements.reserve(term.size());
    for (auto const &term_element : term.elements())
        elements.emplace_back(free_var_substitution__data(term_element, symbol_assignment));
    return sept::TupleTerm_c{std::move(elements)};
}

sept::Data free_var_substitution__ArrayETerm_c (sept::ArrayETerm_c const &term, sept::SymbolTable const &symbol_assignment) noexcept {
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(term) << '\n';
    return sept::ArrayE(
        free_var_substitution__data(term.element_type(), symbol_assignment)
    );
}

sept::Data free_var_substitution__ArrayTerm_c (sept::ArrayTerm_c const &term, sept::SymbolTable const &symbol_assignment) noexcept {
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(term) << '\n';
    sept::DataVector elements;
    elements.reserve(term.size());
    for (auto const &term_element : term.elements())
        elements.emplace_back(free_var_substitution__data(term_element, symbol_assignment));
    return sept::ArrayTerm_c{std::move(elements)};
}

sept::Data free_var_substitution__OrderedMapDCTerm_c (sept::OrderedMapDCTerm_c const &term, sept::SymbolTable const &symbol_assignment) noexcept {
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(term) << '\n';
    return sept::OrderedMapDC(
        free_var_substitution__data(term.domain(), symbol_assignment),
        free_var_substitution__data(term.codomain(), symbol_assignment)
    );
}

sept::Data free_var_substitution__OrderedMapDTerm_c (sept::OrderedMapDTerm_c const &term, sept::SymbolTable const &symbol_assignment) noexcept {
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(term) << '\n';
    return sept::OrderedMapD(
        free_var_substitution__data(term.domain(), symbol_assignment)
    );
}

sept::Data free_var_substitution__OrderedMapCTerm_c (sept::OrderedMapCTerm_c const &term, sept::SymbolTable const &symbol_assignment) noexcept {
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(term) << '\n';
    return sept::OrderedMapC(
        free_var_substitution__data(term.codomain(), symbol_assignment)
    );
}

sept::Data free_var_substitution__UnionTerm_c (sept::UnionTerm_c const &term, sept::SymbolTable const &symbol_assignment) noexcept {
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(term) << '\n';
    sept::DataVector elements;
    elements.reserve(term.size());
    for (auto const &term_element : term.elements())
        elements.emplace_back(free_var_substitution__data(term_element, symbol_assignment));
    return sept::UnionTerm_c{std::move(elements)};
}

sept::Data free_var_substitution__FormalTypeOf_Term_c (sept::FormalTypeOf_Term_c const &term, sept::SymbolTable const &symbol_assignment) noexcept {
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(term) << '\n';
    return sept::FormalTypeOf(
        free_var_substitution__data(term.term(), symbol_assignment)
    );
}

sept::Data free_var_substitution__no_op (sept::Data const &term, sept::SymbolTable const &symbol_assignment) {
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(term) << '\n';
    return term;
}

sept::Data free_var_substitution__data (sept::Data const &term, sept::SymbolTable const &symbol_assignment) noexcept {
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(term) << '\n';
    // Dispatch based on type.
    // TODO: This should use StaticAssociation_t
    if (false) {
        // SPLUNGE
    } else if (sept::inhabits_data(term, sept::FreeVar)) {
        return free_var_substitution__FreeVarTerm_c(term.cast<sept::FreeVarTerm_c const &>(), symbol_assignment);
    } else if (sept::inhabits_data(term, sept::Tuple)) {
        return free_var_substitution__TupleTerm_c(term.cast<sept::TupleTerm_c const &>(), symbol_assignment);
    } else if (sept::inhabits_data(term, sept::ArrayE)) {
        return free_var_substitution__ArrayETerm_c(term.cast<sept::ArrayETerm_c const &>(), symbol_assignment);
    } else if (sept::inhabits_data(term, sept::Array)) {
        return free_var_substitution__ArrayTerm_c(term.cast<sept::ArrayTerm_c const &>(), symbol_assignment);
    } else if (sept::inhabits_data(term, sept::OrderedMapDC)) {
        return free_var_substitution__OrderedMapDCTerm_c(term.cast<sept::OrderedMapDCTerm_c const &>(), symbol_assignment);
    } else if (sept::inhabits_data(term, sept::OrderedMapD)) {
        return free_var_substitution__OrderedMapDTerm_c(term.cast<sept::OrderedMapDTerm_c const &>(), symbol_assignment);
    } else if (sept::inhabits_data(term, sept::OrderedMapC)) {
        return free_var_substitution__OrderedMapCTerm_c(term.cast<sept::OrderedMapCTerm_c const &>(), symbol_assignment);
    } else if (sept::inhabits_data(term, sept::Union)) {
        return free_var_substitution__UnionTerm_c(term.cast<sept::UnionTerm_c const &>(), symbol_assignment);
    } else if (sept::inhabits_data(term, sept::FormalTypeOf)) {
        return free_var_substitution__FormalTypeOf_Term_c(term.cast<sept::FormalTypeOf_Term_c const &>(), symbol_assignment);
    } else {
        // Anything else has to substitution with direct equality.
        return free_var_substitution__no_op(term, symbol_assignment);
    }
}
