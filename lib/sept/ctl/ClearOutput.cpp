// 2020.08.04 - Victor Dods

#include "sept/ctl/ClearOutput.hpp"

namespace sept {
namespace ctl {

ClearOutputType_c ClearOutputType;
ClearOutput_c ClearOutput;

//
// Registrations for Data functions
//

SEPT_DATA_PRINTING_REGISTER_TYPE(ClearOutputType_c)
SEPT_DATA_PRINTING_REGISTER_TYPE(ClearOutput_c)

SEPT_EQ_DATA_REGISTER_TYPE(ClearOutputType_c)
SEPT_EQ_DATA_REGISTER_TYPE(ClearOutput_c)

SEPT_INHABITS_DATA_REGISTER_TYPE_TRIVIAL(ClearOutput_c, ClearOutputType_c)

} // end namespace ctl
} // end namespace sept
