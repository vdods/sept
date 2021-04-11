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
        // TODO: This will print a cyclic reference in an infinite loop unless something stops it
        out << "MemRefTermImpl(" << *m_ptr << ')';
    });
}

} // end namespace sept
