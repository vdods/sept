// 2021.05.15 - Victor Dods

#pragma once

#include "sept/Data.hpp"
#include "sept/SymbolTable.hpp"
#include "sept/TupleTerm.hpp"
#include "trit.hpp"
#include <unordered_set>

// TODO: Could use PartiallyOrderedSet_t containing types, where beliefs are stored
// in the set as FormalTypeOf(belief).
class BeliefSystem {
public:

    using BeliefSet = std::unordered_set<sept::Data>;
//     using InferenceSet = std::unordered_set<sept::Data>;

    BeliefSystem () { }

    // Attempts to evaluate the given predicate as true or false against this BeliefSystem.
    // TODO: Implement some limit on the number of search steps.
    Trit evaluate_predicate (sept::Data const &predicate) const;

    // Attempts to derive new beliefs using a rule of inference.
    void derive_beliefs (sept::Data const &inference);
    // Here's the "real" version of derive_beliefs, which uses pattern matching against a
    // rule of inference "schema" (meaning a rule of inference with free variables which
    // are to be replaced with matching values).
    void derive_beliefs_2 (sept::Data const &inference, bool also_derive_using_contrapositive = true);

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

    static bool validate_inference (sept::Data const &demorganized_premise, sept::Data const &conclusion, lvd::Log *validation_failure_log = nullptr);
    static bool validate_inference (sept::Data const &inference, lvd::Log *validation_failure_log = nullptr);

    static bool validate_inference_2 (sept::Data const &premise, sept::Data const &conclusion, lvd::Log *validation_failure_log = nullptr);
    static bool validate_inference_2 (sept::Data const &inference, lvd::Log *validation_failure_log = nullptr);

private:

    void derive_beliefs_2_impl (lvd::nnsp<sept::SymbolTable> const &parent_symbol_assignment, sept::TupleTerm_c const &premise_logical_literal_tuple, size_t i, sept::Data const &conclusion);

    // For now, just a flat storage of beliefs.
    BeliefSet m_belief_set;
//     // For now, have a separate set of rules of inference.  Eventually these would be incorporated
//     // into the belief set directly and the inference search would be more complex.
//     InferenceSet m_inference_set;
};

std::ostream &operator<< (std::ostream &out, BeliefSystem const &bs);
