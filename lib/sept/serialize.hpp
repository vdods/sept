// 2020.08.02 - Victor Dods

#pragma once

#include "sept/core.hpp"
#include "sept/Data.hpp"
#include <sstream>
#include <typeindex>
#include <unordered_map>

namespace sept {

void serialize_data (Data const &value, std::ostream &out);

// This causes anything but an explicit Data to be passed into serialize_data.
template <typename T_>
void serialize_data (T_ const &, std::ostream &) = delete;

namespace _serialize {

using Serialize_TypeKey = std::type_index;
using Serialize_Evaluator = std::function<void(Data const &value, std::ostream &out)>;

extern std::unordered_map<Serialize_TypeKey,Serialize_Evaluator> SERIALIZE__EVALUATOR_MAP;

} // end namespace _serialize

// This is a way to conveniently register an Serialize_Evaluator.
template <typename Value_>
class Serialize_EvaluatorRegistration_t {
public:

    template <typename... Args_, typename = std::enable_if_t<(sizeof...(Args_) > 0)>>
    Serialize_EvaluatorRegistration_t (Args_&&... args) {
        auto key = std::type_index(typeid(Value_));
        if (_serialize::SERIALIZE__EVALUATOR_MAP.find(key) != _serialize::SERIALIZE__EVALUATOR_MAP.end())
            throw std::runtime_error(LVD_FMT("serialize type (" << typeid(Value_).name() << ") already registered"));
        _serialize::SERIALIZE__EVALUATOR_MAP[key] = _serialize::Serialize_Evaluator(std::forward<Args_>(args)...);
    }
};

} // end namespace sept
