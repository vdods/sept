// 2021.05.15 - Victor Dods

#pragma once

#include "sept/Data.hpp"
#include "sept/SymbolTable.hpp"
#include "sept/TupleTerm.hpp"
#include "trit.hpp"
#include <unordered_set>

using BeliefSet = std::unordered_set<sept::Data>;
// using InferenceSet = std::unordered_set<sept::Data>;

void add_belief_to (BeliefSet &belief_set, sept::Data const &belief, BeliefSet const *existing_belief_set = nullptr);

// TODO: Could use PartiallyOrderedSet_t containing types, where beliefs are stored
// in the set as FormalTypeOf(belief).
class BeliefSystem {
public:

    BeliefSystem () { }

    // Attempts to evaluate the given predicate as true or false against this BeliefSystem.
    // TODO: Implement some limit on the number of search steps.
    Trit evaluate_predicate (sept::Data const &predicate) const;

    // Derives beliefs using pattern matching against a rule of inference "schema" (meaning a
    // rule of inference potentially with free variables which are to be replaced with matching values).
    void derive_beliefs (BeliefSet &new_belief_set, sept::Data const &inference, bool also_derive_using_contrapositive = true);
    inline void derive_beliefs_and_add (sept::Data const &inference, bool also_derive_using_contrapositive = true) {
        BeliefSet new_belief_set;
        derive_beliefs(new_belief_set, inference, also_derive_using_contrapositive);
        // Add the new beliefs to the BeliefSystem's belief set.
        for (auto &new_belief : new_belief_set) {
            m_belief_set.emplace(std::move(new_belief));
        }
    }
    // Note that this will clear new_belief_set before doing anything else.
    inline void derive_beliefs_and_add (BeliefSet &new_belief_set, sept::Data const &inference, bool also_derive_using_contrapositive = true) {
        new_belief_set.clear();
        derive_beliefs(new_belief_set, inference, also_derive_using_contrapositive);
        // Add the new beliefs to the BeliefSystem's belief set.
        for (auto &new_belief : new_belief_set) {
            m_belief_set.emplace(std::move(new_belief));
        }
    }

    BeliefSet const &belief_set () const { return m_belief_set; }
    bool contains_belief (sept::Data const &belief) const {
        return m_belief_set.find(belief) != m_belief_set.end();
    }
//     InferenceSet const &inference_set () const { return m_inference_set; }
//     bool contains_inference (sept::Data const &inference) const {
//         return m_inference_set.find(inference) != m_inference_set.end();
//     }

    // This processes the belief down and adds potentially several beliefs in canonical form.
    void add_belief (sept::Data const &belief);
    void remove_belief (sept::Data const &belief) {
        m_belief_set.erase(belief);
    }

//     void add_inference (sept::Data const &inference) {
//         m_inference_set.insert(inference);
//     }
//     void remove_inference (sept::Data const &inference) {
//         m_inference_set.erase(inference);
//     }

    static bool validate_inference (sept::Data const &premise, sept::Data const &conclusion, lvd::Log *validation_failure_log = nullptr);
    static bool validate_inference (sept::Data const &inference, lvd::Log *validation_failure_log = nullptr);

private:

    void derive_beliefs_impl (BeliefSet &new_belief_set, lvd::nnsp<sept::SymbolTable> const &parent_symbol_assignment, sept::TupleTerm_c const &premise_logical_literal_tuple, size_t i, sept::Data const &conclusion);

    // For now, just a flat storage of beliefs.
    BeliefSet m_belief_set;
//     // For now, have a separate set of rules of inference.  Eventually these would be incorporated
//     // into the belief set directly and the inference search would be more complex.
//     InferenceSet m_inference_set;
};

std::ostream &operator<< (std::ostream &out, BeliefSystem const &bs);
