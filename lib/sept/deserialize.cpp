// 2020.08.03 - Victor Dods

#include "sept/deserialize.hpp"

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

// This assumes that in.peek() will return SerializedTopLevelCode::NON_PARAMETRIC_TERM.
// This function is essentially a switch statement on all possible NPTerm values.
Data deserialize_NonParametricTerm (std::istream &in) {
    auto stlc = SerializedTopLevelCode(in.get());
    assert(stlc == SerializedTopLevelCode::NON_PARAMETRIC_TERM && "pre-condition for this function was not satisfied");
    std::ignore = stlc;

    // TODO: Handle error cases
    auto np_term_enum = NPTerm(in.get());
    if (in.eof()) {
        return ctl::EndOfFile;
    }
    switch (np_term_enum) {
        case NPTerm::TERM: return Term;
        case NPTerm::NON_PARAMETRIC_TERM: return NonParametricTerm;
//         case NPTerm::PARAMETRIC_TERM: return ParametricTerm;

        case NPTerm::TYPE: return Type;
        case NPTerm::NON_TYPE: return NonType;
        case NPTerm::NON_PARAMETRIC_TYPE: return NonParametricType;
//         case NPTerm::PARAMETRIC_TYPE: return ParametricType;

        case NPTerm::VOID: return Void;
        case NPTerm::TRUE: return True;
        case NPTerm::FALSE: return False;

        case NPTerm::VOID_TYPE: return VoidType;
        case NPTerm::TRUE_TYPE: return TrueType;
        case NPTerm::FALSE_TYPE: return FalseType;
        case NPTerm::EMPTY_TYPE: return EmptyType;

        case NPTerm::BOOL: return Bool;
        case NPTerm::SINT8: return Sint8;
        case NPTerm::SINT16: return Sint16;
        case NPTerm::SINT32: return Sint32;
        case NPTerm::SINT64: return Sint64;
        case NPTerm::UINT8: return Uint8;
        case NPTerm::UINT16: return Uint16;
        case NPTerm::UINT32: return Uint32;
        case NPTerm::UINT64: return Uint64;
        case NPTerm::FLOAT32: return Float32;
        case NPTerm::FLOAT64: return Float64;

        case NPTerm::BOOL_TYPE: return BoolType;
        case NPTerm::SINT8_TYPE: return Sint8Type;
        case NPTerm::SINT16_TYPE: return Sint16Type;
        case NPTerm::SINT32_TYPE: return Sint32Type;
        case NPTerm::SINT64_TYPE: return Sint64Type;
        case NPTerm::UINT8_TYPE: return Uint8Type;
        case NPTerm::UINT16_TYPE: return Uint16Type;
        case NPTerm::UINT32_TYPE: return Uint32Type;
        case NPTerm::UINT64_TYPE: return Uint64Type;
        case NPTerm::FLOAT32_TYPE: return Float32Type;
        case NPTerm::FLOAT64_TYPE: return Float64Type;

        case NPTerm::ARRAY_TYPE: return ArrayType;
        case NPTerm::ARRAY_ES: return ArrayES;
        case NPTerm::ARRAY_E: return ArrayE;
        case NPTerm::ARRAY_S: return ArrayS;
        case NPTerm::ARRAY: return Array;

        case NPTerm::ORDERED_MAP_TYPE: return OrderedMapType;
        case NPTerm::ORDERED_MAP_DC: return OrderedMapDC;
        case NPTerm::ORDERED_MAP_D: return OrderedMapD;
        case NPTerm::ORDERED_MAP_C: return OrderedMapC;
        case NPTerm::ORDERED_MAP: return OrderedMap;

        case NPTerm::OUTPUT_TYPE: return ctl::OutputType;
        case NPTerm::OUTPUT: return ctl::Output;
        case NPTerm::CLEAR_OUTPUT_TYPE: return ctl::ClearOutputType;
        case NPTerm::CLEAR_OUTPUT: return ctl::ClearOutput;
        case NPTerm::END_OF_FILE_TYPE: return ctl::EndOfFileType;
        case NPTerm::END_OF_FILE: return ctl::EndOfFile;
        case NPTerm::REQUEST_SYNC_INPUT_TYPE: return ctl::RequestSyncInputType;
        case NPTerm::REQUEST_SYNC_INPUT: return ctl::RequestSyncInput;

        default: throw std::runtime_error(LVD_FMT("deserialize_NonParametricTerm; invalid NPTerm " << int(np_term_enum)));
    }
}

// This assumes that in.peek() will return SerializedTopLevelCode::PARAMETRIC_TERM.
Data deserialize_ParametricTerm (std::istream &in) {
    auto stlc = SerializedTopLevelCode(in.get());
    assert(!in.eof());
    assert(stlc == SerializedTopLevelCode::PARAMETRIC_TERM && "pre-condition for this function was not satisfied");
    std::ignore = stlc;

    Data abstract_type = deserialize_data(in);
    auto it = _deserialize::DESERIALIZE__EVALUATOR_MAP.find(std::type_index(abstract_type.type()));
    if (it == _deserialize::DESERIALIZE__EVALUATOR_MAP.end())
        throw std::runtime_error(LVD_FMT("deserialize_ParametricTerm not defined for " << abstract_type.type()));

    auto const &evaluator = it->second;
    return evaluator(std::move(abstract_type), in);
}

Data deserialize_data (std::istream &in) {
    // TODO: Handle error cases
    auto stlc = SerializedTopLevelCode(in.peek());
    if (in.eof()) {
        return ctl::EndOfFile;
    }
    switch (stlc) {
        case SerializedTopLevelCode::NON_PARAMETRIC_TERM: return deserialize_NonParametricTerm(in);
        case SerializedTopLevelCode::PARAMETRIC_TERM: return deserialize_ParametricTerm(in);
        default: throw std::runtime_error(LVD_FMT("invalid SerializedTopLevelCode " << int(stlc)));
    }
}

namespace _deserialize {

std::unordered_map<Deserialize_TypeKey,Deserialize_Evaluator> DESERIALIZE__EVALUATOR_MAP;

//
// These globals are what register the Deserialize_Evaluator functions.
//

#define SEPT_REGISTER_DESERIALIZE_EVALUATOR(Type,EvaluatorExpression) Deserialize_EvaluatorRegistration_t<Type> Type##_{ \
    [](Data &&abstract_type, std::istream &in) -> Data { \
        EvaluatorExpression \
    } \
};
// TODO: Incorporate type
#define SEPT_REGISTER_DESERIALIZE_EVALUATOR_POD(Value,Type) Deserialize_EvaluatorRegistration_t<Type> Type##_{ \
    [](Data &&abstract_type, std::istream &in) -> Data { \
        return SerializationForPOD<Value>::deserialize_value(in); \
    } \
};

SEPT_REGISTER_DESERIALIZE_EVALUATOR_POD(bool, Bool_c)
// SEPT_REGISTER_DESERIALIZE_EVALUATOR_POD(BoolTerm_c, Bool_c) // Can only pick one of bool or BoolTerm_c, where the type is Bool_c.
SEPT_REGISTER_DESERIALIZE_EVALUATOR_POD(int8_t, Sint8_c)
SEPT_REGISTER_DESERIALIZE_EVALUATOR_POD(int16_t, Sint16_c)
SEPT_REGISTER_DESERIALIZE_EVALUATOR_POD(int32_t, Sint32_c)
SEPT_REGISTER_DESERIALIZE_EVALUATOR_POD(int64_t, Sint64_c)
SEPT_REGISTER_DESERIALIZE_EVALUATOR_POD(uint8_t, Uint8_c)
SEPT_REGISTER_DESERIALIZE_EVALUATOR_POD(uint16_t, Uint16_c)
SEPT_REGISTER_DESERIALIZE_EVALUATOR_POD(uint32_t, Uint32_c)
SEPT_REGISTER_DESERIALIZE_EVALUATOR_POD(uint64_t, Uint64_c)
SEPT_REGISTER_DESERIALIZE_EVALUATOR_POD(float, Float32_c)
SEPT_REGISTER_DESERIALIZE_EVALUATOR_POD(double, Float64_c)

SEPT_REGISTER_DESERIALIZE_EVALUATOR(ArrayES_c, return deserialize_value_ArrayESTerm(std::move(abstract_type), in);)
SEPT_REGISTER_DESERIALIZE_EVALUATOR(ArrayE_c, return deserialize_value_ArrayETerm(std::move(abstract_type), in);)
SEPT_REGISTER_DESERIALIZE_EVALUATOR(ArrayS_c, return deserialize_value_ArraySTerm(std::move(abstract_type), in);)

SEPT_REGISTER_DESERIALIZE_EVALUATOR(ArrayESTerm_c, return deserialize_value_ArrayTerm(std::move(abstract_type), in);)
SEPT_REGISTER_DESERIALIZE_EVALUATOR(ArrayETerm_c, return deserialize_value_ArrayTerm(std::move(abstract_type), in);)
SEPT_REGISTER_DESERIALIZE_EVALUATOR(ArraySTerm_c, return deserialize_value_ArrayTerm(std::move(abstract_type), in);)
SEPT_REGISTER_DESERIALIZE_EVALUATOR(Array_c, return deserialize_value_ArrayTerm(std::move(abstract_type), in);)

SEPT_REGISTER_DESERIALIZE_EVALUATOR(OrderedMapDC_c, return deserialize_value_OrderedMapDCTerm(std::move(abstract_type), in);)
SEPT_REGISTER_DESERIALIZE_EVALUATOR(OrderedMapD_c, return deserialize_value_OrderedMapDTerm(std::move(abstract_type), in);)
SEPT_REGISTER_DESERIALIZE_EVALUATOR(OrderedMapC_c, return deserialize_value_OrderedMapCTerm(std::move(abstract_type), in);)

SEPT_REGISTER_DESERIALIZE_EVALUATOR(OrderedMapDCTerm_c, return deserialize_value_OrderedMapTerm(std::move(abstract_type), in);)
SEPT_REGISTER_DESERIALIZE_EVALUATOR(OrderedMapDTerm_c, return deserialize_value_OrderedMapTerm(std::move(abstract_type), in);)
SEPT_REGISTER_DESERIALIZE_EVALUATOR(OrderedMapCTerm_c, return deserialize_value_OrderedMapTerm(std::move(abstract_type), in);)
SEPT_REGISTER_DESERIALIZE_EVALUATOR(OrderedMap_c, return deserialize_value_OrderedMapTerm(std::move(abstract_type), in);)

// This is necessary because using :: interferes with the macro.
using namespace ctl;

SEPT_REGISTER_DESERIALIZE_EVALUATOR(Output_c, return deserialize_value_OutputTerm(std::move(abstract_type), in);)

SEPT_REGISTER_DESERIALIZE_EVALUATOR(RequestSyncInput_c, return deserialize_value_RequestSyncInputTerm(std::move(abstract_type), in);)

} // end namespace _deserialize
} // end namespace sept
