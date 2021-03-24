// 2020.04.01 - Victor Dods

#pragma once

#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <lvd/req.hpp>
// Apparently have to include all the headers that have a compare function overload defined in them.
#include "sept/ArrayType.hpp"
#include "sept/BaseArray_t.hpp"
#include "sept/BaseArrayT_t.hpp"
#include "sept/BaseArray_S_t.hpp"
#include "sept/compare.hpp"
#include "sept/core.hpp"
#include "sept/DataArray_t.hpp"
#include "sept/DataVector.hpp"
#include "sept/NPTerm.hpp"
#include "sept/OrderedMapTerm.hpp"
#include "sept/OrderedMapType.hpp"
#include <sstream>
#include <string>

namespace req {

//
// Use these macros to get nice, descriptive output.
//

#define SEPT_TEST_REQ_COMPARE(lhs, rhs, expected, ...) req::compare(req_context, lhs, rhs, expected, #lhs, #rhs, #expected, LVD_CALL_SITE(), ##__VA_ARGS__)
#define SEPT_TEST_REQ_COMPARE_IS_EQ(lhs, rhs, ...) req::compare_is_eq(req_context, lhs, rhs, #lhs, #rhs, LVD_CALL_SITE(), ##__VA_ARGS__)
#define SEPT_TEST_REQ_COMPARE_IS_NEQ(lhs, rhs, ...) req::compare_is_neq(req_context, lhs, rhs, #lhs, #rhs, LVD_CALL_SITE(), ##__VA_ARGS__)
#define SEPT_TEST_REQ_COMPARE_IS_LT(lhs, rhs, ...) req::compare_is_lt(req_context, lhs, rhs, #lhs, #rhs, LVD_CALL_SITE(), ##__VA_ARGS__)
#define SEPT_TEST_REQ_COMPARE_IS_LEQ(lhs, rhs, ...) req::compare_is_leq(req_context, lhs, rhs, #lhs, #rhs, LVD_CALL_SITE(), ##__VA_ARGS__)
#define SEPT_TEST_REQ_COMPARE_IS_GT(lhs, rhs, ...) req::compare_is_gt(req_context, lhs, rhs, #lhs, #rhs, LVD_CALL_SITE(), ##__VA_ARGS__)
#define SEPT_TEST_REQ_COMPARE_IS_GEQ(lhs, rhs, ...) req::compare_is_geq(req_context, lhs, rhs, #lhs, #rhs, LVD_CALL_SITE(), ##__VA_ARGS__)

#define SEPT_TEST_REQ_COMPARE_DATA(lhs, rhs, expected, ...) req::compare_data(req_context, lhs, rhs, expected, #lhs, #rhs, #expected, LVD_CALL_SITE(), ##__VA_ARGS__)
#define SEPT_TEST_REQ_COMPARE_DATA_IS_EQ(lhs, rhs, ...) req::compare_data_is_eq(req_context, lhs, rhs, #lhs, #rhs, LVD_CALL_SITE(), ##__VA_ARGS__)
#define SEPT_TEST_REQ_COMPARE_DATA_IS_NEQ(lhs, rhs, ...) req::compare_data_is_neq(req_context, lhs, rhs, #lhs, #rhs, LVD_CALL_SITE(), ##__VA_ARGS__)
#define SEPT_TEST_REQ_COMPARE_DATA_IS_LT(lhs, rhs, ...) req::compare_data_is_lt(req_context, lhs, rhs, #lhs, #rhs, LVD_CALL_SITE(), ##__VA_ARGS__)
#define SEPT_TEST_REQ_COMPARE_DATA_IS_LEQ(lhs, rhs, ...) req::compare_data_is_leq(req_context, lhs, rhs, #lhs, #rhs, LVD_CALL_SITE(), ##__VA_ARGS__)
#define SEPT_TEST_REQ_COMPARE_DATA_IS_GT(lhs, rhs, ...) req::compare_data_is_gt(req_context, lhs, rhs, #lhs, #rhs, LVD_CALL_SITE(), ##__VA_ARGS__)
#define SEPT_TEST_REQ_COMPARE_DATA_IS_GEQ(lhs, rhs, ...) req::compare_data_is_geq(req_context, lhs, rhs, #lhs, #rhs, LVD_CALL_SITE(), ##__VA_ARGS__)

template <typename Lhs_, typename Rhs_, typename Expected_>
inline void compare (
    lvd::req::Context &context,
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    Expected_ const &expected,
    std::string const &lhs_description,
    std::string const &rhs_description,
    std::string const &expected_description,
    lvd::CallSite const &call_site,
    std::string const &explanation = std::string()
) {
    auto comparison = sept::compare(lhs, rhs);
    std::string comparison_description{LVD_FMT("compare(" << lhs_description << ", " << rhs_description << ')')};
    lvd::req::verify_condition_4param(context, comparison == expected, LVD_FMT(comparison_description << " == " << expected_description), lhs, lhs_description, rhs, rhs_description, comparison, comparison_description, expected, expected_description, call_site, explanation);
}

template <typename Lhs_, typename Rhs_>
inline void compare_is_eq (
    lvd::req::Context &context,
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    lvd::CallSite const &call_site,
    std::string const &explanation = std::string()
) {
    // Try lhs, rhs
    auto comparison = sept::compare(lhs, rhs);
    std::string comparison_description{LVD_FMT("compare(" << lhs_description << ", " << rhs_description << ')')};
    lvd::req::verify_condition_3param(context, comparison == 0, LVD_FMT(comparison_description << " == 0"), lhs, lhs_description, rhs, rhs_description, comparison, comparison_description, call_site, explanation);

    // Try the other way as well.
    comparison = sept::compare(rhs, lhs);
    comparison_description = LVD_FMT("compare(" << rhs_description << ", " << lhs_description << ')');
    lvd::req::verify_condition_3param(context, comparison == 0, LVD_FMT(comparison_description << " == 0"), rhs, rhs_description, lhs, lhs_description, comparison, comparison_description, call_site, explanation);
}

template <typename Lhs_, typename Rhs_>
inline void compare_is_neq (
    lvd::req::Context &context,
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    lvd::CallSite const &call_site,
    std::string const &explanation = std::string()
) {
    // Try lhs, rhs
    auto comparison = sept::compare(lhs, rhs);
    std::string comparison_description{LVD_FMT("compare(" << lhs_description << ", " << rhs_description << ')')};
    lvd::req::verify_condition_3param(context, comparison != 0, LVD_FMT(comparison_description << " != 0"), lhs, lhs_description, rhs, rhs_description, comparison, comparison_description, call_site, explanation);

    // Try the other way as well.
    comparison = sept::compare(rhs, lhs);
    comparison_description = LVD_FMT("compare(" << rhs_description << ", " << lhs_description << ')');
    lvd::req::verify_condition_3param(context, comparison != 0, LVD_FMT(comparison_description << " != 0"), rhs, rhs_description, lhs, lhs_description, comparison, comparison_description, call_site, explanation);
}

template <typename Lhs_, typename Rhs_>
inline void compare_is_lt (
    lvd::req::Context &context,
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    lvd::CallSite const &call_site,
    std::string const &explanation = std::string()
) {
    // Try lhs, rhs
    auto comparison = sept::compare(lhs, rhs);
    std::string comparison_description{LVD_FMT("compare(" << lhs_description << ", " << rhs_description << ')')};
    lvd::req::verify_condition_3param(context, comparison < 0, LVD_FMT(comparison_description << " < 0"), lhs, lhs_description, rhs, rhs_description, comparison, comparison_description, call_site, explanation);

    // Try the other way as well.
    comparison = sept::compare(rhs, lhs);
    comparison_description = LVD_FMT("compare(" << rhs_description << ", " << lhs_description << ')');
    lvd::req::verify_condition_3param(context, comparison > 0, LVD_FMT(comparison_description << " > 0"), rhs, rhs_description, lhs, lhs_description, comparison, comparison_description, call_site, explanation);
}

template <typename Lhs_, typename Rhs_>
inline void compare_is_leq (
    lvd::req::Context &context,
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    lvd::CallSite const &call_site,
    std::string const &explanation = std::string()
) {
    // Try lhs, rhs
    auto comparison = sept::compare(lhs, rhs);
    std::string comparison_description{LVD_FMT("compare(" << lhs_description << ", " << rhs_description << ')')};
    lvd::req::verify_condition_3param(context, comparison <= 0, LVD_FMT(comparison_description << " <= 0"), lhs, lhs_description, rhs, rhs_description, comparison, comparison_description, call_site, explanation);

    // Try the other way as well.
    comparison = sept::compare(rhs, lhs);
    comparison_description = LVD_FMT("compare(" << rhs_description << ", " << lhs_description << ')');
    lvd::req::verify_condition_3param(context, comparison >= 0, LVD_FMT(comparison_description << " >= 0"), rhs, rhs_description, lhs, lhs_description, comparison, comparison_description, call_site, explanation);
}

template <typename Lhs_, typename Rhs_>
inline void compare_is_gt (
    lvd::req::Context &context,
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    lvd::CallSite const &call_site,
    std::string const &explanation = std::string()
) {
    // Try lhs, rhs
    auto comparison = sept::compare(lhs, rhs);
    std::string comparison_description{LVD_FMT("compare(" << lhs_description << ", " << rhs_description << ')')};
    lvd::req::verify_condition_3param(context, comparison > 0, LVD_FMT(comparison_description << " > 0"), lhs, lhs_description, rhs, rhs_description, comparison, comparison_description, call_site, explanation);

    // Try the other way as well.
    comparison = sept::compare(rhs, lhs);
    comparison_description = LVD_FMT("compare(" << rhs_description << ", " << lhs_description << ')');
    lvd::req::verify_condition_3param(context, comparison < 0, LVD_FMT(comparison_description << " < 0"), rhs, rhs_description, lhs, lhs_description, comparison, comparison_description, call_site, explanation);
}

template <typename Lhs_, typename Rhs_>
inline void compare_is_geq (
    lvd::req::Context &context,
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    lvd::CallSite const &call_site,
    std::string const &explanation = std::string()
) {
    // Try lhs, rhs
    auto comparison = sept::compare(lhs, rhs);
    std::string comparison_description{LVD_FMT("compare(" << lhs_description << ", " << rhs_description << ')')};
    lvd::req::verify_condition_3param(context, comparison >= 0, LVD_FMT(comparison_description << " >= 0"), lhs, lhs_description, rhs, rhs_description, comparison, comparison_description, call_site, explanation);

    // Try the other way as well.
    comparison = sept::compare(rhs, lhs);
    comparison_description = LVD_FMT("compare(" << rhs_description << ", " << lhs_description << ')');
    lvd::req::verify_condition_3param(context, comparison <= 0, LVD_FMT(comparison_description << " <= 0"), rhs, rhs_description, lhs, lhs_description, comparison, comparison_description, call_site, explanation);
}

template <typename Lhs_, typename Rhs_, typename Expected_>
inline void compare_data (
    lvd::req::Context &context,
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    Expected_ const &expected,
    std::string const &lhs_description,
    std::string const &rhs_description,
    std::string const &expected_description,
    lvd::CallSite const &call_site,
    std::string const &explanation = std::string()
) {
    auto comparison = sept::compare_data(lhs, rhs);
    std::string comparison_description{LVD_FMT("compare_data(" << lhs_description << ", " << rhs_description << ')')};
    lvd::req::verify_condition_4param(context, comparison == expected, LVD_FMT(comparison_description << " == " << expected_description), lhs, lhs_description, rhs, rhs_description, comparison, comparison_description, expected, expected_description, call_site, explanation);
}

template <typename Lhs_, typename Rhs_>
inline void compare_data_is_eq (
    lvd::req::Context &context,
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    lvd::CallSite const &call_site,
    std::string const &explanation = std::string()
) {
    // Try lhs, rhs
    auto comparison = sept::compare_data(lhs, rhs);
    std::string comparison_description{LVD_FMT("compare_data(" << lhs_description << ", " << rhs_description << ')')};
    lvd::req::verify_condition_3param(context, comparison == 0, LVD_FMT(comparison_description << " == 0"), lhs, lhs_description, rhs, rhs_description, comparison, comparison_description, call_site, explanation);

    // Try the other way as well.
    comparison = sept::compare_data(rhs, lhs);
    comparison_description = LVD_FMT("compare_data(" << rhs_description << ", " << lhs_description << ')');
    lvd::req::verify_condition_3param(context, comparison == 0, LVD_FMT(comparison_description << " == 0"), rhs, rhs_description, lhs, lhs_description, comparison, comparison_description, call_site, explanation);
}

template <typename Lhs_, typename Rhs_>
inline void compare_data_is_neq (
    lvd::req::Context &context,
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    lvd::CallSite const &call_site,
    std::string const &explanation = std::string()
) {
    // Try lhs, rhs
    auto comparison = sept::compare_data(lhs, rhs);
    std::string comparison_description{LVD_FMT("compare_data(" << lhs_description << ", " << rhs_description << ')')};
    lvd::req::verify_condition_3param(context, comparison != 0, LVD_FMT(comparison_description << " != 0"), lhs, lhs_description, rhs, rhs_description, comparison, comparison_description, call_site, explanation);

    // Try the other way as well.
    comparison = sept::compare_data(rhs, lhs);
    comparison_description = LVD_FMT("compare_data(" << rhs_description << ", " << lhs_description << ')');
    lvd::req::verify_condition_3param(context, comparison != 0, LVD_FMT(comparison_description << " != 0"), rhs, rhs_description, lhs, lhs_description, comparison, comparison_description, call_site, explanation);
}

template <typename Lhs_, typename Rhs_>
inline void compare_data_is_lt (
    lvd::req::Context &context,
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    lvd::CallSite const &call_site,
    std::string const &explanation = std::string()
) {
    // Try lhs, rhs
    auto comparison = sept::compare_data(lhs, rhs);
    std::string comparison_description{LVD_FMT("compare_data(" << lhs_description << ", " << rhs_description << ')')};
    lvd::req::verify_condition_3param(context, comparison < 0, LVD_FMT(comparison_description << " < 0"), lhs, lhs_description, rhs, rhs_description, comparison, comparison_description, call_site, explanation);

    // Try the other way as well.
    comparison = sept::compare_data(rhs, lhs);
    comparison_description = LVD_FMT("compare_data(" << rhs_description << ", " << lhs_description << ')');
    lvd::req::verify_condition_3param(context, comparison > 0, LVD_FMT(comparison_description << " > 0"), rhs, rhs_description, lhs, lhs_description, comparison, comparison_description, call_site, explanation);
}

template <typename Lhs_, typename Rhs_>
inline void compare_data_is_leq (
    lvd::req::Context &context,
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    lvd::CallSite const &call_site,
    std::string const &explanation = std::string()
) {
    // Try lhs, rhs
    auto comparison = sept::compare_data(lhs, rhs);
    std::string comparison_description{LVD_FMT("compare_data(" << lhs_description << ", " << rhs_description << ')')};
    lvd::req::verify_condition_3param(context, comparison <= 0, LVD_FMT(comparison_description << " <= 0"), lhs, lhs_description, rhs, rhs_description, comparison, comparison_description, call_site, explanation);

    // Try the other way as well.
    comparison = sept::compare_data(rhs, lhs);
    comparison_description = LVD_FMT("compare_data(" << rhs_description << ", " << lhs_description << ')');
    lvd::req::verify_condition_3param(context, comparison >= 0, LVD_FMT(comparison_description << " >= 0"), rhs, rhs_description, lhs, lhs_description, comparison, comparison_description, call_site, explanation);
}

template <typename Lhs_, typename Rhs_>
inline void compare_data_is_gt (
    lvd::req::Context &context,
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    lvd::CallSite const &call_site,
    std::string const &explanation = std::string()
) {
    // Try lhs, rhs
    auto comparison = sept::compare_data(lhs, rhs);
    std::string comparison_description{LVD_FMT("compare_data(" << lhs_description << ", " << rhs_description << ')')};
    lvd::req::verify_condition_3param(context, comparison > 0, LVD_FMT(comparison_description << " > 0"), lhs, lhs_description, rhs, rhs_description, comparison, comparison_description, call_site, explanation);

    // Try the other way as well.
    comparison = sept::compare_data(rhs, lhs);
    comparison_description = LVD_FMT("compare_data(" << rhs_description << ", " << lhs_description << ')');
    lvd::req::verify_condition_3param(context, comparison < 0, LVD_FMT(comparison_description << " < 0"), rhs, rhs_description, lhs, lhs_description, comparison, comparison_description, call_site, explanation);
}

template <typename Lhs_, typename Rhs_>
inline void compare_data_is_geq (
    lvd::req::Context &context,
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    lvd::CallSite const &call_site,
    std::string const &explanation = std::string()
) {
    // Try lhs, rhs
    auto comparison = sept::compare_data(lhs, rhs);
    std::string comparison_description{LVD_FMT("compare_data(" << lhs_description << ", " << rhs_description << ')')};
    lvd::req::verify_condition_3param(context, comparison >= 0, LVD_FMT(comparison_description << " >= 0"), lhs, lhs_description, rhs, rhs_description, comparison, comparison_description, call_site, explanation);

    // Try the other way as well.
    comparison = sept::compare_data(rhs, lhs);
    comparison_description = LVD_FMT("compare_data(" << rhs_description << ", " << lhs_description << ')');
    lvd::req::verify_condition_3param(context, comparison <= 0, LVD_FMT(comparison_description << " <= 0"), rhs, rhs_description, lhs, lhs_description, comparison, comparison_description, call_site, explanation);
}

} // end namespace req
