// 2020.08.04 - Victor Dods

#include "sept/ctl/ClearOutput.hpp"

namespace sept {
namespace ctl {

ClearOutputType_c ClearOutputType;
ClearOutput_c ClearOutput;

//
// Registrations for Data functions
//

SEPT_DATA_OPERATOR_EQ_REGISTER_TYPE(ClearOutputType_c)
SEPT_DATA_OPERATOR_EQ_REGISTER_TYPE(ClearOutput_c)

} // end namespace ctl
} // end namespace sept
