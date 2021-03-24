// 2020.04.05 - Victor Dods

#include <lvd/req.hpp>
#include <lvd/test.hpp>
#include "sept/ArrayTerm.hpp"
#include "sept/Data.hpp"
#include "sept/type/Conversions.hpp"

LVD_TEST_BEGIN(700__type__Conversions__can_convert)
    sept::type::Conversion const *conversion = nullptr;

    LVD_TEST_REQ_IS_TRUE(sept::type::Conversions::SINGLETON.can_convert(sept::Sint64, sept::Sint64));

    conversion = nullptr;
    LVD_TEST_REQ_IS_TRUE(sept::type::Conversions::SINGLETON.can_convert(sept::Sint64, sept::Sint64, &conversion));
    LVD_TEST_REQ_NEQ_NULLPTR(conversion);
LVD_TEST_END

LVD_TEST_BEGIN(700__type__Conversions__conversion_for)
    LVD_TEST_REQ_EQ(
        sept::type::Conversions::SINGLETON.conversion_for(
            sept::Sint32,
            sept::Sint32
        ).source_type(),
        sept::Sint32
    );
    LVD_TEST_REQ_EQ(
        sept::type::Conversions::SINGLETON.convert_to(sept::Float64, sept::Sint64(3)),
        3.0
    );
    LVD_TEST_REQ_EQ(
        sept::type::Conversions::SINGLETON.convert_to(sept::Float64, int64_t(3)),
        3.0
    );
    LVD_TEST_REQ_EQ(
        sept::type::Conversions::SINGLETON.convert_to(sept::Sint64, false),
        int64_t(false)
    );
    LVD_TEST_REQ_EQ(
        sept::type::Conversions::SINGLETON.convert_to(sept::Sint64, true),
        int64_t(true)
    );
//     LVD_TEST_REQ_EQ(sept::type::Conversions::SINGLETON.convert_to<int64_t>(sept::Data(true)), sept::Data_t<int64_t>(1));
//     LVD_TEST_REQ_EQ(sept::type::Conversions::SINGLETON.convert_to<int64_t>(sept::Data(false)), sept::Data_t<int64_t>(0));
LVD_TEST_END
