// 2020.08.04 - Victor Dods

#include "sept/ctl/ClearOutput.hpp"

namespace sept {
namespace ctl {

ClearOutputType_c ClearOutputType;
ClearOutput_c ClearOutput;

//
// Registrations for Data functions
//

SEPT__REGISTER__PRINT(ClearOutputType_c)
SEPT__REGISTER__PRINT(ClearOutput_c)

SEPT__REGISTER__EQ(ClearOutputType_c)
SEPT__REGISTER__EQ(ClearOutput_c)

SEPT__REGISTER__ABSTRACT_TYPE_OF(ClearOutputType_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(ClearOutput_c)

SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(ClearOutput_c, ClearOutputType_c)

SEPT__REGISTER__SERIALIZE(ClearOutputType_c)
SEPT__REGISTER__SERIALIZE(ClearOutput_c)

} // end namespace ctl
} // end namespace sept
