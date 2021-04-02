// 2021.03.31 - Victor Dods

#include <lvd/test.hpp>
#include "req.hpp"
#include "sept/Ref.hpp"
#include "sept/RefTerm.hpp"
#include "sept/NPType.hpp"

LVD_TEST_BEGIN(572__Ref__0)
    // First, compare exact identity of the singletons involved.
    LVD_TEST_REQ_EQ(&sept::abstract_type_of(sept::Ref), &sept::RefType);
    LVD_TEST_REQ_EQ(&sept::abstract_type_of(sept::RefType), &sept::NonParametricType);

    // Now, compare values
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Ref), sept::RefType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::RefType), sept::NonParametricType);

    // Test Ref's instantiation.
    sept::Data x{10.5};
    test_log << lvd::Log::trc()
             << LVD_REFLECT(x) << '\n'
             << LVD_REFLECT(x.can_cast<double>()) << '\n'
             << LVD_REFLECT(x.can_cast<double const>()) << '\n'
             << LVD_REFLECT(x.is_ref()) << '\n'
             << LVD_REFLECT(x.cast<double>()) << '\n'
//              << LVD_REFLECT(x.is_ref_and_can_cast<double>()) << '\n'
//              << LVD_REFLECT(x.is_ref_and_can_cast<double const>()) << '\n'
             << '\n';
    LVD_TEST_REQ_EQ(sept::Ref(&x), sept::RefTerm_c(&x));

    // Test abstract_type_of
    auto r = sept::Ref(&x);
    auto dr = sept::Data{r};
    test_log << lvd::Log::trc()
             << LVD_REFLECT(dr) << '\n'
             << LVD_REFLECT(dr.can_cast<double>()) << '\n'
             << LVD_REFLECT(dr.can_cast<double const>()) << '\n'
             << LVD_REFLECT(dr.is_ref()) << '\n'
             << LVD_REFLECT(dr.cast<double>()) << '\n'
//              << LVD_REFLECT(dr.is_ref_and_can_cast<double>()) << '\n'
//              << LVD_REFLECT(dr.is_ref_and_can_cast<double const>()) << '\n'
//              << LVD_REFLECT(dr.is_ref_and_can_cast<int>()) << '\n'
//              << LVD_REFLECT(dr.is_ref_and_can_cast<float>()) << '\n'
             << '\n';

    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(x), sept::Float64);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(r), sept::Float64);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(dr), sept::Float64);

    LVD_TEST_REQ_EQ(x, sept::Float64(10.5));
    // TODO
//     LVD_TEST_REQ_EQ(r, sept::Float64(10.5));
//     LVD_TEST_REQ_EQ(dr, sept::Float64(10.5));

LVD_TEST_END

// LVD_TEST_BEGIN(572__Ref__compare)
//     SEPT_TEST_REQ_COMPARE_IS_EQ(sept::Ref(), sept::Ref());
//     SEPT_TEST_REQ_COMPARE_IS_LT(sept::Ref(), sept::Ref(10));
//     SEPT_TEST_REQ_COMPARE_IS_LT(sept::Ref(), sept::Ref(10,20));
//     SEPT_TEST_REQ_COMPARE_IS_LT(sept::Ref(), sept::Ref(10,20,30));
//     SEPT_TEST_REQ_COMPARE_IS_GT(sept::Ref(10), sept::Ref());
//     SEPT_TEST_REQ_COMPARE_IS_EQ(sept::Ref(10), sept::Ref(10));
//     SEPT_TEST_REQ_COMPARE_IS_LT(sept::Ref(10), sept::Ref(20));
//     SEPT_TEST_REQ_COMPARE_IS_GT(sept::Ref(30), sept::Ref(20));
//     SEPT_TEST_REQ_COMPARE_IS_GT(sept::Ref(30,30), sept::Ref(20,30));
//     SEPT_TEST_REQ_COMPARE_IS_GT(sept::Ref(30,30), sept::Ref(20,30,40));
// LVD_TEST_END
//
// LVD_TEST_BEGIN(572__Ref__inhabits)
//     auto v0 = sept::Ref(sept::Float64(12.3), sept::Uint32(3001), sept::Array(1,2,3));
// //     test_log << lvd::Log::trc() << LVD_REFLECT(v0) << '\n' << LVD_REFLECT(sept::tuple_type_of(v0)) << '\n';
//     auto t0 = sept::Ref(sept::Float64, sept::Uint32, sept::Array);
//     LVD_TEST_REQ_EQ(sept::abstract_type_of(v0), t0);
//     LVD_TEST_REQ_IS_TRUE(sept::inhabits(v0, t0));
// LVD_TEST_END
