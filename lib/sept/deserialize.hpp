// 2020.08.03 - Victor Dods

#pragma once

#include <istream>
#include "sept/core.hpp"
#include "sept/Data.hpp"
#include <sstream>
#include <typeindex>
#include <unordered_map>

namespace sept {

Data deserialize_data (std::istream &in);

namespace _deserialize {

using Deserialize_TypeKey = std::type_index;
using Deserialize_Evaluator = std::function<Data(Data &&type, std::istream &in)>;

extern std::unordered_map<Deserialize_TypeKey,Deserialize_Evaluator> DESERIALIZE__EVALUATOR_MAP;

} // end namespace _deserialize

// This is a way to conveniently register an Deserialize_Evaluator.
template <typename Value_>
class Deserialize_EvaluatorRegistration_t {
public:

    template <typename... Args_, typename = std::enable_if_t<(sizeof...(Args_) > 0)>>
    Deserialize_EvaluatorRegistration_t (Args_&&... args) {
        auto key = std::type_index(typeid(Value_));
        if (_deserialize::DESERIALIZE__EVALUATOR_MAP.find(key) != _deserialize::DESERIALIZE__EVALUATOR_MAP.end())
            throw std::runtime_error(LVD_FMT("deserialize type (" << typeid(Value_).name() << ") already registered"));
        _deserialize::DESERIALIZE__EVALUATOR_MAP[key] = _deserialize::Deserialize_Evaluator(std::forward<Args_>(args)...);
    }
};

} // end namespace sept
