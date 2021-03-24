// 2020.04.20 - Victor Dods

#pragma once

#include <functional>
#include <unordered_map>
#include "sept/core.hpp"
#include "sept/Data.hpp"
#include "sept/hash.hpp"
#include <sstream>
#include <typeindex>

namespace sept {

// NOTE/TODO: This should really be is_abstract_instance
bool is_instance (Data const &value, Data const &type);

namespace _is_instance {

// The is_instance function is implemented using multi-parameter polymorphism, where the
// type pair of the (value, type) arguments passed to is_instance is used to look up the
// actual function that will determine the return value.  These functions are stored in
// an unordered_map which is indexed by IsInstance_TypeKey.
struct IsInstance_TypeKey {
    std::type_index m_value_ti;
    std::type_index m_type_ti;

    bool operator == (IsInstance_TypeKey const &other) const {
        return m_value_ti == other.m_value_ti && m_type_ti == other.m_type_ti;
    }
};

} // end namespace _is_instance
} // end namespace sept

// This has to go before std::unordered_map<IsInstance_TypeKey,...>
namespace std {

// Template specialization to define std::hash<sept::IsInstance_TypeKey>.
template <>
struct hash<sept::_is_instance::IsInstance_TypeKey> {
    size_t operator () (sept::_is_instance::IsInstance_TypeKey const &k) const {
        return sept::hash(k.m_value_ti, k.m_type_ti);
    }
};

} // end namespace std

namespace sept {
namespace _is_instance {

using IsInstance_Evaluator = std::function<bool(Data const &value, Data const &type)>;

extern std::unordered_map<IsInstance_TypeKey,IsInstance_Evaluator> IS_INSTANCE__EVALUATOR_MAP;

} // end namespace _is_instance

// Instantiate a static instance of this class as a way to conveniently register an IsInstance_Evaluator upon global init.
template <typename Value_, typename Type_>
class IsInstance_EvaluatorRegistration_t {
public:

    // Default is for the evaluator to just return true (this would be used for where the membership
    // doesn't depend on any parameters, only the types).
    IsInstance_EvaluatorRegistration_t () {
        auto key = _is_instance::IsInstance_TypeKey{std::type_index(typeid(Value_)), std::type_index(typeid(Type_))};
        if (_is_instance::IS_INSTANCE__EVALUATOR_MAP.find(key) != _is_instance::IS_INSTANCE__EVALUATOR_MAP.end())
            throw std::runtime_error(LVD_FMT("is_instance type pair (" << typeid(Value_).name() << ", " << typeid(Type_).name() << ") already registered"));
        _is_instance::IS_INSTANCE__EVALUATOR_MAP[key] = nullptr;
    }
    // For when is_instance depends on the specific value.
    template <typename... Args_, typename = std::enable_if_t<(sizeof...(Args_) > 0)>>
    IsInstance_EvaluatorRegistration_t (Args_&&... args) {
        auto key = _is_instance::IsInstance_TypeKey{std::type_index(typeid(Value_)), std::type_index(typeid(Type_))};
        if (_is_instance::IS_INSTANCE__EVALUATOR_MAP.find(key) != _is_instance::IS_INSTANCE__EVALUATOR_MAP.end())
            throw std::runtime_error(LVD_FMT("is_instance type pair (" << typeid(Value_).name() << ", " << typeid(Type_).name() << ") already registered"));
        _is_instance::IS_INSTANCE__EVALUATOR_MAP[key] = _is_instance::IsInstance_Evaluator(std::forward<Args_>(args)...);
    }
};

} // end namespace sept
