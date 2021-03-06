// 2021.03.27 - Victor Dods

#include "sept/Tuple.hpp"

namespace sept {

Tuple_c Tuple;
TupleType_c TupleType;

//
// Registrations for Data functions
//

SEPT__REGISTER__PRINT(Tuple_c)
SEPT__REGISTER__PRINT(TupleType_c)

SEPT__REGISTER__HASH(Tuple_c)
SEPT__REGISTER__HASH(TupleType_c)

SEPT__REGISTER__EQ(Tuple_c)
SEPT__REGISTER__EQ(TupleType_c)


SEPT__REGISTER__ABSTRACT_TYPE_OF(Tuple_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(TupleType_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(TupleTerm_c)


SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(Tuple_c, Type_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(TupleType_c, Type_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(Tuple_c, TupleType_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(TupleTerm_c, Tuple_c)
SEPT__REGISTER__INHABITS__NONDATA(TupleTerm_c, TupleTerm_c)


SEPT__REGISTER__COMPARE__SINGLETON(TupleType_c)
SEPT__REGISTER__COMPARE__SINGLETON(Tuple_c)


SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(Tuple_c, TupleTerm_c)


SEPT__REGISTER__SERIALIZE(TupleType_c)
SEPT__REGISTER__SERIALIZE(Tuple_c)


SEPT__REGISTER__DESERIALIZE(Tuple_c, return deserialize_value_TupleTerm(std::move(abstract_type), in);)

} // end namespace sept
