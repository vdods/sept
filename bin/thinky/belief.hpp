// 2021.05.15 - Victor Dods

#include "sept/Data.hpp"
#include <unordered_set>

// IMO "Trit" sounds dumb, but it is the most reasonable name (analogous to "bit").
// Reference: https://en.wikipedia.org/wiki/Three-valued_logic
using Trit_CType = int8_t;
enum class Trit : Trit_CType {
    NOPE = -1,
    KWATZ = 0,
    YEP = 1,

    __LOWEST__ = NOPE,
    __HIGHEST__ = YEP,
};

inline Trit constexpr Nope = Trit::NOPE;
inline Trit constexpr Kwatz = Trit::KWATZ;
inline Trit constexpr Yep = Trit::YEP;

size_t constexpr BELIEF_STATE_COUNT = size_t(Trit::__HIGHEST__)+1 - size_t(Trit::__LOWEST__);

std::string const &as_string (Trit t);

//
// The following logical operations are named instead of using operator overloads because it would be too
// easy to read an expression like "a && (b || c)" as an ordinary boolean expression.
//

inline Trit not__trit (Trit t) {
    return Trit(-Trit_CType(t));
}

inline Trit and__trit (Trit lhs, Trit rhs) {
    return Trit(std::min(Trit_CType(lhs), Trit_CType(rhs)));
}

inline Trit or__trit (Trit lhs, Trit rhs) {
    return Trit(std::max(Trit_CType(lhs), Trit_CType(rhs)));
}

inline Trit xor__trit (Trit lhs, Trit rhs) {
    return Trit(-(Trit_CType(lhs) * Trit_CType(rhs)));
}

inline std::ostream &operator<< (std::ostream &out, Trit const &t) {
    return out << as_string(t);
}

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

private:

    // For now, just a flat storage of beliefs.
    BeliefSet m_belief_set;
//     // For now, have a separate set of rules of inference.  Eventually these would be incorporated
//     // into the belief set directly and the inference search would be more complex.
//     InferenceSet m_inference_set;
};

std::ostream &operator<< (std::ostream &out, BeliefSystem const &bs);
