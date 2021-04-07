// 2021.03.31 - Victor Dods

#include "sept/GlobalSymRef.hpp"

#include "sept/Data.hpp"

namespace sept {

GlobalSymRef_c GlobalSymRef;
GlobalSymRefType_c GlobalSymRefType;

Data const &GlobalSymRefTermImpl::referenced_data () const & {
    static Data const s_unresolved_symbol{std::string("<unresolved-symbol>")};
    auto it = ms_symbol_table.find(m_symbol_id);
    return it == ms_symbol_table.end() ? s_unresolved_symbol : it->second;
}

Data &GlobalSymRefTermImpl::referenced_data () & {
    static Data s_unresolved_symbol{std::string("<unresolved-symbol>")};
    auto it = ms_symbol_table.find(m_symbol_id);
    return it == ms_symbol_table.end() ? s_unresolved_symbol : it->second;
}

Data GlobalSymRefTermImpl::referenced_data () && {
    LVD_ABORT("rvalue-ref accessing form of GlobalSymRefTermImpl::referenced_data has been disabled to see if it's actually necessary");
}

GlobalSymRefTermImpl::operator lvd::OstreamDelegate () const {
    return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
        out << "GlobalSymRefTermImpl(" << m_symbol_id << ')';
    });
}

std::unordered_map<std::string,Data> GlobalSymRefTermImpl::ms_symbol_table;

} // end namespace sept
