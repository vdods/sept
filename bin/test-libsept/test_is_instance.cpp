// 2020.04.24 - Victor Dods

#include <lvd/req.hpp>
#include <lvd/test.hpp>
#include "sept/is_instance.hpp"
#include "sept/NPTerm.hpp"
#include "sept/NPType.hpp"

LVD_TEST_BEGIN(400__is_instance__0)
    LVD_TEST_REQ_IS_TRUE(sept::is_instance(sept::Void, sept::VoidType));
    LVD_TEST_REQ_IS_TRUE(sept::is_instance(sept::Void, sept::Term));

    LVD_TEST_REQ_IS_FALSE(sept::is_instance(sept::Void, sept::Bool));
    LVD_TEST_REQ_IS_FALSE(sept::is_instance(sept::Void, sept::BoolType));
    LVD_TEST_REQ_IS_FALSE(sept::is_instance(sept::Void, sept::TrueType));
    LVD_TEST_REQ_IS_FALSE(sept::is_instance(sept::Void, sept::FalseType));

    LVD_TEST_REQ_IS_TRUE(sept::is_instance(sept::VoidType, sept::Term));
    LVD_TEST_REQ_IS_TRUE(sept::is_instance(sept::VoidType, sept::Type));

    LVD_TEST_REQ_IS_TRUE(sept::is_instance(sept::Bool, sept::BoolType));
    LVD_TEST_REQ_IS_TRUE(sept::is_instance(sept::TrueType, sept::BoolType));
    LVD_TEST_REQ_IS_TRUE(sept::is_instance(sept::FalseType, sept::BoolType));

    LVD_TEST_REQ_IS_TRUE(sept::is_instance(sept::True, sept::TrueType));
    LVD_TEST_REQ_IS_TRUE(sept::is_instance(sept::BoolTerm_c(true), sept::TrueType));
    LVD_TEST_REQ_IS_TRUE(sept::is_instance(sept::Bool(true), sept::TrueType)); // Bool(...) is preferred over BoolTerm_c(...)
    LVD_TEST_REQ_IS_TRUE(sept::is_instance(true, sept::TrueType));

    LVD_TEST_REQ_IS_TRUE(sept::is_instance(sept::False, sept::FalseType));
    LVD_TEST_REQ_IS_TRUE(sept::is_instance(sept::BoolTerm_c(false), sept::FalseType));
    LVD_TEST_REQ_IS_TRUE(sept::is_instance(sept::Bool(false), sept::FalseType)); // Bool(...) is preferred over BoolTerm_c(...)
    LVD_TEST_REQ_IS_TRUE(sept::is_instance(false, sept::FalseType));
LVD_TEST_END
