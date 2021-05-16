// 2021.05.16 - Victor Dods

#include "sept/FreeVar.hpp"

namespace sept {

FreeVarTerm_c::operator lvd::OstreamDelegate () const {
    return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
        DataPrintCtx ctx;
        print(out, ctx, *this);
    });
}

FreeVarType_c FreeVarType;
FreeVar_c FreeVar;

FreeVarType_c deserialize_value_FreeVarType (Data &&abstract_type, std::istream &in) {
    // No content to read.
    return FreeVarType_c{};
}

FreeVar_c deserialize_value_FreeVar (Data &&abstract_type, std::istream &in) {
    // No content to read.
    return FreeVar_c{};
}

FreeVarTerm_c deserialize_value_FreeVarTerm (Data &&abstract_type, std::istream &in) {
    auto free_var_id = deserialize_data(in);
    return FreeVarTerm_c{std::move(free_var_id)};
}

//
// Registrations for Data functions
//

SEPT__REGISTER__PRINT(FreeVarType_c)
SEPT__REGISTER__PRINT(FreeVar_c)
SEPT__REGISTER__PRINT(FreeVarTerm_c)

SEPT__REGISTER__HASH(FreeVarType_c)
SEPT__REGISTER__HASH(FreeVar_c)
SEPT__REGISTER__HASH(FreeVarTerm_c)

SEPT__REGISTER__EQ(FreeVarType_c)
SEPT__REGISTER__EQ(FreeVar_c)
SEPT__REGISTER__EQ(FreeVarTerm_c)

SEPT__REGISTER__ABSTRACT_TYPE_OF(FreeVarType_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(FreeVar_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(FreeVarTerm_c)

SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(FreeVarType_c, Type_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(FreeVar_c, Type_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(FreeVarTerm_c, Type_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(FreeVar_c, FreeVarType_c)
SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(FreeVarTerm_c, FreeVar_c)
SEPT__REGISTER__INHABITS__DATA(Data, FreeVarTerm_c)

SEPT__REGISTER__COMPARE__SINGLETON(FreeVarType_c)
SEPT__REGISTER__COMPARE__SINGLETON(FreeVar_c)
// This uses compare inherited from BaseArray_S_t.
SEPT__REGISTER__COMPARE(FreeVarTerm_c, FreeVarTerm_c)

// SEPT__REGISTER__SERIALIZE(FreeVarType_c)
// SEPT__REGISTER__SERIALIZE(FreeVar_c)
// SEPT__REGISTER__SERIALIZE(FreeVarTerm_c)

SEPT__REGISTER__DESERIALIZE(FreeVarType_c, return deserialize_value_FreeVarType(std::move(abstract_type), in);)
SEPT__REGISTER__DESERIALIZE(FreeVar_c, return deserialize_value_FreeVar(std::move(abstract_type), in);)
SEPT__REGISTER__DESERIALIZE(FreeVarTerm_c, return deserialize_value_FreeVarTerm(std::move(abstract_type), in);)

} // end namespace sept
