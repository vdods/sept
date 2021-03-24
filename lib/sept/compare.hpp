// 2020.07.16 - Victor Dods

#pragma once

#include <functional>
#include "sept/core.hpp"
#include "sept/Data.hpp"
#include "sept/hash.hpp"
#include <sstream>
#include <typeindex>
#include <unordered_map>

namespace sept {

int compare_data (Data const &lhs, Data const &rhs);

namespace _compare {

// The compare function is implemented using multi-parameter polymorphism, where the
// type pair of the (lhs, rhs) arguments passed to compare is used to look up the actual
// function that will determine the return value.  These functions are stored in
// an unordered_map which is indexed by Compare_TypeKey.
struct Compare_TypeKey {
    std::type_index m_lhs_ti;
    std::type_index m_rhs_ti;

    bool operator == (Compare_TypeKey const &other) const {
        return m_lhs_ti == other.m_lhs_ti && m_rhs_ti == other.m_rhs_ti;
    }
};

} // end namespace _compare
} // end namespace sept

// This has to go before std::unordered_map<Compare_TypeKey,...>
namespace std {

// Template specialization to define std::hash<sept::Compare_TypeKey>.
template <>
struct hash<sept::_compare::Compare_TypeKey> {
    size_t operator () (sept::_compare::Compare_TypeKey const &k) const {
        return sept::hash(k.m_lhs_ti, k.m_rhs_ti);
    }
};

} // end namespace std

namespace sept {
namespace _compare {

using Compare_Evaluator = std::function<int(Data const &lhs, Data const &rhs)>;

extern std::unordered_map<Compare_TypeKey,Compare_Evaluator> COMPARE__EVALUATOR_MAP;

} // end namespace _compare

// Instantiate a static instance of this class as a way to conveniently register an Compare_Evaluator upon global init.
template <typename LhsType_, typename RhsType_>
class Compare_EvaluatorRegistration_t {
public:

    // Default is for the evaluator to just return 0, meaning equal (this would be used for where the membership
    // doesn't depend on any parameters, only the types involved).
    Compare_EvaluatorRegistration_t () {
        auto key = _compare::Compare_TypeKey{std::type_index(typeid(LhsType_)), std::type_index(typeid(RhsType_))};
        if (_compare::COMPARE__EVALUATOR_MAP.find(key) != _compare::COMPARE__EVALUATOR_MAP.end())
            throw std::runtime_error(LVD_FMT("compare type pair (" << typeid(LhsType_).name() << ", " << typeid(RhsType_).name() << ") already registered"));
        _compare::COMPARE__EVALUATOR_MAP[key] = nullptr;
    }
    template <typename... Args_, typename = std::enable_if_t<(sizeof...(Args_) > 0)>>
    Compare_EvaluatorRegistration_t (Args_&&... args) {
        auto key = _compare::Compare_TypeKey{std::type_index(typeid(LhsType_)), std::type_index(typeid(RhsType_))};
        if (_compare::COMPARE__EVALUATOR_MAP.find(key) != _compare::COMPARE__EVALUATOR_MAP.end())
            throw std::runtime_error(LVD_FMT("compare type pair (" << typeid(LhsType_).name() << ", " << typeid(RhsType_).name() << ") already registered"));
        _compare::COMPARE__EVALUATOR_MAP[key] = _compare::Compare_Evaluator(std::forward<Args_>(args)...);
    }
};

} // end namespace sept
