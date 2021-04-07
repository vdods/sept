// 2021.03.30 - Victor Dods

#include <lvd/test.hpp>
#include "req.hpp"
#include "sept/Union.hpp"
#include "sept/UnionTerm.hpp"
#include "sept/NPType.hpp"
#include "sept/MemRef.hpp"

LVD_TEST_BEGIN(571__Union__0)
    // First, compare exact identity of the singletons involved.
    LVD_TEST_REQ_EQ(&sept::abstract_type_of(sept::Union), &sept::UnionType);
    LVD_TEST_REQ_EQ(&sept::abstract_type_of(sept::UnionType), &sept::NonParametricType);

    // Now, compare values
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Union), sept::UnionType);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::UnionType), sept::NonParametricType);

    // Test Union's instantiation.
    LVD_TEST_REQ_EQ(
        sept::Union(sept::TrueType, sept::Uint32, sept::Array),
        sept::UnionTerm_c(sept::DataVector{sept::TrueType, sept::Uint32, sept::Array})
    );

    // Test abstract_type_of
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::Union(sept::Uint32, sept::Float64)), sept::Union);
LVD_TEST_END

LVD_TEST_BEGIN(571__Union__inhabits)
    auto Union_Empty = sept::Union();
    auto Union_Uint32 = sept::Union(sept::Uint32);
    auto Union_Float32_Bool = sept::Union(sept::Float32, sept::Bool);
    auto Union_Float32Type_BoolType = sept::Union(sept::Float32Type, sept::BoolType);

    LVD_TEST_REQ_IS_TRUE(!sept::inhabits(sept::Uint32(10), Union_Empty));

    LVD_TEST_REQ_IS_TRUE(sept::inhabits(sept::Uint32(10), Union_Uint32));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits(uint32_t(10), Union_Uint32));
    LVD_TEST_REQ_IS_TRUE(!sept::inhabits(sept::Uint16(10), Union_Uint32));
    LVD_TEST_REQ_IS_TRUE(!sept::inhabits(sept::True, Union_Uint32));
    LVD_TEST_REQ_IS_TRUE(!sept::inhabits(sept::Array, Union_Uint32));

    LVD_TEST_REQ_IS_TRUE(sept::inhabits(sept::Float32(10.5), Union_Float32_Bool));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits(sept::Bool(true), Union_Float32_Bool));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits(sept::True, Union_Float32_Bool));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits(sept::False, Union_Float32_Bool));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits(true, Union_Float32_Bool));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits(false, Union_Float32_Bool));
    LVD_TEST_REQ_IS_TRUE(!sept::inhabits(uint32_t(10), Union_Float32_Bool));

    LVD_TEST_REQ_IS_TRUE(sept::inhabits(sept::Float32, Union_Float32Type_BoolType));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits(sept::Bool, Union_Float32Type_BoolType));
    LVD_TEST_REQ_IS_TRUE(!sept::inhabits(sept::Float32(10.5), Union_Float32Type_BoolType));
    LVD_TEST_REQ_IS_TRUE(!sept::inhabits(true, Union_Float32Type_BoolType));
    LVD_TEST_REQ_IS_TRUE(!sept::inhabits(false, Union_Float32Type_BoolType));
    LVD_TEST_REQ_IS_TRUE(!sept::inhabits(sept::Float32Type, Union_Float32Type_BoolType));
    LVD_TEST_REQ_IS_TRUE(!sept::inhabits(sept::BoolType, Union_Float32Type_BoolType));

//     // TODO: Figure out if this really makes sense.
//     LVD_TEST_REQ_IS_TRUE(sept::inhabits(Union_Float32_Bool, Union_Float32Type_BoolType));
LVD_TEST_END

LVD_TEST_BEGIN(571__Union__inhabits_via_Ref)
    auto u = sept::Union(sept::Float32, sept::Uint64);
    test_log << lvd::Log::trc() << LVD_REFLECT(u) << '\n';
    auto du = sept::Data(u);
    test_log << lvd::Log::trc() << LVD_REFLECT(du) << '\n';
    auto rdu = sept::MemRef(&du);
    test_log << lvd::Log::trc() << LVD_REFLECT(rdu) << '\n';
    auto drdu = sept::Data(rdu);

    LVD_TEST_REQ_IS_TRUE(sept::inhabits(sept::Uint64(10), u));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits_data(sept::Uint64(10), du));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits_data(sept::Uint64(10), rdu));
    LVD_TEST_REQ_IS_TRUE(sept::inhabits_data(sept::Uint64(10), drdu));
LVD_TEST_END
