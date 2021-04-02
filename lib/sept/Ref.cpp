// 2021.03.31 - Victor Dods

#include "sept/Ref.hpp"

namespace sept {

Ref_c Ref;
RefType_c RefType;

//
// Registrations for Data functions
//

SEPT__REGISTER__PRINT(Ref_c)
SEPT__REGISTER__PRINT(RefType_c)

SEPT__REGISTER__EQ(Ref_c)
SEPT__REGISTER__EQ(RefType_c)


SEPT__REGISTER__ABSTRACT_TYPE_OF(Ref_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(RefType_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(RefTerm_c)


SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(Ref_c, Type_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(RefType_c, Type_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(Ref_c, RefType_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(RefTerm_c, Ref_c)


SEPT__REGISTER__COMPARE__SINGLETON(RefType_c)
SEPT__REGISTER__COMPARE__SINGLETON(Ref_c)


SEPT__REGISTER__SERIALIZE(RefType_c)
SEPT__REGISTER__SERIALIZE(Ref_c)


// SEPT__REGISTER__DESERIALIZE(Ref_c, return deserialize_value_RefTerm(std::move(abstract_type), in);)

} // end namespace sept
