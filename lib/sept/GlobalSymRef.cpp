// 2021.03.31 - Victor Dods

#include "sept/GlobalSymRef.hpp"

#include <lvd/literal.hpp>
#include "sept/Data.hpp"

namespace sept {

GlobalSymRef_c GlobalSymRef;
GlobalSymRefType_c GlobalSymRefType;

Data const &GlobalSymRefTermImpl::referenced_data () const & {
    return ms_symbol_table->resolve_symbol_const(m_symbol_id);
}

Data &GlobalSymRefTermImpl::referenced_data () & {
    return ms_symbol_table->resolve_symbol_nonconst(m_symbol_id);
}

Data GlobalSymRefTermImpl::move_referenced_data () && {
    LVD_ABORT("GlobalSymRefTermImpl::move_referenced_data has been disabled to see if it's actually necessary");
}

GlobalSymRefTermImpl::operator lvd::OstreamDelegate () const {
    return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
        // Print it as an opaque reference.  Printing through a transparent reference is done by Data methods.
        out << "GlobalSymRef(" << lvd::literal_of(m_symbol_id) << ')';
    });
}

lvd::nnsp<SymbolTable> GlobalSymRefTermImpl::ms_symbol_table{lvd::make_nnsp<SymbolTable>()};

} // end namespace sept
