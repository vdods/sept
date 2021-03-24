// 2020.07.16 - Victor Dods

#include "sept/compare.hpp"

#include "sept/ArrayTerm.hpp"
#include "sept/ArrayType.hpp"
#include "sept/NPTerm.hpp"
#include "sept/NPType.hpp"
#include "sept/OrderedMapTerm.hpp"
#include "sept/OrderedMapType.hpp"
#include <tuple>

namespace sept {

int compare_data (Data const &lhs, Data const &rhs) {
    auto key = _compare::Compare_TypeKey{std::type_index(lhs.type()), std::type_index(rhs.type())};
    auto it = _compare::COMPARE__EVALUATOR_MAP.find(key);
    // If none is registered for this type pair, throw exception.
    if (it == _compare::COMPARE__EVALUATOR_MAP.end()) {
        // TODO: Order the types based on name; this implies that there has to be a "name" function defined for all types
        // i.e. produce the string representation of the type's name.
        throw std::runtime_error(LVD_FMT("no compare evaluator registered for LHS type " << lhs.type() << " and RHS type " << rhs.type()));
    }
    // Otherwise call the evaluator.
    auto const &evaluator = it->second;
    // nullptr is taken by convention to return 0, meaning equal.
    return evaluator != nullptr ? evaluator(lhs, rhs) : 0;
}

namespace _compare {

std::unordered_map<Compare_TypeKey,Compare_Evaluator> COMPARE__EVALUATOR_MAP;

//
// These globals are what register the Compare_Evaluator functions.
//

#define SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Type) Compare_EvaluatorRegistration_t<Type,Type> Compare_Evaluator_Singleton_##Type;
// This just calls the compare function on the type-cast lhs and rhs values.
#define SEPT_REGISTER_COMPARE_EVALUATOR(LhsType,RhsType) Compare_EvaluatorRegistration_t<LhsType,RhsType> Compare_Evaluator_##LhsType##_##RhsType{ \
    [](Data const &lhs_, Data const &rhs_){ \
        auto const &lhs = lhs_.cast<LhsType const &>(); \
        auto const &rhs = rhs_.cast<RhsType const &>(); \
        std::ignore = lhs; \
        std::ignore = rhs; \
        return compare(lhs, rhs); \
    } \
};

//
// All NonParametricTerms
//

SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Term_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(NonParametricTerm_c)
// SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(ParametricTerm_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Type_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(NonType_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(NonParametricType_c)
// SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(ParametricType_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Void_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(True_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(False_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(VoidType_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(TrueType_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(FalseType_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(EmptyType_c)
SEPT_REGISTER_COMPARE_EVALUATOR(True_c, BoolTerm_c)
SEPT_REGISTER_COMPARE_EVALUATOR(False_c, BoolTerm_c)
SEPT_REGISTER_COMPARE_EVALUATOR(BoolTerm_c, BoolTerm_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Bool_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Sint8_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Sint16_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Sint32_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Sint64_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Uint8_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Uint16_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Uint32_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Uint64_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Float32_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Float64_c)
SEPT_REGISTER_COMPARE_EVALUATOR(bool, bool)
SEPT_REGISTER_COMPARE_EVALUATOR(int8_t, int8_t)
SEPT_REGISTER_COMPARE_EVALUATOR(int16_t, int16_t)
SEPT_REGISTER_COMPARE_EVALUATOR(int32_t, int32_t)
SEPT_REGISTER_COMPARE_EVALUATOR(int64_t, int64_t)
SEPT_REGISTER_COMPARE_EVALUATOR(uint8_t, uint8_t)
SEPT_REGISTER_COMPARE_EVALUATOR(uint16_t, uint16_t)
SEPT_REGISTER_COMPARE_EVALUATOR(uint32_t, uint32_t)
SEPT_REGISTER_COMPARE_EVALUATOR(uint64_t, uint64_t)
SEPT_REGISTER_COMPARE_EVALUATOR(float, float)
SEPT_REGISTER_COMPARE_EVALUATOR(double, double)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(BoolType_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Sint8Type_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Sint16Type_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Sint32Type_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Sint64Type_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Uint8Type_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Uint16Type_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Uint32Type_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Uint64Type_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Float32Type_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Float64Type_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(ArrayType_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(ArrayES_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(ArrayE_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(ArrayS_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(Array_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(OrderedMapType_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(OrderedMapDC_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(OrderedMapD_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(OrderedMapC_c)
SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON(OrderedMap_c)

//
// Non-NonParametricTerm types
//

SEPT_REGISTER_COMPARE_EVALUATOR(ArrayESTerm_c, ArrayESTerm_c)
SEPT_REGISTER_COMPARE_EVALUATOR(ArrayETerm_c, ArrayETerm_c)
SEPT_REGISTER_COMPARE_EVALUATOR(ArraySTerm_c, ArraySTerm_c)
SEPT_REGISTER_COMPARE_EVALUATOR(ArrayTerm_c, ArrayTerm_c)
SEPT_REGISTER_COMPARE_EVALUATOR(OrderedMapDCTerm_c, OrderedMapDCTerm_c)
SEPT_REGISTER_COMPARE_EVALUATOR(OrderedMapDTerm_c, OrderedMapDTerm_c)
SEPT_REGISTER_COMPARE_EVALUATOR(OrderedMapCTerm_c, OrderedMapCTerm_c)
SEPT_REGISTER_COMPARE_EVALUATOR(OrderedMapTerm_c, OrderedMapTerm_c)

#undef SEPT_REGISTER_COMPARE_EVALUATOR_FOR_SINGLETON
#undef SEPT_REGISTER_COMPARE_EVALUATOR

} // end namespace _compare
} // end namespace sept
