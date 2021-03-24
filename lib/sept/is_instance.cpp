// 2020.04.24 - Victor Dods

#include "sept/is_instance.hpp"

#include "sept/ArrayTerm.hpp"
#include "sept/ArrayType.hpp"
#include "sept/ctl/ClearOutput.hpp"
#include "sept/ctl/EndOfFile.hpp"
#include "sept/ctl/Output.hpp"
#include "sept/ctl/RequestSyncInput.hpp"
#include "sept/NPTerm.hpp"
#include "sept/NPType.hpp"
#include "sept/OrderedMapTerm.hpp"
#include "sept/OrderedMapType.hpp"
#include <tuple>

namespace sept {

bool is_instance (Data const &value, Data const &type) {
    // Everything is a Term
    if (type.type() == typeid(Term_c))
        return true;

    // TODO: Maybe check abstract_type_of(value) against type here, so it doesn't have to be explicitly specified below

    auto key = _is_instance::IsInstance_TypeKey{std::type_index(value.type()), std::type_index(type.type())};
    auto it = _is_instance::IS_INSTANCE__EVALUATOR_MAP.find(key);
    // If none is registered for this type pair, return false.
    if (it == _is_instance::IS_INSTANCE__EVALUATOR_MAP.end())
        return false;
    // Otherwise call the evaluator.
    auto const &evaluator = it->second;
    // nullptr is taken by convention to return true.
    return evaluator != nullptr ? evaluator(value, type) : true;
}

namespace _is_instance {

std::unordered_map<IsInstance_TypeKey,IsInstance_Evaluator> IS_INSTANCE__EVALUATOR_MAP;

//
// These globals are what register the IsInstance_Evaluator functions.
//

#define SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(Value,Type) IsInstance_EvaluatorRegistration_t<Value,Type> IsInstance_Evaluator_##Value##_##Type;
#define SEPT_REGISTER_IS_INSTANCE_EVALUATOR(Value,Type,EvaluatorExpression) IsInstance_EvaluatorRegistration_t<Value,Type> IsInstance_Evaluator_##Value##_##Type{ \
    [](Data const &value_, Data const &type_){ \
        auto const &value = value_.cast<Value const &>(); \
        auto const &type = type_.cast<Type const &>(); \
        std::ignore = value; \
        std::ignore = type; \
        EvaluatorExpression \
    } \
};

SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(Void_c, VoidType_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(Void_c, NonParametricTerm_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(Void_c, NonType_c)

SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(VoidType_c, NonParametricTerm_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(VoidType_c, Type_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(VoidType_c, NonParametricType_c)

SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(True_c, TrueType_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(False_c, FalseType_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(True_c, Bool_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(False_c, Bool_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(BoolTerm_c, Bool_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(bool, Bool_c) // bool and BoolTerm_c are equivalent
SEPT_REGISTER_IS_INSTANCE_EVALUATOR(BoolTerm_c, TrueType_c, return value == true; )
SEPT_REGISTER_IS_INSTANCE_EVALUATOR(BoolTerm_c, FalseType_c, return value == false; )
SEPT_REGISTER_IS_INSTANCE_EVALUATOR(bool, TrueType_c, return value == true; )
SEPT_REGISTER_IS_INSTANCE_EVALUATOR(bool, FalseType_c, return value == false; )
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(Bool_c, BoolType_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(TrueType_c, BoolType_c) // TrueType is a subtype of Bool
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(FalseType_c, BoolType_c) // FalseType is a subtype of Bool

SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(int8_t, Sint8_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(int16_t, Sint16_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(int32_t, Sint32_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(int64_t, Sint64_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(Sint8_c, Sint8Type_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(Sint16_c, Sint16Type_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(Sint32_c, Sint32Type_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(Sint64_c, Sint64Type_c)

SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(uint8_t, Uint8_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(uint16_t, Uint16_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(uint32_t, Uint32_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(uint64_t, Uint64_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(Uint8_c, Uint8Type_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(Uint16_c, Uint16Type_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(Uint32_c, Uint32Type_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(Uint64_c, Uint64Type_c)

SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(float, Float32_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(double, Float64_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(Float32_c, Float32Type_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(Float64_c, Float64Type_c)

SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(Term_c, Type_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(NonParametricTerm_c, Type_c)
// SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(ParametricTerm_c, Type_c)

SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(Type_c, Type_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(NonType_c, Type_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(NonParametricType_c, Type_c)
// SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(ParametricType_c, Type_c)

SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(ArrayES_c, Type_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(ArrayE_c, Type_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(ArrayS_c, Type_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(Array_c, Type_c)

SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(ArrayES_c, ArrayType_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(ArrayE_c, ArrayType_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(ArrayS_c, ArrayType_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(Array_c, ArrayType_c)

SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(ArrayESTerm_c, ArrayES_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(ArrayETerm_c, ArrayE_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(ArraySTerm_c, ArrayS_c)

SEPT_REGISTER_IS_INSTANCE_EVALUATOR(ArrayTerm_c, ArrayESTerm_c, return type.constraint_is_satisfied(value.elements()); )
SEPT_REGISTER_IS_INSTANCE_EVALUATOR(ArrayTerm_c, ArrayETerm_c, return type.constraint_is_satisfied(value.elements()); )
SEPT_REGISTER_IS_INSTANCE_EVALUATOR(ArrayTerm_c, ArraySTerm_c, return type.constraint_is_satisfied(value.elements()); )
SEPT_REGISTER_IS_INSTANCE_EVALUATOR(ArrayTerm_c, Array_c, return type.constraint_is_satisfied(value.elements()); )


SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(OrderedMapDC_c, Type_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(OrderedMapD_c, Type_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(OrderedMapC_c, Type_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(OrderedMap_c, Type_c)

SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(OrderedMapDC_c, OrderedMapType_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(OrderedMapD_c, OrderedMapType_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(OrderedMapC_c, OrderedMapType_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(OrderedMap_c, OrderedMapType_c)

SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(OrderedMapDCTerm_c, OrderedMapDC_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(OrderedMapDTerm_c, OrderedMapD_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(OrderedMapCTerm_c, OrderedMapC_c)

SEPT_REGISTER_IS_INSTANCE_EVALUATOR(OrderedMapTerm_c, OrderedMapDCTerm_c, return type.constraint_is_satisfied(value.pairs()); )
SEPT_REGISTER_IS_INSTANCE_EVALUATOR(OrderedMapTerm_c, OrderedMapDTerm_c, return type.constraint_is_satisfied(value.pairs()); )
SEPT_REGISTER_IS_INSTANCE_EVALUATOR(OrderedMapTerm_c, OrderedMapCTerm_c, return type.constraint_is_satisfied(value.pairs()); )
SEPT_REGISTER_IS_INSTANCE_EVALUATOR(OrderedMapTerm_c, OrderedMap_c, return type.constraint_is_satisfied(value.pairs()); )

// This is necessary because using :: interferes with the macro.
using namespace ctl;

SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(Output_c, OutputType_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(OutputTerm_c, Output_c)

SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(ClearOutput_c, ClearOutputType_c)

SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(EndOfFile_c, EndOfFileType_c)

SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(RequestSyncInput_c, RequestSyncInputType_c)
SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL(RequestSyncInputTerm_c, RequestSyncInput_c)

#undef SEPT_REGISTER_IS_INSTANCE_EVALUATOR_TRIVIAL
#undef SEPT_REGISTER_IS_INSTANCE_EVALUATOR

} // end namespace _is_instance
} // end namespace sept
