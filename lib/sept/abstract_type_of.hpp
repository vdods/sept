// 2020.04.24 - Victor Dods

#pragma once

#include "sept/core.hpp"
#include "sept/Data.hpp"
#include <sstream>
#include <typeindex>
#include <unordered_map>

namespace sept {

Data abstract_type_of_data (Data const &value);

namespace _abstract_type_of {

using AbstractTypeOf_TypeKey = std::type_index;
using AbstractTypeOf_Evaluator = std::function<Data(Data const &value)>;

extern std::unordered_map<AbstractTypeOf_TypeKey,AbstractTypeOf_Evaluator> ABSTRACT_TYPE_OF__EVALUATOR_MAP;

} // end namespace _abstract_type_of

// This is a way to conveniently register an AbstractTypeOf_Evaluator.
template <typename Value_>
class AbstractTypeOf_EvaluatorRegistration_t {
public:

    template <typename... Args_, typename = std::enable_if_t<(sizeof...(Args_) > 0)>>
    AbstractTypeOf_EvaluatorRegistration_t (Args_&&... args) {
        auto key = std::type_index(typeid(Value_));
        if (_abstract_type_of::ABSTRACT_TYPE_OF__EVALUATOR_MAP.find(key) != _abstract_type_of::ABSTRACT_TYPE_OF__EVALUATOR_MAP.end())
            throw std::runtime_error(LVD_FMT("abstract_type_of type (" << typeid(Value_).name() << ") already registered"));
        _abstract_type_of::ABSTRACT_TYPE_OF__EVALUATOR_MAP[key] = _abstract_type_of::AbstractTypeOf_Evaluator(std::forward<Args_>(args)...);
    }
};

} // end namespace sept
