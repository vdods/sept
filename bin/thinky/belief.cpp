// 2021.05.15 - Victor Dods

#include "belief.hpp"

#include "ast.hpp"
#include "common.hpp"
#include "logic.hpp"
#include "pattern.hpp"
#include <lvd/abort.hpp>
#include <lvd/cloned.hpp>
#include <lvd/comma.hpp>
#include <lvd/fmt.hpp>

void add_belief_to (BeliefSet &belief_set, sept::Data const &belief) {
    auto demorganized_belief = demorganize_data(belief);
    // If a belief is Predicate_And, then it can be broken up into separate beliefs and each one added.
    // Otherwise it's just added as is.
    if (inhabits_data(demorganized_belief, Predicate_And)) {
        auto operand_tuple = demorganized_belief[1].move_cast<sept::TupleTerm_c>();
        for (auto const &operand : operand_tuple.elements()) {
            lvd::g_log << lvd::Log::inf() << "adding belief (to " << &belief_set << "): " << operand << '\n';
            belief_set.insert(operand);
        }
    } else {
        lvd::g_log << lvd::Log::inf() << "adding belief (to " << &belief_set << "): " << belief << '\n';
        belief_set.insert(belief);
    }
}

//
// Remaining stuff
//

Trit BeliefSystem::evaluate_predicate (sept::Data const &predicate) const {
    // Check if the predicate is a verbatim belief already.
    if (contains_belief(predicate))
        return Yep;
    // Also check the negation.  Not very efficient.  Also this won't work in general because
    // stuff isn't put into a canonical form using deMorgan's laws.
    if (contains_belief(Predicate(Not, predicate)))
        return Nope;

    // TODO: Use StaticAssociation_t
    if (false) {
        // SPLUNGE
    } else if (inhabits_data(predicate, Implication)) {
        // TODO: Extract functions
        auto premise = predicate[0];
        auto conclusion = predicate[1];
        return or__trit(not__trit(evaluate_predicate(premise)), evaluate_predicate(conclusion));
    } else if (inhabits_data(predicate, Predicate_Not)) {
        return not__trit(evaluate_predicate(predicate[1]));
    } else if (inhabits_data(predicate, Predicate_And)) {
        auto operand_tuple = predicate[1].move_cast<sept::TupleTerm_c>();
        Trit retval = Yep; // Identity element of `and` operation.
        for (auto const &operand : operand_tuple.elements()) {
            retval = and__trit(retval, evaluate_predicate(operand));
            // If we break away from Yep, then it could be Nope or Kwatz, either of which causes an early out.
            if (retval != Yep)
                return retval;
        }
        return retval;
    } else if (inhabits_data(predicate, Predicate_Or)) {
        auto operand_tuple = predicate[1].move_cast<sept::TupleTerm_c>();
        Trit retval = Nope; // Identity element of `or` operation.
        for (auto const &operand : operand_tuple.elements()) {
            retval = or__trit(retval, evaluate_predicate(operand));
            // If we break away from Nope, then it could be Yep or Kwatz, either of which causes an early out.
            if (retval != Nope)
                return retval;
        }
        return retval;
    } else if (inhabits_data(predicate, Predicate_Xor)) {
        auto operand_tuple = predicate[1].move_cast<sept::TupleTerm_c>();
        Trit retval = Nope; // Identity element of `or` operation.
        for (auto const &operand : operand_tuple.elements()) {
            retval = or__trit(retval, evaluate_predicate(operand));
            // There is no early out for xor.
        }
        return retval;
    } else {
        // Fallthrough -- no information in the belief system.
        // TODO: If predicate occurred somewhere as a subexpression of some belief, that would
        // be useful information for an associative search.
        return Kwatz;
    }
}

void BeliefSystem::derive_beliefs (sept::Data const &inference, bool also_derive_using_contrapositive) {
    lvd::g_log << lvd::Log::dbg() << "deriving beliefs from " << LVD_REFLECT(inference) << '\n';

    // TODO: Write extractions
    auto premise = inference[0];
    assert(inference[1] == Implies);
    auto conclusion = inference[2];

    {
        std::ostringstream out;
        lvd::Log log_out(out);
        bool v = validate_inference(premise, conclusion, &log_out);
        lvd::g_log << lvd::Log::trc() << "Skipping inference " << inference << " because it's not valid; " << out.str() << '\n';
        if (!v) {
            if (!also_derive_using_contrapositive)
                return;
            else
                LVD_ABORT(out.str());
        }
    }

    BeliefSet new_belief_set;
    if (inhabits_data(premise, Predicate_And)) {
        auto parent_symbol_assignment = lvd::make_sp<sept::SymbolTable>();
        auto premise_logical_literal_tuple = premise[1].cast<sept::TupleTerm_c>();
        derive_beliefs_impl(new_belief_set, parent_symbol_assignment, premise_logical_literal_tuple, 0, conclusion);
    } else {
        lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(premise) << '\n';
        auto ig = lvd::IndentGuard(lvd::g_log);

        // TEMP HACK: Brute force search for matches.  Eventually this will be replaced with
        // a poset search.
        for (auto const &belief : belief_set()) {
            lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - checking premise " << premise << " against " << LVD_REFLECT(belief) << " ...\n";
            // Most beliefs won't match, so this clone is wasteful.  TODO: Fix.
            auto match_o = matched_pattern__data(premise, lvd::cloned(belief));
            if (match_o.has_value()) {
                auto const &match = match_o.value();
//                 lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(match) << " -- adding conclusion to belief_set...\n";
                add_belief_to(new_belief_set, free_var_substitution__data(conclusion, match.symbol_assignment()));
            }
        }
    }
    // Add the new beliefs to the BeliefSystem's belief set.
    for (auto &new_belief : new_belief_set) {
        m_belief_set.emplace(std::move(new_belief));
    }

    if (also_derive_using_contrapositive) {
        auto contrapositive = Implication(Predicate(Not, conclusion), Implies, Predicate(Not, premise));
        // Don't derive using contrapositive again, or infinite loop.
        derive_beliefs(contrapositive, false);
    }
}

void BeliefSystem::add_belief (sept::Data const &belief) {
    add_belief_to(m_belief_set, belief);
}

// TEMP HACK
namespace std {

template <typename K_, typename Hash_, typename KeyEqual_, typename Allocator_>
inline ostream &operator << (ostream &out, unordered_set<K_,Hash_,KeyEqual_,Allocator_> const &s) {
    auto d = lvd::make_comma_space_delimiter();
    out << '{';
    for (auto const &x : s)
        out << d << x;
    return out << '}';
}

} // end namespace std

bool BeliefSystem::validate_inference (sept::Data const &premise, sept::Data const &conclusion, lvd::Log *validation_failure_log) {
    if (!is_logical_literal(premise) && !is_conjunction_of_logical_literals(premise))
        return false;
    // Compute the FreeVar set that can be part of the conclusion.
    auto concludable_free_var_s = concludable_free_var_set__data(premise);
    // Compute the FreeVar set in conclusion
    auto conclusion_free_var_s = free_var_collection__data(conclusion);
    // Check the constraint.
    if (!is_subset(conclusion_free_var_s, concludable_free_var_s)) {
        if (validation_failure_log != nullptr)
            *validation_failure_log << "conclusion " << conclusion << " has non-matched free vars: " << unordered_set_difference(conclusion_free_var_s, concludable_free_var_s) << "; " << LVD_REFLECT(concludable_free_var_s) << ", " << LVD_REFLECT(conclusion_free_var_s) << "; premise was " << premise;
        return false;
    }
    // If it passed this far, it's good.
    return true;
}

bool BeliefSystem::validate_inference (sept::Data const &inference, lvd::Log *validation_failure_log) {
    if (!sept::inhabits_data(inference, Implication))
        return false;
    auto premise = inference[0];
    auto conclusion = inference[2];
    return validate_inference(premise, conclusion, validation_failure_log);
}

void BeliefSystem::derive_beliefs_impl (BeliefSet &new_belief_set, lvd::nnsp<sept::SymbolTable> const &parent_symbol_assignment, sept::TupleTerm_c const &premise_logical_literal_tuple, size_t i, sept::Data const &conclusion) {
    if (belief_set().empty())
        return;

    assert(i <= premise_logical_literal_tuple.size());
    if (i == premise_logical_literal_tuple.size()) {
        auto substituted_conclusion = free_var_substitution__data(conclusion, parent_symbol_assignment);
        lvd::g_log << lvd::Log::dbg() << "concluding " << substituted_conclusion << " from premise " << premise_logical_literal_tuple << " with symbol assignment " << *parent_symbol_assignment << '\n';
        add_belief_to(new_belief_set, substituted_conclusion);
        return;
    }

    // TEMP HACK: The way this is implemented, it's exponential in the number of elements of premise_logical_literal_tuple,
    // where the base of the exponential is the number of beliefs.  This will later be optimized to do an efficient
    // pattern-matching search through the belief_set using a poset search.
    for (auto const &belief : belief_set()) {
        // Most beliefs won't match, so this clone is wasteful.  TODO: Fix.
        auto log = lvd::g_log << lvd::Log::dbg() << "checking belief " << belief << " against pattern " << premise_logical_literal_tuple[i] << " ... ";
        auto match_o = matched_pattern__data(premise_logical_literal_tuple[i], lvd::cloned(belief), parent_symbol_assignment);
        if (match_o.has_value()) {
            auto const &match = match_o.value();
            log << "match occurred; " << LVD_REFLECT(match.symbol_assignment()) << '\n';
            // Recurse, using the match's symbol_assignment as parent for the next.
            derive_beliefs_impl(new_belief_set, match.symbol_assignment_nnsp(), premise_logical_literal_tuple, i+1, conclusion);
        } else {
            log << "no match occurred\n";
        }
    }
}

std::ostream &operator<< (std::ostream &out, BeliefSystem const &bs) {
    lvd::Log log(out);
    log << "BeliefSystem{\n";
    {
        auto ig1 = lvd::IndentGuard(log);
        log << "belief_set: {\n";
        {
            auto ig2 = lvd::IndentGuard(log);
            for (auto const &belief : bs.belief_set())
                log << belief << '\n';
        }
        log << "}\n";
    }
    log << "}\n";
    return out;
}
