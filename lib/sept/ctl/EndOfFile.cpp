// 2020.08.09 - Victor Dods

#include "sept/ctl/EndOfFile.hpp"

namespace sept {
namespace ctl {

EndOfFileType_c EndOfFileType;
EndOfFile_c EndOfFile;

//
// Registrations for Data functions
//

SEPT_DATA_PRINTING_REGISTER_TYPE(EndOfFileType_c)
SEPT_DATA_PRINTING_REGISTER_TYPE(EndOfFile_c)

SEPT_EQ_DATA_REGISTER_TYPE(EndOfFileType_c)
SEPT_EQ_DATA_REGISTER_TYPE(EndOfFile_c)

SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_DEFAULT(EndOfFileType_c)
SEPT_ABSTRACT_TYPE_OF_DATA_REGISTER_TYPE_DEFAULT(EndOfFile_c)

SEPT_INHABITS_DATA_REGISTER_TYPE_TRIVIAL(EndOfFile_c, EndOfFileType_c)

} // end namespace ctl
} // end namespace sept
