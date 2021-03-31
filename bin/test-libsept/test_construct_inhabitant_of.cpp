// 2021.03.29 - Victor Dods

#include <lvd/req.hpp>
#include <lvd/test.hpp>
#include "sept/Data.hpp"
#include "sept/NPType.hpp"
#include "sept/Tuple.hpp"
#include "sept/TupleTerm.hpp"

LVD_TEST_BEGIN(460__construct_inhabitant_of__0)
    sept::Data bool_data{sept::Bool};
    LVD_TEST_REQ_EQ(bool_data(true), sept::Data{true});

    sept::Data tuple_data{sept::Tuple(sept::Uint32, sept::Float64, sept::Bool)};
    LVD_TEST_REQ_EQ(
        tuple_data(sept::Tuple(uint32_t(123), 12.45, true)),
        sept::Tuple(sept::Uint32(123), sept::Float64(12.45), true)
    );
LVD_TEST_END
