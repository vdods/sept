// 2021.03.31 - Victor Dods

#include "sept/LocalSymRef.hpp"

#include <lvd/literal.hpp>
#include "sept/Data.hpp"

namespace sept {

LocalSymRef_c LocalSymRef;
LocalSymRefType_c LocalSymRefType;

//
// LocalSymRefTermImpl
//

Data const &LocalSymRefTermImpl::referenced_data () const & {
    return m_symbol_table->resolve_symbol_const(m_symbol_id);
}

Data &LocalSymRefTermImpl::referenced_data () & {
    return m_symbol_table->resolve_symbol_nonconst(m_symbol_id);
}

Data LocalSymRefTermImpl::move_referenced_data () && {
    LVD_ABORT("LocalSymRefTermImpl::move_referenced_data has been disabled to see if it's actually necessary");
}

LocalSymRefTermImpl::operator lvd::OstreamDelegate () const {
    return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
        // Print it as an opaque reference.  Printing through a transparent reference is done by Data methods.
        out << "LocalSymRef(" << lvd::literal_of(m_symbol_id) << ", " << m_symbol_table.get() << ')';
    });
}

} // end namespace sept
