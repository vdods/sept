// 2021.03.31 - Victor Dods

#include "sept/MemRef.hpp"

#include "sept/Data.hpp"

namespace sept {

MemRef_c MemRef;
MemRefType_c MemRefType;

Data MemRefTermImpl::move_referenced_data () && {
//     return std::move(*m_ptr);
    LVD_ABORT("MemRefTermImpl::move_referenced_data has been disabled to see if it's actually necessary");
}

MemRefTermImpl::operator lvd::OstreamDelegate () const {
    return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
        // Print it as an opaque reference.  Printing through a transparent reference is done by Data methods.
        out << "MemRef(" << m_ptr << ')';
    });
}

} // end namespace sept
