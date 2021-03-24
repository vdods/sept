// 2020.04.09 - Victor Dods

#include <lvd/req.hpp>
#include <lvd/test.hpp>
#include "sept/ArrayTerm.hpp"
#include "sept/Data.hpp"
#include "sept/proj/ElementExtraction.hpp"
#include "sept/proj/TypeFunctor.hpp"

LVD_TEST_BEGIN(900__proj__ExtractElement__0_single)
    sept::Data array(sept::make_array(1, 4, 9, 25, 36));
    sept::Data index(int64_t(3));

    sept::proj::ElementExtraction ee;
    LVD_TEST_REQ_EQ(ee.project(array, index), sept::Data(25));
    sept::Data pwc_ = ee.project_with_complement(array, index);
    LVD_TEST_REQ_EQ(pwc_, sept::make_array(sept::Data(25), index));
    LVD_TEST_REQ_IS_TRUE(ee.can_embed(pwc_, array));

    sept::Data_t<sept::ArrayTerm_c> &pwc = pwc_.as<sept::ArrayTerm_c>();
    test_log << lvd::Log::dbg() << LVD_REFLECT(pwc) << '\n';

    sept::ArrayTerm_c &pwc_array = pwc_.cast<sept::ArrayTerm_c &>();
    test_log << lvd::Log::dbg() << LVD_REFLECT(pwc_array) << '\n';
    LVD_TEST_REQ_IS_TRUE(ee.can_embed(pwc, array));
    LVD_TEST_REQ_IS_TRUE(ee.can_embed(pwc_, array));
    pwc_array[0] = 100;
    LVD_TEST_REQ_IS_TRUE(ee.can_embed(pwc, array));
    LVD_TEST_REQ_IS_TRUE(ee.can_embed(pwc_, array));
    ee.embed(pwc_, array);
    LVD_TEST_REQ_EQ(array, sept::Data(sept::make_array(1, 4, 9, 100, 36)));
LVD_TEST_END

LVD_TEST_BEGIN(900__proj__ExtractElement__1_multiple)
    sept::Data array(sept::Array(1, 4, 9, 25, 36, 49, 64, 81, 100));
    sept::Data indices(sept::Array(int64_t(1), int64_t(3), int64_t(4)));

    sept::proj::ElementExtraction ee;
    LVD_TEST_REQ_EQ(ee.project(array, indices), sept::Array(4, 25, 36));
    sept::Data pwc_ = ee.project_with_complement(array, indices);
    LVD_TEST_REQ_EQ(pwc_, sept::Array(sept::Array(4, 25, 36), indices));
    LVD_TEST_REQ_IS_TRUE(ee.can_embed(pwc_, array));

    sept::Data_t<sept::ArrayTerm_c> &pwc = pwc_.as<sept::ArrayTerm_c>();
    test_log << lvd::Log::dbg() << LVD_REFLECT(pwc) << '\n';

    sept::ArrayTerm_c &pwc_array = pwc_.cast<sept::ArrayTerm_c &>();
    test_log << lvd::Log::dbg() << LVD_REFLECT(pwc_array) << '\n';
    LVD_TEST_REQ_EQ(pwc_array.size(), size_t(2));
    LVD_TEST_REQ_IS_TRUE(ee.can_embed(pwc, array));
    LVD_TEST_REQ_IS_TRUE(ee.can_embed(pwc_, array));

    sept::ArrayTerm_c &projection_array = pwc_array[0].cast<sept::ArrayTerm_c &>();
    LVD_TEST_REQ_EQ(projection_array, sept::Array(4, 25, 36));
    test_log << lvd::Log::dbg() << "before: " << LVD_REFLECT(pwc_) << '\n';
    test_log << lvd::Log::dbg() << "before: " << LVD_REFLECT(array) << '\n';
    projection_array[0] = 5;
    projection_array[1] = 26;
    projection_array[2] = 38;

    LVD_TEST_REQ_IS_TRUE(ee.can_embed(pwc, array));
    LVD_TEST_REQ_IS_TRUE(ee.can_embed(pwc_, array));
    ee.embed(pwc_, array);
    LVD_TEST_REQ_EQ(array, sept::Array(1, 5, 9, 26, 38, 49, 64, 81, 100));
    test_log << lvd::Log::dbg() << "after: " << LVD_REFLECT(pwc_) << '\n';
    test_log << lvd::Log::dbg() << "after: " << LVD_REFLECT(array) << '\n';
LVD_TEST_END

LVD_TEST_BEGIN(900__proj__TypeFunctor)
    sept::Data n = int64_t(123456);
    sept::Data parameters(0); // arbitrary

    sept::proj::TypeFunctor tf;
    LVD_TEST_REQ_EQ(tf.project(n, parameters), sept::Sint64);
    sept::Data pwc_ = tf.project_with_complement(n, parameters);
    LVD_TEST_REQ_EQ(pwc_, sept::make_array(sept::Sint64, parameters));
    LVD_TEST_REQ_IS_TRUE(tf.can_embed(pwc_, n));

    sept::Data_t<sept::ArrayTerm_c> &pwc = pwc_.as<sept::ArrayTerm_c>();
    test_log << lvd::Log::dbg() << LVD_REFLECT(pwc) << '\n';

    sept::ArrayTerm_c &pwc_array = pwc_.cast<sept::ArrayTerm_c &>();
    test_log << lvd::Log::dbg() << LVD_REFLECT(pwc_array) << '\n';
    LVD_TEST_REQ_IS_TRUE(tf.can_embed(pwc, n));
    LVD_TEST_REQ_IS_TRUE(tf.can_embed(pwc_, n));
    pwc_array[0] = sept::Float64;
    LVD_TEST_REQ_IS_TRUE(tf.can_embed(pwc, n));
    LVD_TEST_REQ_IS_TRUE(tf.can_embed(pwc_, n));
    tf.embed(pwc_, n);
    LVD_TEST_REQ_EQ(n, 123456.0);
LVD_TEST_END
