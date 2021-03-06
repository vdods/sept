// 2021.04.02 - Victor Dods

#include "sept/FormalTypeOf.hpp"

namespace sept {

FormalTypeOf_c FormalTypeOf;

FormalTypeOf_Term_c::operator lvd::OstreamDelegate () const {
    return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
        DataPrintCtx ctx;
        print(out, ctx, *this);
    });
}

//
// Registrations for Data functions
//

SEPT__REGISTER__PRINT(FormalTypeOf_Term_c)

SEPT__REGISTER__HASH(FormalTypeOf_Term_c)

SEPT__REGISTER__EQ(FormalTypeOf_Term_c)


SEPT__REGISTER__ABSTRACT_TYPE_OF(FormalTypeOf_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(FormalTypeOf_Term_c)


SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(FormalTypeOf_c, Type_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(FormalTypeOf_Term_c, FormalTypeOf_c)
SEPT__REGISTER__INHABITS__NONDATA(FormalTypeOf_Term_c, FormalTypeOf_Term_c)
SEPT__REGISTER__INHABITS__DATA(Data, FormalTypeOf_Term_c)
//
//
// SEPT__REGISTER__COMPARE__SINGLETON(FormalTypeOfType_c)
// SEPT__REGISTER__COMPARE__SINGLETON(FormalTypeOf_c)
//

SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(FormalTypeOf_Term_c, FormalTypeOf_Term_c)

//
// SEPT__REGISTER__SERIALIZE(FormalTypeOfType_c)
// SEPT__REGISTER__SERIALIZE(FormalTypeOf_c)
//
//
// SEPT__REGISTER__DESERIALIZE(FormalTypeOf_c, return deserialize_value_FormalTypeOfTerm(std::move(abstract_type), in);)

} // end namespace sept
