// 2021.05.15 - Victor Dods

#include "belief.hpp"

#include "ast.hpp"
#include "common.hpp"
#include "pattern.hpp"
#include <lvd/abort.hpp>
#include <lvd/cloned.hpp>
#include <lvd/comma.hpp>
#include <lvd/fmt.hpp>

std::string const &as_string (BeliefState t) {
    static std::array<std::string,BELIEF_STATE_COUNT> const TABLE{
        "Deny",
        "Unknown",
        "Accept",
    };
    return TABLE.at(size_t(t));
}

BeliefState BeliefSystem::evaluate_predicate (sept::Data const &predicate) const {
    // Check if the predicate is a verbatim belief already.
    if (contains_belief(predicate))
        return Accept;
    // Also check the negation.  Not very efficient.  Also this won't work in general because
    // stuff isn't put into a canonical form using deMorgan's laws.
    if (contains_belief(Predicate_Not(Not, predicate)))
        return Deny;

    // TODO: Use StaticAssociation_t
    if (false) {
        // SPLUNGE
    } else if (inhabits_data(predicate, Implication)) {
        // TODO: Extract functions
        auto premise = predicate[0];
        auto conclusion = predicate[1];
        return or__belief_state(not__belief_state(evaluate_predicate(premise)), evaluate_predicate(conclusion));
    } else if (inhabits_data(predicate, Predicate_Not)) {
        return not__belief_state(evaluate_predicate(predicate[1]));
    } else if (inhabits_data(predicate, Predicate_And)) {
        auto operand_tuple = predicate[1].move_cast<sept::TupleTerm_c>();
        BeliefState retval = Accept; // Identity element of `and` operation.
        for (auto const &operand : operand_tuple.elements()) {
            retval = and__belief_state(retval, evaluate_predicate(operand));
            // If we break away from Accept, then it could be Deny or Unknown, either of which causes an early out.
            if (retval != Accept)
                return retval;
        }
        return retval;
    } else if (inhabits_data(predicate, Predicate_Or)) {
        auto operand_tuple = predicate[1].move_cast<sept::TupleTerm_c>();
        BeliefState retval = Deny; // Identity element of `or` operation.
        for (auto const &operand : operand_tuple.elements()) {
            retval = or__belief_state(retval, evaluate_predicate(operand));
            // If we break away from Deny, then it could be Accept or Unknown, either of which causes an early out.
            if (retval != Deny)
                return retval;
        }
        return retval;
    } else if (inhabits_data(predicate, Predicate_Xor)) {
        auto operand_tuple = predicate[1].move_cast<sept::TupleTerm_c>();
        BeliefState retval = Deny; // Identity element of `or` operation.
        for (auto const &operand : operand_tuple.elements()) {
            retval = or__belief_state(retval, evaluate_predicate(operand));
            // There is no early out for xor.
        }
        return retval;
    } else {
        // Fallthrough -- no information in the belief system.
        // TODO: If predicate occurred somewhere as a subexpression of some belief, that would
        // be useful information for an associative search.
        return Unknown;
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
        case Accept:
            add_belief(conclusion);
            break;
        case Unknown:
            // TODO: If the premise is Predicate_Not/And/Or/Xor, then the unknown bit
            // could be added to some "wondering about" predicate set.
            break;
        case Deny:
            // Nothing to be inferred.
            break;
    }
    // Evaluate the contrapositive.
    switch (evaluate_predicate(conclusion)) {
        case Accept:
            // Nothing to be inferred.
            break;
        case Unknown:
            // TODO: If the premise is Predicate_Not/And/Or/Xor, then the unknown bit
            // could be added to some "wondering about" predicate set.
            break;
        case Deny:
            add_belief(Predicate_Not(Not, premise));
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
        // unbound FreeVar in the conclusion)
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
        auto contrapositive = Implication(Predicate_Not(Not, conclusion), Implies, Predicate_Not(Not, premise));
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
