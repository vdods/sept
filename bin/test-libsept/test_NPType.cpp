// 2020.07.18 - Victor Dods

#include <lvd/test.hpp>
#include "req.hpp"
#include "sept/NPType.hpp"

LVD_TEST_BEGIN(310__NPType__compare)
    SEPT_TEST_REQ_COMPARE_IS_EQ(true, true);
    SEPT_TEST_REQ_COMPARE_IS_EQ(false, false);
    SEPT_TEST_REQ_COMPARE_IS_LT(false, true);
    SEPT_TEST_REQ_COMPARE_IS_GT(true, false);

    SEPT_TEST_REQ_COMPARE_IS_EQ(int8_t(10), int8_t(10));
    SEPT_TEST_REQ_COMPARE_IS_LT(int8_t(10), int8_t(20));
    SEPT_TEST_REQ_COMPARE_IS_GT(int8_t(10), int8_t(0));

    SEPT_TEST_REQ_COMPARE_IS_EQ(int16_t(10), int16_t(10));
    SEPT_TEST_REQ_COMPARE_IS_LT(int16_t(10), int16_t(20));
    SEPT_TEST_REQ_COMPARE_IS_GT(int16_t(10), int16_t(0));

    SEPT_TEST_REQ_COMPARE_IS_EQ(int32_t(10), int32_t(10));
    SEPT_TEST_REQ_COMPARE_IS_LT(int32_t(10), int32_t(20));
    SEPT_TEST_REQ_COMPARE_IS_GT(int32_t(10), int32_t(0));

    SEPT_TEST_REQ_COMPARE_IS_EQ(int64_t(10), int64_t(10));
    SEPT_TEST_REQ_COMPARE_IS_LT(int64_t(10), int64_t(20));
    SEPT_TEST_REQ_COMPARE_IS_GT(int64_t(10), int64_t(0));

    SEPT_TEST_REQ_COMPARE_IS_EQ(uint8_t(10), uint8_t(10));
    SEPT_TEST_REQ_COMPARE_IS_LT(uint8_t(10), uint8_t(20));
    SEPT_TEST_REQ_COMPARE_IS_GT(uint8_t(10), uint8_t(0));

    SEPT_TEST_REQ_COMPARE_IS_EQ(uint16_t(10), uint16_t(10));
    SEPT_TEST_REQ_COMPARE_IS_LT(uint16_t(10), uint16_t(20));
    SEPT_TEST_REQ_COMPARE_IS_GT(uint16_t(10), uint16_t(0));

    SEPT_TEST_REQ_COMPARE_IS_EQ(uint32_t(10), uint32_t(10));
    SEPT_TEST_REQ_COMPARE_IS_LT(uint32_t(10), uint32_t(20));
    SEPT_TEST_REQ_COMPARE_IS_GT(uint32_t(10), uint32_t(0));

    SEPT_TEST_REQ_COMPARE_IS_EQ(uint64_t(10), uint64_t(10));
    SEPT_TEST_REQ_COMPARE_IS_LT(uint64_t(10), uint64_t(20));
    SEPT_TEST_REQ_COMPARE_IS_GT(uint64_t(10), uint64_t(0));

    SEPT_TEST_REQ_COMPARE_IS_EQ(float(10), float(10));
    SEPT_TEST_REQ_COMPARE_IS_LT(float(10), float(20));
    SEPT_TEST_REQ_COMPARE_IS_GT(float(10), float(0));

    SEPT_TEST_REQ_COMPARE_IS_EQ(double(10), double(10));
    SEPT_TEST_REQ_COMPARE_IS_LT(double(10), double(20));
    SEPT_TEST_REQ_COMPARE_IS_GT(double(10), double(0));
LVD_TEST_END
