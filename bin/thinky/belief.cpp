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

void BeliefSystem::derive_beliefs (sept::Data const &inference) {
    assert(inhabits_data(inference, Implication));
    // TODO: Write extractions
    auto premise = inference[0];
    assert(inference[1] == Implies);
    auto conclusion = inference[2];

    // NOTE: Because the direct implication and the contrapositive are both acted upon,
    // this could result in some redundancy.

    // Direct implication.
    switch (evaluate_predicate(premise)) {
        case Yep:
            add_belief(conclusion);
            break;
        case Kwatz:
            // TODO: If the premise is Predicate_Not/And/Or/Xor, then the unknown bit
            // could be added to some "wondering about" predicate set.
            break;
        case Nope:
            // Nothing to be inferred.
            break;
    }
    // Evaluate the contrapositive.
    switch (evaluate_predicate(conclusion)) {
        case Yep:
            // Nothing to be inferred.
            break;
        case Kwatz:
            // TODO: If the premise is Predicate_Not/And/Or/Xor, then the unknown bit
            // could be added to some "wondering about" predicate set.
            break;
        case Nope:
            add_belief(Predicate(Not, premise));
            break;
    }
}

void BeliefSystem::derive_beliefs_2 (sept::Data const &inference, bool also_derive_using_contrapositive) {
    // TODO: Write extractions
    auto premise = inference[0];
    assert(inference[1] == Implies);
    auto conclusion = inference[2];

    // TODO: pre-demorganize premise?
    auto demorganized_premise = demorganize_data(premise);
    // If the premise is Predicate_And, then it can be broken up into separate predicates and each one
    // dealt with individually.
    if (inhabits_data(demorganized_premise, Predicate_And)) {
        // TODO: Have to verify that if there are FreeVars in the conclusion, that they're all
        // present in the premise (otherwise the pattern matching and substitution will leave an
        // unbound FreeVar in the conclusion).


//         // p => q
//         // not(p) or q
//         //
//         // (a and b) => q
//         // not(a and b) or q
//         // (not(a) or not(b)) or q
//         // not(a) or (not(b) or q)
//         // a => not(b) or q
//         TODO start here -- this needs some re-thinking...
//         auto operand_tuple = demorganized_premise[1].cast<sept::TupleTerm_c>();
//         for (auto const &operand : operand_tuple.elements()) {
//             auto match_o = matched_pattern__data(premise, lvd::cloned(belief));
//             if (match_o.has_value()) {
//                 auto const &match = match_o.value();
//                 add_belief(free_var_substitution__data(conclusion, match.symbol_assignment()));
//             }
// //             lvd::g_log << lvd::Log::trc() << "adding belief: " << operand << '\n';
// //             m_belief_set.insert(operand);
//         }
        LVD_ABORT("derive_beliefs_2 not yet implemented for Predicate_And");
    } else if (inhabits_data(demorganized_premise, Predicate_Or)) {
        LVD_ABORT("derive_beliefs_2 not yet implemented for Predicate_Or");
        // TODO: Have to verify that if there are FreeVars in the conclusion, that they're all
        // present in each of the branches of the Or (otherwise the pattern matching and substitution
        // will leave an unbound FreeVar in the conclusion)
//         auto operand_tuple = demorganized_premise[1].cast<sept::TupleTerm_c>();
//         for (auto const &operand : operand_tuple.elements()) {
//             auto match_o = matched_pattern__data(premise, lvd::cloned(belief));
//             if (match_o.has_value()) {
//                 auto const &match = match_o.value();
//                 add_belief(free_var_substitution__data(conclusion, match.symbol_assignment()));
//             }
// //             lvd::g_log << lvd::Log::trc() << "adding belief: " << operand << '\n';
// //             m_belief_set.insert(operand);
//         }
    } else if (inhabits_data(demorganized_premise, Predicate_Xor)) {
        // TODO: Have to verify that if there are FreeVars in the conclusion, that they're all
        // present in the premise (otherwise the pattern matching and substitution will leave an
        // unbound FreeVar in the conclusion)
        // TODO: This would have to evaluate all operands
        LVD_ABORT("derive_beliefs_2 not yet implemented for Predicate_Xor");
    } else {
        lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(demorganized_premise) << '\n';
        auto ig = lvd::IndentGuard(lvd::g_log);

        for (auto const &belief : belief_set()) {
            lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - checking demorganized_premise against " << LVD_REFLECT(belief) << " ...\n";
            // Most beliefs won't match, so this clone is wasteful.  TODO: Fix.
            auto match_o = matched_pattern__data(demorganized_premise, lvd::cloned(belief));
            if (match_o.has_value()) {
                auto const &match = match_o.value();
                lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(match) << " -- adding conclusion to belief_set...\n";
                add_belief(free_var_substitution__data(conclusion, match.symbol_assignment()));
            }
        }
    }

    if (also_derive_using_contrapositive) {
        auto contrapositive = Implication(Predicate(Not, conclusion), Implies, Predicate(Not, premise));
        // Don't derive using contrapositive again, or infinite loop.
        derive_beliefs_2(contrapositive, false);
    }
}

void BeliefSystem::add_belief (sept::Data const &belief) {
    auto demorganized_belief = demorganize_data(belief);
    // If a belief is Predicate_And, then it can be broken up into separate beliefs and each one added.
    // Otherwise it's just added as is.
    if (inhabits_data(demorganized_belief, Predicate_And)) {
        auto operand_tuple = demorganized_belief[1].move_cast<sept::TupleTerm_c>();
        for (auto const &operand : operand_tuple.elements()) {
            lvd::g_log << lvd::Log::dbg() << "adding belief: " << operand << '\n';
            m_belief_set.insert(operand);
        }
    } else {
        lvd::g_log << lvd::Log::dbg() << "adding belief: " << belief << '\n';
        m_belief_set.insert(belief);
    }
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

bool BeliefSystem::validate_inference (sept::Data &demorganized_premise, sept::Data const &conclusion, lvd::Log *validation_failure_log) {
    // Compute the FreeVar set that can be part of the conclusion.
    auto concludable_free_var_s = concludable_free_var_set__data(demorganized_premise);
    // Compute the FreeVar set in conclusion
    auto conclusion_free_var_s = free_var_collection__data(conclusion);
    // Check the constraint.
    if (!is_subset(conclusion_free_var_s, concludable_free_var_s)) {
        if (validation_failure_log != nullptr)
            *validation_failure_log << "conclusion has non-matched free vars: " << unordered_set_difference(conclusion_free_var_s, concludable_free_var_s) << "; " << LVD_REFLECT(concludable_free_var_s) << ", " << LVD_REFLECT(conclusion_free_var_s) << '\n';
        return false;
    }
    // If it passed this far, it's good.
    return true;
}

bool BeliefSystem::validate_inference (sept::Data const &inference, lvd::Log *validation_failure_log) {
    assert(sept::inhabits_data(inference, Implication));
    auto demorganized_premise = demorganize_data(inference[0]);
    auto conclusion = inference[2];
    return validate_inference(demorganized_premise, conclusion, validation_failure_log);
}

// void BeliefSystem::derive_beliefs_2__impl (sept::Data const &demorganized_premise, sept::Data const &conclusion) {
//
// }

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
