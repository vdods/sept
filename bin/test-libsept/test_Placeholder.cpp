// 2020.07.25 - Victor Dods

#include <lvd/test.hpp>
#include "req.hpp"
#include "sept/Placeholder.hpp"

LVD_TEST_BEGIN(590__Placeholder__Placeholder)
    // First, compare exact identity of the singletons involved.
    LVD_TEST_REQ_EQ(&sept::abstract_type_of(sept::Placeholder), &sept::PlaceholderType);
    LVD_TEST_REQ_EQ(&sept::concrete_type_of(sept::Placeholder), &sept::NonParametricTerm);

    // Now, compare values
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Placeholder), sept::PlaceholderType);
    LVD_TEST_REQ_EQ(sept::concrete_type_of(sept::Placeholder), sept::NonParametricTerm);

    // Test Placeholder's instantiation.
    LVD_TEST_REQ_EQ(sept::Placeholder(0), sept::PlaceholderTerm_c(0));
    LVD_TEST_REQ_EQ(sept::Placeholder(1), sept::PlaceholderTerm_c(1));
    LVD_TEST_REQ_NEQ(sept::Placeholder(0), sept::PlaceholderTerm_c(1));
    LVD_TEST_REQ_NEQ(sept::Placeholder(1), sept::PlaceholderTerm_c(0));

    // Test type of Placeholder's instantiation.
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Placeholder(0)), sept::Placeholder);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Placeholder(1)), sept::Placeholder);
    LVD_TEST_REQ_NEQ(sept::abstract_type_of(sept::Placeholder(1)), sept::Placeholder(1));

    // Test instantiation of Placeholder(n)
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Placeholder(0)(1,2,3)), sept::Placeholder(0));
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Placeholder(1)(4,5,6)), sept::Placeholder(1));

    // Test instantiation of Placeholder(n)(...)
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Placeholder(1)(4,5,6)(7,8)), sept::Placeholder(1)(4,5,6));
LVD_TEST_END

LVD_TEST_BEGIN(590__Placeholder__compare)
    SEPT_TEST_REQ_COMPARE_IS_EQ(sept::Placeholder(0), sept::Placeholder(0));
    SEPT_TEST_REQ_COMPARE_IS_LEQ(sept::Placeholder(0), sept::Placeholder(0));
    SEPT_TEST_REQ_COMPARE_IS_GEQ(sept::Placeholder(0), sept::Placeholder(0));
    SEPT_TEST_REQ_COMPARE_IS_LT(sept::Placeholder(0), sept::Placeholder(1));
    SEPT_TEST_REQ_COMPARE_IS_LEQ(sept::Placeholder(0), sept::Placeholder(1));
    SEPT_TEST_REQ_COMPARE_IS_GT(sept::Placeholder(1), sept::Placeholder(0));
    SEPT_TEST_REQ_COMPARE_IS_GEQ(sept::Placeholder(1), sept::Placeholder(0));
LVD_TEST_END
