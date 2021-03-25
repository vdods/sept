// 2020.04.25 - Victor Dods

#include <lvd/req.hpp>
#include <lvd/test.hpp>
#include "sept/ArrayTerm.hpp"
#include "sept/ArrayType.hpp"
#include "sept/Data.hpp"
#include "sept/NPTerm.hpp"
#include "sept/NPType.hpp"

LVD_TEST_BEGIN(450__abstract_type_of__specific)
    //
    // Test the specific-C++-typed overloads of abstract_type_of
    //

    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Term), sept::NonParametricType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::NonParametricTerm), sept::NonParametricType);

    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Type), sept::NonParametricType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::NonType), sept::NonParametricType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::NonParametricType), sept::NonParametricType);

    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Void), sept::VoidType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::True), sept::TrueType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::False), sept::FalseType);

    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::VoidType), sept::NonParametricType); // TODO: Should this be PODType?
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::TrueType), sept::NonParametricType); // TODO: Should this be PODType?
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::FalseType), sept::NonParametricType); // TODO: Should this be PODType?
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::EmptyType), sept::NonParametricType);

    LVD_TEST_REQ_EQ(sept::abstract_type_of(true), sept::Bool);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(false), sept::Bool);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(int8_t(123)), sept::Sint8);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(int16_t(123)), sept::Sint16);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(int32_t(123)), sept::Sint32);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(int64_t(123)), sept::Sint64);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(uint8_t(123)), sept::Uint8);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(uint16_t(123)), sept::Uint16);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(uint32_t(123)), sept::Uint32);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(uint64_t(123)), sept::Uint64);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(float(10.0f)), sept::Float32);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(double(10.0)), sept::Float64);

    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Bool), sept::BoolType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Sint8), sept::Sint8Type);

    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::BoolType), sept::NonParametricType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Sint8Type), sept::NonParametricType);

    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayES), sept::ArrayType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayE), sept::ArrayType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayS), sept::ArrayType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Array), sept::ArrayType);

    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayType), sept::NonParametricType);

    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayES(sept::Bool,3)), sept::ArrayES);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayES(sept::Sint8,4)), sept::ArrayES);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayES(sept::VoidType,2000)), sept::ArrayES);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayE(sept::Term)), sept::ArrayE);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayE(sept::NonParametricTerm)), sept::ArrayE);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayE(sept::Type)), sept::ArrayE);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayE(sept::BoolType)), sept::ArrayE);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayS(0)), sept::ArrayS);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayS(20)), sept::ArrayS);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayS(100000)), sept::ArrayS);

    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Array()), sept::Array);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Array(true)), sept::Array);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Array(true,sept::True)), sept::Array);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Array(1,2,3)), sept::Array);

    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayES(sept::Bool,0)()), sept::ArrayES(sept::Bool,0));
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayES(sept::Bool,3)(sept::True,true,sept::Bool(false))), sept::ArrayES(sept::Bool,3));

    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayE(sept::Bool)()), sept::ArrayE(sept::Bool));
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayE(sept::Bool)(sept::True,true,sept::Bool(false))), sept::ArrayE(sept::Bool));

    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayS(0)()), sept::ArrayS(0));
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayS(3)(1,true,3.0)), sept::ArrayS(3));
LVD_TEST_END

LVD_TEST_BEGIN(450__abstract_type_of__data)
    //
    // Test the Data-typed abstract_type_of_data
    //

    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::Term), sept::NonParametricType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::NonParametricTerm), sept::NonParametricType);

    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::Type), sept::NonParametricType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::NonType), sept::NonParametricType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::NonParametricType), sept::NonParametricType);

    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::Void), sept::VoidType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::True), sept::TrueType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::False), sept::FalseType);

    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::VoidType), sept::NonParametricType); // TODO: Should this be PODType?
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::TrueType), sept::NonParametricType); // TODO: Should this be PODType?
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::FalseType), sept::NonParametricType); // TODO: Should this be PODType?
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::EmptyType), sept::NonParametricType);

    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::Bool), sept::BoolType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::Sint8), sept::Sint8Type);

    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::BoolType), sept::NonParametricType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::Sint8Type), sept::NonParametricType);

    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::ArrayES), sept::ArrayType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::ArrayE), sept::ArrayType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::ArrayS), sept::ArrayType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::Array), sept::ArrayType);

    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::ArrayType), sept::NonParametricType);

    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::ArrayES(sept::Bool,3)), sept::ArrayES);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::ArrayES(sept::Sint8,4)), sept::ArrayES);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::ArrayES(sept::VoidType,2000)), sept::ArrayES);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::ArrayE(sept::Term)), sept::ArrayE);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::ArrayE(sept::NonParametricTerm)), sept::ArrayE);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::ArrayE(sept::Type)), sept::ArrayE);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::ArrayE(sept::BoolType)), sept::ArrayE);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::ArrayS(0)), sept::ArrayS);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::ArrayS(20)), sept::ArrayS);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::ArrayS(100000)), sept::ArrayS);

    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::Array()), sept::Array);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::Array(true)), sept::Array);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::Array(true,sept::True)), sept::Array);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::Array(1,2,3)), sept::Array);

    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::ArrayES(sept::Bool,0)()), sept::ArrayES(sept::Bool,0));
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::ArrayES(sept::Bool,3)(sept::True,true,sept::Bool(false))), sept::ArrayES(sept::Bool,3));

    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::ArrayE(sept::Bool)()), sept::ArrayE(sept::Bool));
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::ArrayE(sept::Bool)(sept::True,true,sept::Bool(false))), sept::ArrayE(sept::Bool));

    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::ArrayS(0)()), sept::ArrayS(0));
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(sept::ArrayS(3)(1,true,3.0)), sept::ArrayS(3));
LVD_TEST_END
