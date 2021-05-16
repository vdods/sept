// 2021.05.15 - Victor Dods

#include "sept/Data.hpp"
#include <unordered_set>

using BeliefStateRepr = uint8_t;
enum class BeliefState : BeliefStateRepr {
    DENY = 0,
    UNKNOWN,
    ACCEPT,

    __LOWEST__ = DENY,
    __HIGHEST__ = ACCEPT,
};

inline BeliefState constexpr Deny = BeliefState::DENY;
inline BeliefState constexpr Unknown = BeliefState::UNKNOWN;
inline BeliefState constexpr Accept = BeliefState::ACCEPT;

size_t constexpr BELIEF_STATE_COUNT = size_t(BeliefState::__HIGHEST__)+1 - size_t(BeliefState::__LOWEST__);

std::string const &as_string (BeliefState t);

inline BeliefState not__belief_state (BeliefState t) {
    switch (t) {
        case Deny: return Accept;
        case Unknown: return Unknown;
        case Accept: return Deny;
        default: LVD_ABORT("Invalid BeliefState");
    }
}

inline BeliefState and__belief_state (BeliefState lhs, BeliefState rhs) {
    if (lhs == Unknown || rhs == Unknown)
        return Unknown;
    else
        return (lhs == Accept && rhs == Accept) ? Accept : Deny;
}

inline BeliefState or__belief_state (BeliefState lhs, BeliefState rhs) {
    if (lhs == Unknown || rhs == Unknown)
        return Unknown;
    else
        return (lhs == Accept || rhs == Accept) ? Accept : Deny;
}

inline BeliefState xor__belief_state (BeliefState lhs, BeliefState rhs) {
    if (lhs == Unknown || rhs == Unknown)
        return Unknown;
    else
        return ((lhs == Accept) != (rhs == Accept)) ? Accept : Deny;
}

inline std::ostream &operator<< (std::ostream &out, BeliefState const &t) {
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
    BeliefState evaluate_predicate (sept::Data const &predicate) const;

    // Attempts to derive new beliefs using a rule of inference.
    void derive_beliefs (sept::Data const &inference);

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
