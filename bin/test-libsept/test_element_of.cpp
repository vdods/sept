// 2021.03.26 - Victor Dods

#include <lvd/test.hpp>
#include "req.hpp"
#include "sept/ArrayTerm.hpp"
#include "sept/OrderedMapTerm.hpp"

void test_case_ArrayTerm_element_of (lvd::req::Context &req_context, sept::ArrayTerm_c const &a) {
    for (uint64_t index = 0; index < a.size(); ++index) {
        LVD_TEST_REQ_EQ(sept::element_of_data(a, int8_t(index)), a[index]);
        LVD_TEST_REQ_EQ(sept::element_of_data(a, int16_t(index)), a[index]);
        LVD_TEST_REQ_EQ(sept::element_of_data(a, int32_t(index)), a[index]);
        LVD_TEST_REQ_EQ(sept::element_of_data(a, int64_t(index)), a[index]);
        LVD_TEST_REQ_EQ(sept::element_of_data(a, uint8_t(index)), a[index]);
        LVD_TEST_REQ_EQ(sept::element_of_data(a, uint16_t(index)), a[index]);
        LVD_TEST_REQ_EQ(sept::element_of_data(a, uint32_t(index)), a[index]);
        LVD_TEST_REQ_EQ(sept::element_of_data(a, uint64_t(index)), a[index]);
    }

    // Test negative indexing
    for (int64_t index = -1; index >= -int64_t(a.size()); --index) {
        LVD_TEST_REQ_EQ(sept::element_of_data(a, int8_t(index)), a[index+int64_t(a.size())]);
        LVD_TEST_REQ_EQ(sept::element_of_data(a, int16_t(index)), a[index+int64_t(a.size())]);
        LVD_TEST_REQ_EQ(sept::element_of_data(a, int32_t(index)), a[index+int64_t(a.size())]);
        LVD_TEST_REQ_EQ(sept::element_of_data(a, int64_t(index)), a[index+int64_t(a.size())]);
    }

    lvd::test::call_function_and_expect_exception<std::out_of_range>([&](){
        sept::element_of_data(a, -int64_t(a.size())-1); // 1 before the beginning.
    });
    lvd::test::call_function_and_expect_exception<std::out_of_range>([&](){
        sept::element_of_data(a, -int64_t(a.size())-2); // 2 before the beginning
    });
    lvd::test::call_function_and_expect_exception<std::out_of_range>([&](){
        sept::element_of_data(a, uint64_t(a.size())); // 1 past the end.
    });
    lvd::test::call_function_and_expect_exception<std::out_of_range>([&](){
        sept::element_of_data(a, uint64_t(a.size()+1)); // 2 past the end.
    });
}

LVD_TEST_BEGIN(560__element_of__Array)
    auto a0 = sept::Array();
    auto a1 = sept::Array(8);
    auto a2 = sept::Array(8, 20);
    auto a3 = sept::Array(8, 20, -44);
    auto a9 = sept::Array(8, 20, -44, 334, 0, 52, 10222, 999000, 89);

    test_case_ArrayTerm_element_of(req_context, a0);
    test_case_ArrayTerm_element_of(req_context, a1);
    test_case_ArrayTerm_element_of(req_context, a2);
    test_case_ArrayTerm_element_of(req_context, a3);
    test_case_ArrayTerm_element_of(req_context, a9);
LVD_TEST_END

LVD_TEST_BEGIN(560__element_of__OrderedMap)
    sept::OrderedMapTerm_c m0{sept::DataOrderedMap{}};
    sept::OrderedMapTerm_c m1{{std::pair(10,123)}};
    sept::OrderedMapTerm_c m2{{std::pair(10,123), std::pair('x',123.45)}};

    lvd::test::call_function_and_expect_exception<std::out_of_range>([&](){
        element_of_data(m0, sept::Data{0});
    });
    lvd::test::call_function_and_expect_exception<std::out_of_range>([&](){
        element_of_data(m0, sept::Data{10});
    });
    lvd::test::call_function_and_expect_exception<std::out_of_range>([&](){
        element_of_data(m0, sept::Data{'x'});
    });

    LVD_TEST_REQ_EQ(sept::element_of_data(m1, sept::Data{10}), sept::Data{123});
LVD_TEST_END
