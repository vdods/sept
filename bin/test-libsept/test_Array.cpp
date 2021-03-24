// 2020.04.20 - Victor Dods

#include <lvd/test.hpp>
#include "req.hpp"
#include "sept/ArrayType.hpp"
#include "sept/ArrayTerm.hpp"
#include "sept/NPType.hpp"

LVD_TEST_BEGIN(500__Array__Array)
    // First, compare exact identity of the singletons involved.
    LVD_TEST_REQ_EQ(&sept::abstract_type_of(sept::Array), &sept::ArrayType);
    LVD_TEST_REQ_EQ(&sept::concrete_type_of(sept::Array), &sept::NonParametricTerm);

    // Now, compare values
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Array), sept::ArrayType);
    LVD_TEST_REQ_EQ(sept::concrete_type_of(sept::Array), sept::NonParametricTerm);

    // Test Array's instantiation.
    LVD_TEST_REQ_EQ(sept::Array(10,20,30), sept::ArrayTerm_c(std::vector{10,20,30}));
LVD_TEST_END

LVD_TEST_BEGIN(500__Array__compare)
    SEPT_TEST_REQ_COMPARE_IS_EQ(sept::Array(), sept::Array());
    SEPT_TEST_REQ_COMPARE_IS_LT(sept::Array(), sept::Array(10));
    SEPT_TEST_REQ_COMPARE_IS_LT(sept::Array(), sept::Array(10,20));
    SEPT_TEST_REQ_COMPARE_IS_LT(sept::Array(), sept::Array(10,20,30));
    SEPT_TEST_REQ_COMPARE_IS_GT(sept::Array(10), sept::Array());
    SEPT_TEST_REQ_COMPARE_IS_EQ(sept::Array(10), sept::Array(10));
    SEPT_TEST_REQ_COMPARE_IS_LT(sept::Array(10), sept::Array(20));
    SEPT_TEST_REQ_COMPARE_IS_GT(sept::Array(30), sept::Array(20));
    SEPT_TEST_REQ_COMPARE_IS_GT(sept::Array(30,30), sept::Array(20,30));
    SEPT_TEST_REQ_COMPARE_IS_GT(sept::Array(30,30), sept::Array(20,30,40));
LVD_TEST_END

LVD_TEST_BEGIN(500__Array__ArrayES)
    // First, compare exact identity of the singletons involved.
    LVD_TEST_REQ_EQ(&sept::abstract_type_of(sept::ArrayES), &sept::ArrayType);
    LVD_TEST_REQ_EQ(&sept::concrete_type_of(sept::ArrayES), &sept::NonParametricTerm);

    // Now, compare values
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayES), sept::ArrayType);
    LVD_TEST_REQ_EQ(sept::concrete_type_of(sept::ArrayES), sept::NonParametricTerm);

    // Test Array's instantiation with ArrayES constraint

    // This one doesn't work at the moment because std::vector<bool>::operator[] returns a "bit reference"
    // object which isn't the same as bool, and therefore doesn't work as expected in the is_instance
    // function.
//     LVD_TEST_REQ_EQ(
//         sept::ArrayES(sept::Bool,3)(true,false,true),
//         sept::ArrayTerm_c(std::vector<bool>{true,false,true}).with_constraint(sept::ArrayES(sept::Bool,3))
//     );
    LVD_TEST_REQ_EQ(
        sept::ArrayES(sept::Sint32,3)(123,456,789),
        sept::ArrayTerm_c(std::vector<int32_t>{123,456,789}).with_constraint(sept::ArrayES(sept::Sint32,3))
    );
LVD_TEST_END

LVD_TEST_BEGIN(500__Array__ArrayE)
    // First, compare exact identity of the singletons involved.
    LVD_TEST_REQ_EQ(&sept::abstract_type_of(sept::ArrayE), &sept::ArrayType);
    LVD_TEST_REQ_EQ(&sept::concrete_type_of(sept::ArrayE), &sept::NonParametricTerm);

    // Now, compare values
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayE), sept::ArrayType);
    LVD_TEST_REQ_EQ(sept::concrete_type_of(sept::ArrayE), sept::NonParametricTerm);

    // Test Array's instantiation with ArrayE constraint

    // This one doesn't work at the moment because std::vector<bool>::operator[] returns a "bit reference"
    // object which isn't the same as bool, and therefore doesn't work as expected in the is_instance
    // function.
//     LVD_TEST_REQ_EQ(
//         sept::ArrayE(sept::Bool)(true,false,true),
//         sept::ArrayTerm_c(std::vector<bool>{true,false,true}).with_constraint(sept::ArrayE(sept::Bool))
//     );
    LVD_TEST_REQ_EQ(
        sept::ArrayE(sept::Sint32)(123,456,789),
        sept::ArrayTerm_c(std::vector<int32_t>{123,456,789}).with_constraint(sept::ArrayE(sept::Sint32))
    );
LVD_TEST_END

LVD_TEST_BEGIN(500__Array__ArrayS)
    // First, compare exact identity of the singletons involved.
    LVD_TEST_REQ_EQ(&sept::abstract_type_of(sept::ArrayS), &sept::ArrayType);
    LVD_TEST_REQ_EQ(&sept::concrete_type_of(sept::ArrayS), &sept::NonParametricTerm);

    // Now, compare values
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ArrayS), sept::ArrayType);
    LVD_TEST_REQ_EQ(sept::concrete_type_of(sept::ArrayS), sept::NonParametricTerm);

    // Test Array's instantiation with ArrayS constraint
    LVD_TEST_REQ_EQ(
        sept::ArrayS(3)(123,true,40.404),
        sept::ArrayTerm_c(std::vector<sept::Data>{123,true,40.404}).with_constraint(sept::ArrayS(3))
    );
LVD_TEST_END
