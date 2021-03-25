// 2021.03.24 - Victor Dods

#include "sept/BaseArray_t.hpp"

namespace sept {

//
// Registrations for Data functions
//

SEPT_DATA_PRINTING_REGISTER_TYPE_EXPLICIT(BaseArray_t<>, BaseArray_t__NoArgs)

SEPT_EQ_DATA_REGISTER_TYPE_EXPLICIT(BaseArray_t<>, BaseArray_t__NoArgs)

} // end namespace sept