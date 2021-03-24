// 2020.07.25 - Victor Dods

#include "sept/Placeholder.hpp"

namespace sept {

PlaceholderType_c PlaceholderType;
Placeholder_c Placeholder;

//
// Registrations for Data functions
//

SEPT_DATA_OPERATOR_EQ_REGISTER_TYPE(PlaceholderType_c)
SEPT_DATA_OPERATOR_EQ_REGISTER_TYPE(Placeholder_c)
SEPT_DATA_OPERATOR_EQ_REGISTER_TYPE(PlaceholderTerm_c)

} // end namespace sept
