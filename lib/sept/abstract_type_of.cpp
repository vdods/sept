// 2020.04.24 - Victor Dods

#include "sept/abstract_type_of.hpp"

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
#include <sstream>

namespace sept {

Data abstract_type_of_data (Data const &value) {
    auto it = _abstract_type_of::ABSTRACT_TYPE_OF__EVALUATOR_MAP.find(std::type_index(value.type()));
    if (it == _abstract_type_of::ABSTRACT_TYPE_OF__EVALUATOR_MAP.end())
        throw std::runtime_error(LVD_FMT("abstract_type_of_data not defined for " << value.type()));
//         return Term; // Default fallback; everything is a term.  TODO: Return "FormalAbstractTypeOf" construction.

    auto const &evaluator = it->second;
    return evaluator(value);
}

namespace _abstract_type_of {

std::unordered_map<AbstractTypeOf_TypeKey,AbstractTypeOf_Evaluator> ABSTRACT_TYPE_OF__EVALUATOR_MAP;

//
// These globals are what register the AbstractTypeOf_Evaluator functions.
//

#define SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Value) AbstractTypeOf_EvaluatorRegistration_t<Value> Value##_##Type{ \
    [](Data const &value_) -> Data { \
        auto const &value = value_.cast<Value const &>(); \
        return abstract_type_of(value); \
    } \
};
#define SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(Value,EvaluatorExpression) AbstractTypeOf_EvaluatorRegistration_t<Value> Value##_##Type{ \
    [](Data const &value_) -> Data { \
        auto const &value = value_.cast<Value const &>(); \
        std::ignore = value; \
        EvaluatorExpression \
    } \
};

//
// Use the specific-C++-typed overloads of abstract_type_of to define the Data-typed abstract_type_of.
//
// TODO: Maybe use FormalTypeOf(T) construction here?

SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Term_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(NonParametricTerm_c)
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(ParametricTerm_c)

SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Type_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(NonType_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(NonParametricType_c)
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(ParametricType_c)

SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Void_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(True_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(False_c)

SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(VoidType_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(TrueType_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(FalseType_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(EmptyType_c)

SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Bool_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Sint8_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Sint16_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Sint32_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Sint64_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Uint8_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Uint16_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Uint32_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Uint64_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Float32_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Float64_c)

SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(bool)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(int8_t)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(int16_t)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(int32_t)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(int64_t)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(uint8_t)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(uint16_t)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(uint32_t)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(uint64_t)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(float)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(double)

SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(BoolType_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Sint8Type_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Sint16Type_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Sint32Type_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Sint64Type_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Uint8Type_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Uint16Type_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Uint32Type_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Uint64Type_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Float32Type_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Float64Type_c)

SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(ArrayES_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(ArrayE_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(ArrayS_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Array_c)

SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(ArrayType_c)

SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(ArrayESTerm_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(ArrayETerm_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(ArraySTerm_c)

SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(ArrayTerm_c)

SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(OrderedMapDC_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(OrderedMapD_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(OrderedMapC_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(OrderedMap_c)

SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(OrderedMapType_c)

SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(OrderedMapDCTerm_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(OrderedMapDTerm_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(OrderedMapCTerm_c)

SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(OrderedMapTerm_c)

// This is necessary because using :: interferes with the macro.
using namespace ctl;

SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(OutputType_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(Output_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(OutputTerm_c)

SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(ClearOutputType_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(ClearOutput_c)

SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(EndOfFileType_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(EndOfFile_c)

SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(RequestSyncInputType_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(RequestSyncInput_c)
SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR_DEFAULT(RequestSyncInputTerm_c)

// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(Term_c, return NonParametricType; )
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(NonParametricTerm_c, return NonParametricType; )
// // SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(ParametricTerm_c, return NonParametricType; )
//
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(Type_c, return NonParametricType; )
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(NonType_c, return NonParametricType; )
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(NonParametricType_c, return NonParametricType; )
// // SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(ParametricType_c, return NonParametricType; )
//
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(Void_c, return VoidType; )
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(True_c, return TrueType; )
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(False_c, return FalseType; )
//
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(VoidType_c, return NonParametricType; ) // TODO: Could be PODType?
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(TrueType_c, return NonParametricType; ) // TODO: Should be PODType?
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(FalseType_c, return NonParametricType; ) // TODO: Should be PODType?
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(EmptyType_c, return NonParametricType; )
//
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(Bool_c, return BoolType; )
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(Sint8_c, return Sint8Type; )
//
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(BoolType_c, return NonParametricType; ) // TODO: Should be PODType
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(Sint8Type_c, return NonParametricType; ) // TODO: Should be PODType
//
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(ArrayES_c, return ArrayType; )
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(ArrayE_c, return ArrayType; )
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(ArrayS_c, return ArrayType; )
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(Array_c, return ArrayType; )
//
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(ArrayType_c, return NonParametricType; )
//
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(ArrayESTerm_c, return ArrayES; )
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(ArrayETerm_c, return ArrayE; )
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(ArraySTerm_c, return ArrayS; )
//
// SEPT_REGISTER_ABSTRACT_TYPE_OF_EVALUATOR(ArrayTerm_c, return value.constraint().array_type(); )
//
} // end namespace _abstract_type_of
} // end namespace sept
