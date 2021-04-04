// 2021.04.02 - Victor Dods

#include <lvd/test.hpp>
#include "req.hpp"
#include "sept/FormalTypeOf.hpp"
#include "sept/NPType.hpp"

LVD_TEST_BEGIN(573__FormalTypeOf__0)
    // First, compare exact identity of the singletons involved.
    LVD_TEST_REQ_EQ(&sept::abstract_type_of(sept::FormalTypeOf), &sept::NonParametricType);

    // Now, compare values
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::FormalTypeOf), sept::NonParametricType);

    // Test FormalTypeOf's instantiation.
    LVD_TEST_REQ_EQ(
        sept::FormalTypeOf(sept::True),
        sept::FormalTypeOf_Term_c{sept::True}
    );

    // Test inhabits
    LVD_TEST_REQ_IS_TRUE(sept::inhabits_data(sept::True, sept::FormalTypeOf(sept::True)));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits_data(sept::Uint32, sept::FormalTypeOf(sept::Uint32)));
LVD_TEST_END
