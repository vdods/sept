// 2020.08.09 - Victor Dods

#include "sept/ctl/EndOfFile.hpp"

namespace sept {
namespace ctl {

EndOfFileType_c EndOfFileType;
EndOfFile_c EndOfFile;

//
// Registrations for Data functions
//

SEPT__REGISTER__PRINT(EndOfFileType_c)
SEPT__REGISTER__PRINT(EndOfFile_c)

SEPT__REGISTER__EQ(EndOfFileType_c)
SEPT__REGISTER__EQ(EndOfFile_c)

SEPT__REGISTER__ABSTRACT_TYPE_OF(EndOfFileType_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(EndOfFile_c)

SEPT__REGISTER__INHABITS__NONDATA__UNCONDITIONAL(EndOfFile_c, EndOfFileType_c)

SEPT__REGISTER__SERIALIZE(EndOfFileType_c)
SEPT__REGISTER__SERIALIZE(EndOfFile_c)

} // end namespace ctl
} // end namespace sept
