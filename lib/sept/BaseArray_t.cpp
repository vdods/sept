// 2021.03.24 - Victor Dods

#include "sept/BaseArray_t.hpp"

namespace sept {

//
// Registrations for Data functions
//

SEPT__REGISTER__PRINT__GIVE_ID(BaseArray_t<>, BaseArray_t__NoArgs)

SEPT__REGISTER__HASH__GIVE_ID(BaseArray_t<>, BaseArray_t__NoArgs)

SEPT__REGISTER__EQ__GIVE_ID(BaseArray_t<>, BaseArray_t__NoArgs)

} // end namespace sept
