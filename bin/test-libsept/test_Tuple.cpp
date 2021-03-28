// 2021.03.27 - Victor Dods

#include <lvd/test.hpp>
#include "req.hpp"
#include "sept/ArrayTerm.hpp"
#include "sept/ArrayType.hpp"
#include "sept/Tuple.hpp"
#include "sept/TupleTerm.hpp"
#include "sept/NPType.hpp"

LVD_TEST_BEGIN(570__Tuple__0)
    // First, compare exact identity of the singletons involved.
    LVD_TEST_REQ_EQ(&sept::abstract_type_of(sept::Tuple), &sept::TupleType);
    LVD_TEST_REQ_EQ(&sept::abstract_type_of(sept::TupleType), &sept::NonParametricType);

    // Now, compare values
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Tuple), sept::TupleType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::TupleType), sept::NonParametricType);

    // Test Tuple's instantiation.
    LVD_TEST_REQ_EQ(sept::Tuple(10,20,30), sept::TupleTerm_c(sept::DataVector{10,20,30}));
LVD_TEST_END

LVD_TEST_BEGIN(570__Tuple__compare)
    SEPT_TEST_REQ_COMPARE_IS_EQ(sept::Tuple(), sept::Tuple());
    SEPT_TEST_REQ_COMPARE_IS_LT(sept::Tuple(), sept::Tuple(10));
    SEPT_TEST_REQ_COMPARE_IS_LT(sept::Tuple(), sept::Tuple(10,20));
    SEPT_TEST_REQ_COMPARE_IS_LT(sept::Tuple(), sept::Tuple(10,20,30));
    SEPT_TEST_REQ_COMPARE_IS_GT(sept::Tuple(10), sept::Tuple());
    SEPT_TEST_REQ_COMPARE_IS_EQ(sept::Tuple(10), sept::Tuple(10));
    SEPT_TEST_REQ_COMPARE_IS_LT(sept::Tuple(10), sept::Tuple(20));
    SEPT_TEST_REQ_COMPARE_IS_GT(sept::Tuple(30), sept::Tuple(20));
    SEPT_TEST_REQ_COMPARE_IS_GT(sept::Tuple(30,30), sept::Tuple(20,30));
    SEPT_TEST_REQ_COMPARE_IS_GT(sept::Tuple(30,30), sept::Tuple(20,30,40));
LVD_TEST_END

LVD_TEST_BEGIN(570__Tuple__inhabits)
    auto v0 = sept::Tuple(sept::Float64(12.3), sept::Uint32(3001), sept::Array(1,2,3));
//     test_log << lvd::Log::trc() << LVD_REFLECT(v0) << '\n' << LVD_REFLECT(sept::tuple_type_of(v0)) << '\n';
    auto t0 = sept::Tuple(sept::Float64, sept::Uint32, sept::Array);
    LVD_TEST_REQ_EQ(sept::tuple_type_of(v0), t0);
    LVD_TEST_REQ_IS_TRUE(sept::inhabits(v0, t0));
LVD_TEST_END
