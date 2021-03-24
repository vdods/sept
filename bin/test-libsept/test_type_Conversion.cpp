// 2020.04.04 - Victor Dods

#include <lvd/req.hpp>
#include <lvd/test.hpp>
#include "sept/ArrayTerm.hpp"
#include "sept/Data.hpp"
#include "sept/type/Conversion.hpp"

LVD_TEST_BEGIN(600__type__Conversion__typeid_uniqueness)
    // The cppreference docs say about the typeid operator:
    //
    //     The typeid expression is an lvalue expression which refers to an object with static storage
    //     duration, of the polymorphic type const std::type_info or of some type derived from it.
    //
    // so presumably the reference that typeid(T) returns refers to a singleton.  This test is just
    // a simple verification of that fact for a few types as a smoke test.

    LVD_TEST_REQ_EQ(&typeid(int64_t), &typeid(int64_t));
    LVD_TEST_REQ_EQ(&typeid(double), &typeid(double));
    LVD_TEST_REQ_EQ(&typeid(char), &typeid(char));
    LVD_TEST_REQ_EQ(&typeid(bool), &typeid(bool));
    LVD_TEST_REQ_EQ(&typeid(sept::Data), &typeid(sept::Data));
    LVD_TEST_REQ_EQ(&typeid(sept::ArrayTerm_c), &typeid(sept::ArrayTerm_c));

    LVD_TEST_REQ_NEQ(&typeid(int64_t), &typeid(double));
LVD_TEST_END

LVD_TEST_BEGIN(600__type__Conversion__identity)
    auto ident_int32 = sept::type::make_identity_conversion(sept::Sint32);
    LVD_TEST_REQ_EQ(ident_int32(3), 3);

    auto ident_double = sept::type::make_identity_conversion(sept::Float64);
    LVD_TEST_REQ_EQ(ident_double(12.25), 12.25);
LVD_TEST_END
