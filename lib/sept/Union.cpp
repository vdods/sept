// 2020.03.30 - Victor Dods

#include "sept/Union.hpp"

namespace sept {

Union_c Union;
UnionType_c UnionType;

//
// Registrations for Data functions
//

SEPT__REGISTER__PRINT(Union_c)
SEPT__REGISTER__PRINT(UnionType_c)

SEPT__REGISTER__HASH(Union_c)
SEPT__REGISTER__HASH(UnionType_c)

SEPT__REGISTER__EQ(Union_c)
SEPT__REGISTER__EQ(UnionType_c)


SEPT__REGISTER__ABSTRACT_TYPE_OF(Union_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(UnionType_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(UnionTerm_c)


SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(Union_c, Type_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(UnionType_c, Type_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(Union_c, UnionType_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(UnionTerm_c, Union_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(UnionTerm_c, UnionTerm_c)


SEPT__REGISTER__COMPARE__SINGLETON(UnionType_c)
SEPT__REGISTER__COMPARE__SINGLETON(Union_c)


SEPT__REGISTER__SERIALIZE(UnionType_c)
SEPT__REGISTER__SERIALIZE(Union_c)


SEPT__REGISTER__DESERIALIZE(Union_c, return deserialize_value_UnionTerm(std::move(abstract_type), in);)

} // end namespace sept
