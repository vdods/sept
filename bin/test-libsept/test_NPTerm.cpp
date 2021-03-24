// 2020.04.17 - Victor Dods

#include <lvd/test.hpp>
#include "req.hpp"
#include "sept/ArrayType.hpp"
#include "sept/ArrayTerm.hpp"
#include "sept/NPTerm.hpp"
#include "sept/NPType.hpp"

LVD_TEST_BEGIN(300__NPTerm__basics)
    // First, compare exact identity of the singletons involved.

    LVD_TEST_REQ_EQ(&sept::abstract_type_of(sept::Void), &sept::VoidType);
    LVD_TEST_REQ_EQ(&sept::abstract_type_of(sept::True), &sept::TrueType);
    LVD_TEST_REQ_EQ(&sept::abstract_type_of(sept::False), &sept::FalseType);

    LVD_TEST_REQ_EQ(&sept::concrete_type_of(sept::VoidType), &sept::NonParametricTerm);
    LVD_TEST_REQ_EQ(&sept::concrete_type_of(sept::TrueType), &sept::NonParametricTerm);
    LVD_TEST_REQ_EQ(&sept::concrete_type_of(sept::FalseType), &sept::NonParametricTerm);

    // Now, compare values

    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Void), sept::VoidType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::True), sept::TrueType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::False), sept::FalseType);

    LVD_TEST_REQ_EQ(sept::concrete_type_of(sept::Void), sept::NonParametricTerm);
    LVD_TEST_REQ_EQ(sept::concrete_type_of(sept::True), sept::NonParametricTerm);
    LVD_TEST_REQ_EQ(sept::concrete_type_of(sept::False), sept::NonParametricTerm);

    // More comparisons

    LVD_TEST_REQ_EQ(sept::BoolTerm_c{true}, sept::True);
    LVD_TEST_REQ_EQ(sept::BoolTerm_c{false}, sept::False);
    LVD_TEST_REQ_EQ(sept::True, sept::BoolTerm_c{true});
    LVD_TEST_REQ_EQ(sept::False, sept::BoolTerm_c{false});
    LVD_TEST_REQ_EQ(sept::True, sept::True);
    LVD_TEST_REQ_EQ(sept::False, sept::False);
    LVD_TEST_REQ_NEQ(sept::BoolTerm_c{true}, sept::BoolTerm_c{false});
    LVD_TEST_REQ_NEQ(sept::BoolTerm_c{false}, sept::BoolTerm_c{true});
    LVD_TEST_REQ_NEQ(sept::BoolTerm_c{true}, sept::False);
    LVD_TEST_REQ_NEQ(sept::BoolTerm_c{false}, sept::True);
    LVD_TEST_REQ_NEQ(sept::False, sept::BoolTerm_c{true});
    LVD_TEST_REQ_NEQ(sept::True, sept::BoolTerm_c{false});
    LVD_TEST_REQ_NEQ(sept::True, sept::False);
    LVD_TEST_REQ_NEQ(sept::False, sept::True);

    // `compare` function

    SEPT_TEST_REQ_COMPARE_IS_EQ(sept::Term, sept::Term);
    SEPT_TEST_REQ_COMPARE_IS_EQ(sept::NonParametricTerm, sept::NonParametricTerm);
//     SEPT_TEST_REQ_COMPARE_IS_EQ(sept::ParametricTerm, sept::ParametricTerm);
    SEPT_TEST_REQ_COMPARE_IS_EQ(sept::Void, sept::Void);
    SEPT_TEST_REQ_COMPARE_IS_EQ(sept::True, sept::True);
    SEPT_TEST_REQ_COMPARE_IS_EQ(sept::False, sept::False);
    SEPT_TEST_REQ_COMPARE_IS_GT(sept::True, sept::False);
    SEPT_TEST_REQ_COMPARE_IS_LT(sept::False, sept::True);

    // The Bool object can instantiate BoolTerm_c
    LVD_TEST_REQ_EQ(sept::Bool(true), sept::BoolTerm_c{true});
    LVD_TEST_REQ_EQ(sept::Bool(false), sept::BoolTerm_c{false});
    LVD_TEST_REQ_EQ(sept::BoolTerm_c{true}, sept::Bool(true));
    LVD_TEST_REQ_EQ(sept::BoolTerm_c{false}, sept::Bool(false));

    // Sint8
    LVD_TEST_REQ_EQ(sept::Sint8(123), 123);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Sint8), sept::Sint8Type);

    // ArrayES, ArrayE, ArrayS, Array
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayES), sept::ArrayType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayE), sept::ArrayType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayS), sept::ArrayType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Array), sept::ArrayType);

    // Instantiating ArrayES, ArrayE, ArrayS
    LVD_TEST_REQ_EQ(sept::ArrayES(sept::Bool,10), sept::ArrayESTerm_c(sept::Bool,10));
    LVD_TEST_REQ_EQ(sept::ArrayE(sept::Bool), sept::ArrayETerm_c(sept::Bool));
    LVD_TEST_REQ_EQ(sept::ArrayS(10), sept::ArraySTerm_c{10});
LVD_TEST_END

LVD_TEST_BEGIN(300__NPTerm__printing)
    #define TEST_CASE_NPTerm_as_string(T) LVD_TEST_REQ_EQ(LVD_FMT(sept::T), #T)

    TEST_CASE_NPTerm_as_string(Term);
    TEST_CASE_NPTerm_as_string(NonParametricTerm);
//     TEST_CASE_NPTerm_as_string(ParametricTerm);
    TEST_CASE_NPTerm_as_string(Type);
    TEST_CASE_NPTerm_as_string(NonType);
    TEST_CASE_NPTerm_as_string(NonParametricType);
//     TEST_CASE_NPTerm_as_string(ParametricType);
    TEST_CASE_NPTerm_as_string(Void);
    TEST_CASE_NPTerm_as_string(True);
    TEST_CASE_NPTerm_as_string(False);
    TEST_CASE_NPTerm_as_string(VoidType);
    TEST_CASE_NPTerm_as_string(TrueType);
    TEST_CASE_NPTerm_as_string(FalseType);
    TEST_CASE_NPTerm_as_string(EmptyType);
    TEST_CASE_NPTerm_as_string(Bool);
    TEST_CASE_NPTerm_as_string(Sint8);
//     TEST_CASE_NPTerm_as_string(Sint16);
//     TEST_CASE_NPTerm_as_string(Sint32);
//     TEST_CASE_NPTerm_as_string(Sint64);
//     TEST_CASE_NPTerm_as_string(Uint8);
//     TEST_CASE_NPTerm_as_string(Uint16);
//     TEST_CASE_NPTerm_as_string(Uint32);
//     TEST_CASE_NPTerm_as_string(Uint64);
//     TEST_CASE_NPTerm_as_string(Float32);
//     TEST_CASE_NPTerm_as_string(Float64);
    TEST_CASE_NPTerm_as_string(BoolType);
    TEST_CASE_NPTerm_as_string(Sint8Type);
//     TEST_CASE_NPTerm_as_string(Sint16Type);
//     TEST_CASE_NPTerm_as_string(Sint32Type);
//     TEST_CASE_NPTerm_as_string(Sint64Type);
//     TEUT_CAUE_NPTerm_as_string(Uint8Type);
//     TEUT_CAUE_NPTerm_as_string(Uint16Type);
//     TEUT_CAUE_NPTerm_as_string(Uint32Type);
//     TEUT_CAUE_NPTerm_as_string(Uint64Type);
//     TEST_CASE_NPTerm_as_string(Float32Type);
//     TEST_CASE_NPTerm_as_string(Float64Type);
//     TEST_CASE_NPTerm_as_string(SintType);
//     TEST_CASE_NPTerm_as_string(Sint);
//     TEST_CASE_NPTerm_as_string(UintType);
//     TEST_CASE_NPTerm_as_string(Uint);
//     TEST_CASE_NPTerm_as_string(FloatType);
//     TEST_CASE_NPTerm_as_string(Float);
//     TEST_CASE_NPTerm_as_string(PODType);
//     TEST_CASE_NPTerm_as_string(POD);
//     TEST_CASE_NPTerm_as_string(Union);
//     TEST_CASE_NPTerm_as_string(Intersection);
//     TEST_CASE_NPTerm_as_string(Negation);
//     TEST_CASE_NPTerm_as_string(Difference);
    TEST_CASE_NPTerm_as_string(ArrayType);
    TEST_CASE_NPTerm_as_string(ArrayES);
    TEST_CASE_NPTerm_as_string(ArrayE);
    TEST_CASE_NPTerm_as_string(ArrayS);
    TEST_CASE_NPTerm_as_string(Array);
LVD_TEST_END

LVD_TEST_BEGIN(300__NPTerm__hash)
    #define TEST_CASE_NPTerm_hash(T) LVD_TEST_REQ_EQ(sept::hash(sept::T), sept::hash(sept::T))

    TEST_CASE_NPTerm_hash(Term);
    TEST_CASE_NPTerm_hash(NonParametricTerm);
//     TEST_CASE_NPTerm_hash(ParametricTerm);
    TEST_CASE_NPTerm_hash(Type);
    TEST_CASE_NPTerm_hash(NonType);
    TEST_CASE_NPTerm_hash(NonParametricType);
//     TEST_CASE_NPTerm_hash(ParametricType);
    TEST_CASE_NPTerm_hash(Void);
    TEST_CASE_NPTerm_hash(True);
    TEST_CASE_NPTerm_hash(False);
    TEST_CASE_NPTerm_hash(VoidType);
    TEST_CASE_NPTerm_hash(TrueType);
    TEST_CASE_NPTerm_hash(FalseType);
    TEST_CASE_NPTerm_hash(EmptyType);
    TEST_CASE_NPTerm_hash(Bool);
    TEST_CASE_NPTerm_hash(Sint8);
//     TEST_CASE_NPTerm_hash(Sint16);
//     TEST_CASE_NPTerm_hash(Sint32);
//     TEST_CASE_NPTerm_hash(Sint64);
//     TEST_CASE_NPTerm_hash(Uint8);
//     TEST_CASE_NPTerm_hash(Uint16);
//     TEST_CASE_NPTerm_hash(Uint32);
//     TEST_CASE_NPTerm_hash(Uint64);
//     TEST_CASE_NPTerm_hash(Float32);
//     TEST_CASE_NPTerm_hash(Float64);
    TEST_CASE_NPTerm_hash(BoolType);
    TEST_CASE_NPTerm_hash(Sint8Type);
//     TEST_CASE_NPTerm_hash(Sint16Type);
//     TEST_CASE_NPTerm_hash(Sint32Type);
//     TEST_CASE_NPTerm_hash(Sint64Type);
//     TEUT_CAUE_NPTerm_hash(Uint8Type);
//     TEUT_CAUE_NPTerm_hash(Uint16Type);
//     TEUT_CAUE_NPTerm_hash(Uint32Type);
//     TEUT_CAUE_NPTerm_hash(Uint64Type);
//     TEST_CASE_NPTerm_hash(Float32Type);
//     TEST_CASE_NPTerm_hash(Float64Type);
//     TEST_CASE_NPTerm_hash(SintType);
//     TEST_CASE_NPTerm_hash(Sint);
//     TEST_CASE_NPTerm_hash(UintType);
//     TEST_CASE_NPTerm_hash(Uint);
//     TEST_CASE_NPTerm_hash(FloatType);
//     TEST_CASE_NPTerm_hash(Float);
//     TEST_CASE_NPTerm_hash(PODType);
//     TEST_CASE_NPTerm_hash(POD);
//     TEST_CASE_NPTerm_hash(Union);
//     TEST_CASE_NPTerm_hash(Intersection);
//     TEST_CASE_NPTerm_hash(Negation);
//     TEST_CASE_NPTerm_hash(Difference);
    TEST_CASE_NPTerm_hash(ArrayType);
    TEST_CASE_NPTerm_hash(ArrayES);
    TEST_CASE_NPTerm_hash(ArrayE);
    TEST_CASE_NPTerm_hash(ArrayS);
    TEST_CASE_NPTerm_hash(Array);
LVD_TEST_END
