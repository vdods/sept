// 2020.04.24 - Victor Dods

#include <lvd/req.hpp>
#include <lvd/test.hpp>
#include "sept/NPTerm.hpp"
#include "sept/NPType.hpp"

LVD_TEST_BEGIN(400__inhabits__0)
    LVD_TEST_REQ_IS_TRUE(sept::inhabits(sept::Void, sept::VoidType));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits(sept::Void, sept::Term));

    LVD_TEST_REQ_IS_TRUE(sept::inhabits(sept::VoidType, sept::Term));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits(sept::VoidType, sept::Type));

    LVD_TEST_REQ_IS_TRUE(sept::inhabits(sept::Bool, sept::BoolType));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits(sept::TrueType, sept::BoolType));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits(sept::FalseType, sept::BoolType));

    LVD_TEST_REQ_IS_TRUE(sept::inhabits(sept::True, sept::TrueType));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits(sept::BoolTerm_c(true), sept::TrueType));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits(sept::Bool(true), sept::TrueType)); // Bool(...) is preferred over BoolTerm_c(...)
    LVD_TEST_REQ_IS_TRUE(sept::inhabits(true, sept::TrueType));

    LVD_TEST_REQ_IS_TRUE(sept::inhabits(sept::False, sept::FalseType));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits(sept::BoolTerm_c(false), sept::FalseType));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits(sept::Bool(false), sept::FalseType)); // Bool(...) is preferred over BoolTerm_c(...)
    LVD_TEST_REQ_IS_TRUE(sept::inhabits(false, sept::FalseType));

    LVD_TEST_REQ_IS_TRUE(sept::inhabits(sept::TrueType, sept::BoolType));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits(sept::FalseType, sept::BoolType));
LVD_TEST_END

LVD_TEST_BEGIN(400__inhabits__1__data)
    LVD_TEST_REQ_IS_TRUE(sept::inhabits_data(sept::Void, sept::VoidType));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits_data(sept::Void, sept::Term));

    LVD_TEST_REQ_IS_FALSE(sept::inhabits_data(sept::Void, sept::Bool));
    LVD_TEST_REQ_IS_FALSE(sept::inhabits_data(sept::Void, sept::BoolType));
    LVD_TEST_REQ_IS_FALSE(sept::inhabits_data(sept::Void, sept::TrueType));
    LVD_TEST_REQ_IS_FALSE(sept::inhabits_data(sept::Void, sept::FalseType));

    LVD_TEST_REQ_IS_TRUE(sept::inhabits_data(sept::VoidType, sept::Term));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits_data(sept::VoidType, sept::Type));

    LVD_TEST_REQ_IS_TRUE(sept::inhabits_data(sept::Bool, sept::BoolType));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits_data(sept::TrueType, sept::BoolType));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits_data(sept::FalseType, sept::BoolType));

    LVD_TEST_REQ_IS_TRUE(sept::inhabits_data(sept::True, sept::TrueType));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits_data(sept::BoolTerm_c(true), sept::TrueType));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits_data(sept::Bool(true), sept::TrueType)); // Bool(...) is preferred over BoolTerm_c(...)
    LVD_TEST_REQ_IS_TRUE(sept::inhabits_data(true, sept::TrueType));

    LVD_TEST_REQ_IS_TRUE(sept::inhabits_data(sept::False, sept::FalseType));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits_data(sept::BoolTerm_c(false), sept::FalseType));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits_data(sept::Bool(false), sept::FalseType)); // Bool(...) is preferred over BoolTerm_c(...)
    LVD_TEST_REQ_IS_TRUE(sept::inhabits_data(false, sept::FalseType));

    LVD_TEST_REQ_IS_TRUE(sept::inhabits_data(sept::TrueType, sept::BoolType));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits_data(sept::FalseType, sept::BoolType));
LVD_TEST_END
