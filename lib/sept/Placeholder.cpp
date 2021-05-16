// 2020.07.25 - Victor Dods

#include "sept/Placeholder.hpp"

namespace sept {

PlaceholderTerm_c::operator lvd::OstreamDelegate () const {
    return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
        DataPrintCtx ctx;
        print(out, ctx, *this);
    });
}

PlaceholderType_c PlaceholderType;
Placeholder_c Placeholder;

//
// Registrations for Data functions
//

SEPT__REGISTER__PRINT(PlaceholderType_c)
SEPT__REGISTER__PRINT(Placeholder_c)
SEPT__REGISTER__PRINT(PlaceholderTerm_c)

SEPT__REGISTER__HASH(PlaceholderType_c)
SEPT__REGISTER__HASH(Placeholder_c)
SEPT__REGISTER__HASH(PlaceholderTerm_c)

SEPT__REGISTER__EQ(PlaceholderType_c)
SEPT__REGISTER__EQ(Placeholder_c)
SEPT__REGISTER__EQ(PlaceholderTerm_c)

} // end namespace sept
